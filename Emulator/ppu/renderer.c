//
// PPU rendering engine
// Copyright (c) 2019 Kacper Rączy
//

#include "ppu/renderer.h"

// ppu_shift_storage functions

struct ppu_shift_storage* ppu_shift_storage_create() {
  struct ppu_shift_storage* first = malloc(sizeof(struct ppu_shift_storage*)); 
  struct ppu_shift_storage* prev = first;
  struct ppu_shift_storage* next;
  for (int i = 0; i < 2; i++) {
    next = malloc(sizeof(struct ppu_shift_storage*));
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

struct ppu_render_handle* ppu_render_handle_create() {
  struct ppu_render_handle* handle = malloc(sizeof(struct ppu_render_handle));
  handle->frame = malloc(sizeof(uint8_t) * HORIZONTAL_RES * VERTICAL_RES * 3);
  handle->spr_pixel_buf = malloc(sizeof(uint8_t) * HORIZONTAL_RES);
  handle->spr_buffer = malloc(sizeof(struct ppu_sprite) * MAX_SPRITES_PER_LINE);
  handle->frame_buf_pos = 0;
  handle->cycle = 340;
  handle->line = 240;
  handle->fetch_storage = ppu_shift_storage_create();
  handle->render_storage = handle->fetch_storage->next;
  handle->nmi_trigger = false;

  return handle;
}

void ppu_render_handle_free(struct ppu_render_handle* handle) {
  free(handle->frame);
  free(handle->spr_pixel_buf);
  free(handle->spr_buffer);
  ppu_shift_storage_free(handle->fetch_storage);
  free(handle);
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

int ppu_evaluate_sprites(struct ppu_state* ppu, struct ppu_sprite* output, int outlen, int pV) {
  // counters
  int ramc = 0, outc = 0;
  struct ppu_sprite sprite;
  // go through whole sprite ram or outlen and 1 more for sprite overflow detection
  while (ramc < PPU_SPRRAM_SIZE && outc <= outlen) {
    sprite = ppu->memory->sprite_ram[ramc];
    if (pV >= sprite.y_coord && pV < (sprite.y_coord + TILE_SIZE)) {
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
 * Pixel layout for sprite:
 * Each pixel is represented by a byte with bits arrangement:
 * Low Priority: X X X X X X X X :High priority
 * Where each bit represents sprite based on its index.
 */
void ppu_sprite_pixel_layout(struct ppu_sprite* sprites, int sprlen, uint8_t* buffer, int bufsize) {
  struct ppu_sprite spr;
  for (int i = 0; i < bufsize; i++) {
    buffer[i] = 0;
    for (int j = sprlen - 1; j >= 0; j--) {
      spr = sprites[j];
      if (i >= spr.x_coord && i < (spr.x_coord + TILE_SIZE)) {
        buffer[i] |= (0x01 << j);
      }
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
  struct ppu_shift_storage* storage = handle->render_storage;
  uint8_t fine_x = ppu->fine_x & 0x07;
  uint16_t bg_color_address = 0x3f00 | (uint16_t) ((storage->bg_tiles >> ((TILE_SIZE - fine_x - 1) << 2)) & 0x0F); // (7 - x_fine) * 4
  storage->bg_tiles <<= 4;
  struct ppu_color bg_color = NES_COLOR(ppu_memory_load(ppu->memory, bg_color_address));
  handle->frame_buf_pos += ppu_update_frame(handle->frame, bg_color, handle->frame_buf_pos);
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
  handle->fetch_storage = handle->fetch_storage->next;
  handle->render_storage = handle->render_storage->next;
}

void ppu_storage_compile(struct ppu_shift_storage* storage) {
  uint32_t value;
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
  storage->bg_tiles = value;  
}

void ppu_execute_cycle(struct ppu_state* ppu, struct ppu_render_handle* handle) {
  handle->cycle += 1;
  if (handle->cycle > 340) {
    handle->cycle = 0;
    handle->line += 1;
    if (handle->line > 261) {
      handle->line = 0;
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
  int bg_ptable = ppu->control.bg_pttrntable;
  if (ppu->mask.show_bg || ppu->mask.show_spr) {
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
          storage->bg_tile_lower0 = ppu_memory_fetch_pt(ppu->memory, storage->bg_pt_index * 16 + ppu->fine_y, bg_ptable);
          break;
        case 7:
          storage->bg_tile_lower1 = ppu_memory_fetch_pt(ppu->memory, storage->bg_pt_index * 16 + ppu->fine_y + TILE_SIZE, bg_ptable);
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
