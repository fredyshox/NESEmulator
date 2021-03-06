//
// PPU rendering engine
// Copyright (c) 2019 Kacper Rączy
//

#include "ppu/renderer.h"

// ppu_shift_storage functions

struct ppu_shift_storage* ppu_shift_storage_create(void) {
  struct ppu_shift_storage* first = malloc(sizeof(struct ppu_shift_storage));
  struct ppu_shift_storage* prev = first;
  struct ppu_shift_storage* next;
  for (int i = 0; i < 2; i++) {
    next = malloc(sizeof(struct ppu_shift_storage));
    prev->next = next;
    prev = next;
  }
  prev->next = first;

  return first;
}

void ppu_shift_storage_free(struct ppu_shift_storage* pss) {
  struct ppu_shift_storage* temp1 = pss->next;
  struct ppu_shift_storage* temp2;
  while (temp1 != NULL && temp1 != pss) {
    temp2 = temp1->next;
    free(temp1);
    temp1 = temp2;
  }
}

// ppu_render_handle funcitons

struct ppu_render_handle* ppu_render_handle_create(void) {
  struct ppu_render_handle* handle = malloc(sizeof(struct ppu_render_handle));
  handle->frame = malloc(sizeof(uint8_t) * HORIZONTAL_RES * VERTICAL_RES * 3);
  handle->vframe = malloc(sizeof(uint8_t) * HORIZONTAL_RES * VERTICAL_RES * 3); 
  handle->spr_pixel_buf = malloc(sizeof(uint8_t) * HORIZONTAL_RES);
  handle->spr_buffer = malloc(sizeof(struct ppu_sprite) * MAX_SPRITES_PER_LINE);
  handle->frame_buf_pos = 0;
  handle->cycle = 340;
  handle->line = 240;
  handle->fetch_storage = ppu_shift_storage_create();
  handle->render_storage = handle->fetch_storage->next;
  handle->nmi_trigger = false;
  handle->odd_frame = false;

  return handle;
}

void ppu_render_handle_free(struct ppu_render_handle* handle) {
  free(handle->frame);
  free(handle->vframe);
  free(handle->spr_pixel_buf);
  free(handle->spr_buffer);
  ppu_shift_storage_free(handle->fetch_storage);
}

// memory helpers

uint8_t ppu_memory_fetch_pt(struct ppu_memory* mem, uint16_t address, int pt_index) {
  assert(pt_index == 0 || pt_index == 1);
  assert(address < 0x1000);

  uint16_t offset = (pt_index == 1) ? 0x1000 : 0x0000;
  return mem->load_handler(mem, address + offset);
}

// cool trick for fetching from attr blocks
uint8_t ppu_memory_fetch_at(struct ppu_memory* mem, union ppu_internal_register v) {
  uint16_t vaddress = v.address;
  uint16_t attr_address = 0x23C0 | (vaddress & 0x0c00) | ((vaddress >> 4) & 0x38) | ((vaddress >> 2) & 0x07);
  uint8_t shift = ((v.y_scroll << 1) & 0x04) | (v.x_scroll & 0x02); // 2 * 0bYX where Y is 1st bit of y and X is 1st bit of x
  uint8_t attr  = ppu_memory_load(mem, attr_address);
  return (attr >> shift) & 0x03;
}

uint8_t ppu_memory_fetch_nt(struct ppu_memory* mem, union ppu_internal_register v) {
  uint16_t nt_addr = 0x2000 | (v.address & 0x0fff);
  return ppu_memory_load(mem, nt_addr);
}

// sprite helpers

/**
 * Evaluates first 8 sprites to appear on scanline.
 * Sprites are ordered based on priority (position in vram)
 */
int ppu_evaluate_sprites(struct ppu_state* ppu, struct ppu_sprite* output, int outlen, int y_coord) {
  // counters
  int ramc = 0, outc = 0;
  struct ppu_sprite sprite;
  // offset for 16x8 sprites
  int offset = ppu->control.spr_size_16x8 ? 2 * TILE_SIZE : TILE_SIZE;
  // go through whole sprite ram or outlen and 1 more for sprite overflow detection
  while (ramc < PPU_SPRRAM_SIZE && outc <= outlen) {
    sprite = ppu->memory->sprite_ram[ramc];
    int spr_y_coord = (int) sprite.y_coord;
    if (y_coord >= spr_y_coord && y_coord < (spr_y_coord + offset)) {
      if (outc != outlen)
        output[outc] = sprite;
      outc += 1;
    }
    ramc += 1;
  }

  // sprite overflow
  if (outc > outlen) {
    ppu->status.sprite_ovf = 1;
    return outlen;
  } else {
    return outc;
  }
}

