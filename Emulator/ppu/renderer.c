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
  handle->h = 0;
  handle->v = 0;
  handle->pH = 0;
  handle->pV = 0;

  return handle;
}

void ppu_render_handle_free(struct ppu_render_handle* handle) {
  free(handle->frame);
  free(handle->spr_pixel_buf);
  free(handle->spr_buffer);
  free(handle);
}

// nametable/attrtable/pttrntable pickers

void ntat_addr(struct ppu_state* ppu, uint8_t** nt_ptr, uint8_t** at_ptr) {
  switch (ppu->control.nametable_addr) {
    case 0:
      *nt_ptr = ppu->memory->nametable0;
      *at_ptr = ppu->memory->attrtable0;
      break;
    case 1:
      *nt_ptr = ppu->memory->nametable1;
      *at_ptr = ppu->memory->attrtable1;
      break;
    case 2:
      *nt_ptr = ppu->memory->nametable2;
      *at_ptr = ppu->memory->attrtable2;
      break;
    case 3:
      *nt_ptr = ppu->memory->nametable3;
      *at_ptr = ppu->memory->attrtable3;
      break;
    default:
      fputs("WTF! Invalid nt addr!", stderr); /* Never should happen */
      break;
  }
}

// nametable/attrtable fetchers

uint8_t fetch_nt_byte(uint8_t* nt_ptr, int h, int v) {
  assert(h >= 0 && h < TILE_HMAX);
  assert(v >= 0 && v < TILE_VMAX);

  int index = (TILE_HMAX * v) + h;
  return nt_ptr[index];
}

uint8_t fetch_at_byte(uint8_t* at_ptr, int h, int v) {
  assert(h >= 0 && h < TILE_HMAX);
  assert(v >= 0 && v < TILE_VMAX);

  // 4x4 group of tiles, 8 each row
  int index = (v / 4) * 8 + (h / 4);
  uint8_t attr = at_ptr[index]; // 0x33221100
  if ((v & 0b11) < 2 && (h & 0b11) < 2) {
    // bits 0, 1
    return attr & 0x03;
  } else if ((v & 0b11) < 2) {
    // bits 2, 3
    return (attr >> 2) & 0x03;
  } else if ((v & 0b11) >= 2 && (h & 0b11) < 2) {
    // bits 4, 5
    return (attr >> 4) & 0x03;
  } else {
    // bits 6, 7
    return (attr >> 6);
  }
}

#define FETCH_PT_BYTE(PPU, PTIDX, ADDR) ppu_memory_fetch_pt(PPU->memory, ADDR, PTIDX)

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
    for (int s = 0; s < handle->sprbuf_size; s++) {
      if ((spx & (0x80 >> s)) != 0) {
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
    if (spr.priority == 0 && !NES_COLOR_TRANSPARENT(spr_color)) {
      color = spr_color;
    } else if (spr.priority != 0 && NES_COLOR_TRANSPARENT(color)) {
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
