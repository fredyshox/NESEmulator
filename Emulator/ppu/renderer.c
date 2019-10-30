//
// PPU rendering engine
// Copyright (c) 2019 Kacper Rączy
//

#include "ppu/renderer.h"

// ppu_render_handle funcitons

struct ppu_render_handle* ppu_render_handle_create() {
  struct ppu_render_handle* handle = malloc(sizeof(struct ppu_render_handle));
  handle->frame = malloc(sizeof(uint8_t) * HORIZONTAL_RES * VERTICAL_RES * 3);
  handle->spr_pixel_buf = malloc(sizeof(uint8_t) * HORIZONTAL_RES);
  handle->spr_buffer = malloc(sizeof(struct ppu_sprite) * MAX_SPRITES_PER_LINE);
  handle->frame_buf_pos = 0;
  handle->cycle = 340;
  handle->line = 240;
  handle->x_fine = 0;
  handle->y_fine = 0;
  handle->v.address = 0x0000;
  handle->t.address = 0x0000;

  return handle;
}

void ppu_render_handle_free(struct ppu_render_handle* handle) {
  free(handle->frame);
  free(handle->spr_pixel_buf);
  free(handle->spr_buffer);
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
  uint16_t attr_address = 0x23C0 | (v.address & 0x0c00) | ((v.address >> 4) & 0x38) | ((v.address >> 2) & 0x07);
  return ppu_memory_load(mem, attr_address);
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

uint8_t ppu_color_idx(uint8_t tile_lower0, uint8_t tile_lower1, uint8_t tile_upper, int pV, int pH) {
  uint8_t index = (tile_upper << 2) & 0x0c;
  index |= (((tile_lower1 >> (TILE_SIZE - pH - 1)) << 1) & 0x02);
  index |= ((tile_lower0 >> (TILE_SIZE - pH - 1)) & 0x01);

  return index;
}

void ppu_execute_cycle(struct ppu_state* ppu, struct ppu_render_handle* handle) {
  int h = handle->h, v = handle->v, pH = handle->pH, pV = handle->pV;
  uint8_t bg_color_idx, spr_color_idx;
  uint8_t *nametable, *attrtable;
  uint8_t *frame = handle->frame;
  int frame_buf_pos;
  uint8_t spx;
  struct ppu_color color, spr_color;
  struct ppu_sprite spr;

  // debugging
  // debug_print_ppu(ppu);
  debug_print_handle(handle);

  if (NEW_FRAME(handle)) {
    handle->spr_ptable = ppu->control.spr_pttrntable;
    handle->bg_ptable = ppu->control.bg_pttrntable;
    handle->frame_buf_pos = 0;
    ppu->status.vblank = 0;
    ppu->status.sprite_ovf = 0;
    ppu->status.sprite_zhit = 0;
    ntat_addr(ppu, &handle->nametable, &handle->attrtable);
  }

  nametable = handle->nametable;
  attrtable = handle->attrtable;
  frame_buf_pos = handle->frame_buf_pos;

  if (NEW_SCANLINE(handle)) {
    handle->sprbuf_size = ppu_evaluate_sprites(ppu, handle->spr_buffer, MAX_SPRITES_PER_LINE, v * TILE_SIZE + pV);
    ppu_sprite_pixel_layout(handle->spr_buffer, handle->sprbuf_size, handle->spr_pixel_buf, HORIZONTAL_RES);
  }

  if (NEW_TILE(handle)) {
    // background
    // index in pattern table for bg tile
    uint8_t bg_pt_index = fetch_nt_byte(nametable, h, v);
    // tile upper bits from attr table
    handle->bg_tile_upper = fetch_at_byte(attrtable, h, v);
    // lower bits for each pixel in tile
    handle->bg_tile_lower0 = FETCH_PT_BYTE(ppu, handle->bg_ptable, (bg_pt_index * 16) + pV);
    handle->bg_tile_lower1 = FETCH_PT_BYTE(ppu, handle->bg_ptable, (bg_pt_index * 16) + pV + TILE_SIZE);
  }
  // background color
  bg_color_idx = ppu_color_idx(handle->bg_tile_lower0, handle->bg_tile_lower1, handle->bg_tile_upper, pV, pH);
  color = NES_COLOR(ppu->memory->image_palette[bg_color_idx]);
  // sprites
  spx = handle->spr_pixel_buf[h * TILE_SIZE + pH];
  if (spx != 0) {
    // check sprite pixel color
    // from high priority to low priority, until the opaque color is found
    for (int s = 0; s < handle->sprbuf_size; s++) {
      if ((spx & (0x01 << s)) != 0) {
        spr = handle->spr_buffer[s];
        // vertical flip
        uint16_t spr_pt_addr = (spr.vflip) ? (spr.index * 16) + (TILE_SIZE - 1 - pV) : (spr.index * 16) + pV;
        // fetch bytes from pt
        uint8_t sp_tile_lower0 = FETCH_PT_BYTE(ppu, handle->spr_ptable, spr_pt_addr);
        uint8_t sp_tile_lower1 = FETCH_PT_BYTE(ppu, handle->spr_ptable, spr_pt_addr + TILE_SIZE);
        // horizontal flip
        if (spr.hflip) {
          sp_tile_lower0 = utility_bit_reverse(sp_tile_lower0);
          sp_tile_lower1 = utility_bit_reverse(sp_tile_lower1);
        }

        // get color
        spr_color_idx = ppu_color_idx(sp_tile_lower0, sp_tile_lower1, spr.palette_msb, pV, pH);
        spr_color = NES_COLOR(ppu->memory->sprite_palette[spr_color_idx]);
        if (!NES_COLOR_TRANSPARENT(spr_color)) break;
      }
    }
    // bg non trans and spr back -> bg color
    // bg trans and spr back -> spr
    // bg non trans and spr front -> spr
    // bg trans and spr front -> spr
    if (!NES_COLOR_TRANSPARENT(spr_color) && (NES_COLOR_TRANSPARENT(color) || spr.priority == 0)) {
      color = spr_color;
    }
  }

  handle->frame_buf_pos += ppu_update_frame(frame, color, frame_buf_pos);

  if (VBLANK(handle)) {
    ppu->status.vblank = 1;
  }

  handle->pH = (pH + 1) % TILE_SIZE;
  if (handle->pH == 0) {
    // next h tile
    handle->h = (h + 1) % TILE_HMAX;
    if (handle->h == 0) {
      // next scanline
      handle->pV = (pV + 1) % TILE_SIZE;
      if (handle->pV == 0) {
        handle->v = (v + 1) % TILE_VMAX;
      }
    }
  }
}

void ppu_render_pixel(struct ppu_state* ppu, struct ppu_render_handle* handle) {
  
}

void ppu_increment_x(struct ppu_state* ppu) {
  ppu->v.x_scroll += 1;
  if (!ppu->v.x_scroll) {
    ppu->v.nametable_addr ^= 1;
  }
}

void ppu_increment_y(struct ppu_state* ppu) {
  if (ppu->fine_y < 7) {
    ppu->fine_x += 1;
  } else {
    ppu->fine_y = 0;
    uint8_t y = ppu->v.y_scroll;
    if (y == 29) {
      ppu->v.y_scroll = 0;
      ppu->v.nametable_addr ^= 2;
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
  //TODO y fine scroll copy
}

void ppu_execute_cycle2(struct ppu_state* ppu, struct ppu_render_handle* handle) {
  handle->cycle += 1;
  if (handle->cycle > 340) {
    handle->cycle = 0;
    handle->line += 1;
    if (handle->line > 261) {
      handle->line = 0;
    }
  }

  bool visibleFrame  = handle->line >= 0 && handle->line <= 239;
  bool visibleCycle  = handle->cycle >= 1 && handle->cycle <= 256;
  bool preFetchCycle = handle->cycle >= 321 && handle->cycle <= 336;
  bool preRenderLine = handle->line == 261;
  bool fetchCycle = visibleCycle || preFetchCycle;
  
  // check if rendering is enabled 
  int bg_ptable = ppu->control.bg_pttrntable;
  if (ppu->mask.show_bg || ppu->mask.show_spr) {
    if (visibleFrame && visibleCycle) {
      // just draw stuff
      ppu_render_pixel(ppu, handle);
    }
    if ((visibleFrame || preRenderLine) && fetchCycle) {
      switch (handle->cycle & 0b111) {
        case 1:
          handle->bg_pt_index = ppu_memory_fetch_nt(ppu->memory, ppu->v);
          break;
        case 3:
          handle->bg_tile_upper = ppu_memory_fetch_at(ppu->memory, ppu->v);
          break;
        // lower bits for each pixel in tile
        case 5:
          handle->bg_tile_lower0 = ppu_memory_fetch_pt(ppu->memory, handle->bg_pt_index * 16 + ppu->fine_y, bg_ptable);
          break;
        case 7:
          handle->bg_tile_lower1 = ppu_memory_fetch_pt(ppu->memory, handle->bg_pt_index * 16 + ppu->fine_y + TILE_SIZE, bg_ptable);
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

  // vblanks
}