/**
 * Pixel layout for scanline using sprite buffer
 * Each value in pixel buffer has format:
 * _ S S S C C C C 
 * Where S is index in sprbuffer, C is palette index.
 * Value is equal to zero if pixel is transparent.
 */
void ppu_sprite_pixel_layout(struct ppu_state* ppu, struct ppu_sprite* sprites, int sprlen, uint8_t* buffer, int bufsize, int y_coord) {
  memset(buffer, 0, bufsize);
  ppu_sprite spr;
  uint8_t spr_tile_lower0, spr_tile_lower1, spr_tile_upper, color_idx;
  bool spr_size_16x8 = ppu->control.spr_size_16x8;
  int pttrntable_idx = ppu->control.spr_pttrntable & 0x01;
  int pttrntable_address;
  int spr_tile_y;
  for (int i = 0; i < sprlen; i++) {
    spr = sprites[i];
    spr_tile_y = y_coord - (int) spr.y_coord;
    if (spr_size_16x8) {
      pttrntable_idx = spr.index & 0x01;
      pttrntable_address = (spr.index & 0xfe) * 0x10; // *16
      if (spr_tile_y >= TILE_SIZE) {
        spr_tile_y = spr_tile_y - TILE_SIZE;
        pttrntable_address += (spr.vflip) ? 0x00 : 0x10; // +0/16
      }
    } else {
      pttrntable_address = spr.index * 0x10; // *16
    }

    if (spr.vflip) {
      spr_tile_y = TILE_SIZE - 1 - spr_tile_y;
    }
    
    spr_tile_lower0 = ppu_memory_fetch_pt(ppu->memory, pttrntable_address + spr_tile_y, pttrntable_idx);
    spr_tile_lower1 = ppu_memory_fetch_pt(ppu->memory, pttrntable_address + spr_tile_y + TILE_SIZE, pttrntable_idx);
    if (spr.hflip) {
      spr_tile_lower0 = utility_bit_reverse(spr_tile_lower0);
      spr_tile_lower1 = utility_bit_reverse(spr_tile_lower1);
    }

    spr_tile_upper = ((spr.palette_msb << 2) & 0x0c);
    spr_tile_upper |= ((uint8_t) i << 4);

    for (int x = spr.x_coord; x < spr.x_coord + TILE_SIZE && x < bufsize; x++) {
      // check if buffer has already opaque pixel value
      if (buffer[x] != 0) {
        goto tile_bitshift;
      }

      color_idx = 0x00;
      color_idx |= ((spr_tile_lower1 >> 6) & 0x02);
      color_idx |= ((spr_tile_lower0 >> 7) & 0x01);
      // if lower bits are zero -> color is transparent
      if (color_idx == 0) {
        goto tile_bitshift;
      }

      color_idx |= spr_tile_upper;
      buffer[x] = color_idx;
      tile_bitshift:
        spr_tile_lower0 <<= 1;
        spr_tile_lower1 <<= 1; 
    }
  }
}

// color utility

int ppu_update_frame(uint8_t* frame, struct ppu_color color, int pos) {
  frame[pos] = color.red;
  frame[pos + 1] = color.green;
  frame[pos + 2] = color.blue;

  return 3;
}

// rendering process

void ppu_render_pixel(struct ppu_state* ppu, struct ppu_render_handle* handle) {
  uint8_t fine_x = ppu->fine_x & 0x07;
  struct ppu_shift_storage* storage = handle->render_storage;
  bool hide_left_bg = handle->cycle <= 8 && !ppu->mask.bg_l8;
  uint16_t bg_color_address;
  uint32_t bg_tiles = (uint32_t) (storage->bg_tiles >> 32);
  if (ppu->mask.show_bg && !hide_left_bg) {
    bg_color_address = 0x3f00 | (uint16_t) ((bg_tiles >> ((TILE_SIZE - fine_x - 1) << 2)) & 0x0F); // (7 - x_fine) * 4
  } else {
    bg_color_address = 0x3f00;
  }

  uint8_t spr_layout = handle->spr_pixel_buf[handle->cycle - 1];
  struct ppu_sprite spr = handle->spr_buffer[(spr_layout >> 4) & 0x07];
  bool hide_left_spr = handle->cycle <= 8 && !ppu->mask.spr_l8;
  uint16_t spr_color_address;
  if (ppu->mask.show_spr && !hide_left_spr) {
    spr_color_address = 0x3f10 | (uint16_t) (spr_layout & 0x0f);
  } else {
    spr_color_address = 0x3f10;
  }

  bool bg_transparent = (bg_color_address & 0x0003) == 0;
  bool spr_transparent = (spr_color_address & 0x0003) == 0;
  uint16_t color_address;
  if (bg_transparent && spr_transparent) {
    color_address = 0x3f00;
  } else if (bg_transparent) {
    color_address = spr_color_address;
  } else if (spr_transparent) {
    color_address = bg_color_address;
  } else {
    color_address = (spr.priority) ? bg_color_address : spr_color_address;
    if (spr.ram_index == 0 && ppu->status.sprite_zhit == 0 && handle->cycle != 256) {
      ppu->status.sprite_zhit = 1; 
    }
  }

  //uint16_t color_address = bg_color_address;
  struct ppu_color color = NES_COLOR(ppu_memory_load(ppu->memory, color_address));
  handle->frame_buf_pos += ppu_update_frame(handle->frame, color, handle->frame_buf_pos);
  storage->bg_tiles <<= 4;
}

void ppu_increment_x(struct ppu_state* ppu) {
  ppu->v.x_scroll += 1;
  if (ppu->v.x_scroll == 0) {
    ppu->v.nametable_addr ^= 1;
    //ppu->v.address ^= 0x0400;
  }
}

void ppu_increment_y(struct ppu_state* ppu) {
  if (ppu->fine_y < 7) {
    ppu->fine_y += 1;
  } else {
    ppu->fine_y = 0;
    uint8_t y = ppu->v.y_scroll;
    if (y == 29) {
      ppu->v.y_scroll = 0;
      ppu->v.nametable_addr ^= 2;
      //ppu->v.address ^= 0x0800;
    } else {
      ppu->v.y_scroll += 1;
    }
  }
}

void ppu_copy_x(struct ppu_state* ppu) {
  ppu->v.x_scroll = ppu->t.x_scroll;
  ppu->v.nametable_addr = ((ppu->v.nametable_addr & 0x02) | (ppu->t.nametable_addr & 0x01));
}

void ppu_copy_y(struct ppu_state* ppu) {
  ppu->v.y_scroll = ppu->t.y_scroll;
  ppu->v.nametable_addr = ((ppu->v.nametable_addr & 0x01) | (ppu->t.nametable_addr & 0x02));
  ppu->fine_y = ppu->temp_fine_y;
}

void ppu_shift(struct ppu_render_handle* handle) {
  struct ppu_shift_storage* old_fstorage = handle->fetch_storage;
  struct ppu_shift_storage* new_rstorage = handle->render_storage->next;
  uint64_t bg_tiles = new_rstorage->bg_tiles;
  bg_tiles = (bg_tiles & 0xffffffff00000000);
  bg_tiles = (bg_tiles | ((old_fstorage->bg_tiles >> 32) & 0xffffffff));
  new_rstorage->bg_tiles = bg_tiles;
  handle->fetch_storage = old_fstorage->next;
  handle->render_storage = new_rstorage;
}

void ppu_storage_compile(struct ppu_shift_storage* storage) {
  uint32_t value = 0;
  uint8_t attr = (storage->bg_tile_upper << 2) & 0x0c, pttr;
  uint8_t lower0 = storage->bg_tile_lower0, lower1 = storage->bg_tile_lower1;
  for (int i = 0; i < 8; i++) {
    value <<= 4;
    pttr  = ((lower1 >> 6) & 0x02);
    pttr |= ((lower0 >> 7) & 0x01);
    lower1 <<= 1;
    lower0 <<= 1;
    value |= (uint32_t) (attr | pttr);
  }
  storage->bg_tiles = ((uint64_t) value) << 32;  
}

void ppu_prepare_next_frame(struct ppu_render_handle* handle) {
  handle->odd_frame = !handle->odd_frame;
  uint8_t* temp = handle->frame;
  handle->frame = handle->vframe;
  handle->vframe = temp;
}

void ppu_execute_cycle(struct ppu_state* ppu, struct ppu_render_handle* handle) {
  bool renderingEnabled = ppu->mask.show_bg || ppu->mask.show_spr;
  if (renderingEnabled) {
    if (handle->odd_frame && handle->line == 261 && handle->cycle == 339) {
      handle->cycle = 0;
      handle->line = 0;
      ppu_prepare_next_frame(handle);
      return;
    }
  }

  handle->cycle += 1;
  if (handle->cycle > 340) {
    handle->cycle = 0;
    handle->line += 1;
    if (handle->line > 261) {
      handle->line = 0;
      ppu_prepare_next_frame(handle);
    }
  }

  bool frameBegin = handle->cycle == 1 && handle->line == 0;
  bool visibleFrame  = handle->line >= 0 && handle->line <= 239;
  bool visibleCycle  = handle->cycle >= 1 && handle->cycle <= 256;
  bool preFetchCycle = handle->cycle >= 321 && handle->cycle <= 336;
  bool preRenderLine = handle->line == 261;
  bool fetchCycle = visibleCycle || preFetchCycle;

  if (frameBegin) {
    // reset frame buffer position
    handle->frame_buf_pos = 0;
  }

  // check if rendering is enabled 
  int bg_ptable = ppu->control.bg_pttrntable & 0x01;
  if (renderingEnabled) {
    if (visibleFrame && visibleCycle) {
      // just draw stuff
      ppu_render_pixel(ppu, handle);
    }
    if ((visibleFrame || preRenderLine) && fetchCycle) {
      struct ppu_shift_storage* storage = handle->fetch_storage;
      switch (handle->cycle & 0b111) {
        case 0:
          // compile fetched data into ready to use form
          ppu_storage_compile(storage);
          // shift storage as this is last fetch cycle
          ppu_shift(handle);
          break;
        case 1:
          storage->bg_pt_index = ppu_memory_fetch_nt(ppu->memory, ppu->v);
          break;
        case 3:
          storage->bg_tile_upper = ppu_memory_fetch_at(ppu->memory, ppu->v);
          break;
        // lower bits for each pixel in tile
        case 5:
          storage->bg_tile_lower0 = ppu_memory_fetch_pt(ppu->memory, (uint16_t) storage->bg_pt_index * 16 + (uint16_t) ppu->fine_y, bg_ptable);
          break;
        case 7:
          storage->bg_tile_lower1 = ppu_memory_fetch_pt(ppu->memory, (uint16_t) storage->bg_pt_index * 16 + (uint16_t) ppu->fine_y + TILE_SIZE, bg_ptable);
          break;
        default: break;
      }
    }
    if (visibleFrame || preRenderLine) {
      if (fetchCycle && (handle->cycle & 0b111) == 0) {
        // inc x
        ppu_increment_x(ppu);
      } 

      if (handle->cycle == 256) {
        // inc y
        ppu_increment_y(ppu);
      }

      if (handle->cycle == 257) {
        // copy x
        ppu_copy_x(ppu);
      }
    }

    if (preRenderLine && handle->cycle >= 280 && handle->cycle <= 304) {
      // copy y
      ppu_copy_y(ppu);
    } 
  }

  // evaluate sprites on 257
  if ((visibleFrame) && handle->cycle == 257) {
    uint8_t y_coord = (ppu->v.y_scroll << 3) + ppu->fine_y;
    handle->sprbuf_size = ppu_evaluate_sprites(ppu, handle->spr_buffer, MAX_SPRITES_PER_LINE, y_coord);
    ppu_sprite_pixel_layout(ppu, handle->spr_buffer, handle->sprbuf_size, handle->spr_pixel_buf, HORIZONTAL_RES, y_coord);
  }

  // flags (vblank, zhit, ovf)
  if (handle->line == 241 && handle->cycle == 1) {
    ppu->status.vblank = 1;
    handle->nmi_trigger = true;
  } else if (preRenderLine && handle->cycle == 1) {
    ppu->status.vblank = 0;
    ppu->status.sprite_zhit = 0;
    ppu->status.sprite_ovf = 0;
    handle->nmi_trigger = false;
  }
}
