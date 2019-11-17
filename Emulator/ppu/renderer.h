//
// PPU rendering engine
// Copyright (c) 2019 Kacper Rączy
//

#ifndef renderer_h
#define renderer_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "ppu/ppu.h"
#include "ppu/color.h"
#include "common/utility.h"

#define HORIZONTAL_RES 256
#define VERTICAL_RES 240
#define TILE_HMAX 32
#define TILE_VMAX 30
#define TILE_SIZE 8
#define MAX_SPRITES_PER_LINE 8

/**
 * Cyclic container for attributes and patterns fetched during fetch cycles.
 * Used by render handle.
 */
struct ppu_shift_storage {
  uint8_t bg_pt_index;
  uint8_t bg_tile_upper;
  uint8_t bg_tile_lower0;
  uint8_t bg_tile_lower1;
  // compiled bg tiles (8 tiles, each 4 bits)
  uint64_t bg_tiles;
  struct ppu_shift_storage* next;
};

struct ppu_shift_storage* ppu_shift_storage_create(void);
void ppu_shift_storage_free(struct ppu_shift_storage* pss);

/**
 * Structure with pointers to memory for rendering.
 * Should be reused between rendering calls.
 */
struct ppu_render_handle {
  int frame_buf_pos;
  // frame buffer
  uint8_t* frame;
  // visible frame
  uint8_t* vframe;
  bool odd_frame;
  uint8_t* spr_pixel_buf;
  int sprbuf_size;
  struct ppu_sprite* spr_buffer;
    // current tile buffer
  struct ppu_shift_storage* fetch_storage;
  struct ppu_shift_storage* render_storage;
  // tables for scanline
  int spr_ptable;
  int bg_ptable;
  // tile coordinates
  int cycle;
  int line;
  // nmi util
  bool nmi_trigger;
};

typedef struct ppu_render_handle ppu_render_handle;

/**
 * Creates handle instance by allocated memory for rendering needs
 * @return pointer to handle
 */
struct ppu_render_handle* ppu_render_handle_create(void);

#define debug_print_handle(handle) \
  debug_print("V:%02d pV:%d H:%02d pH:%d FPOS:%04x SBUF:%d SPT:%d BPT:%d\n", \
              handle->v, handle->pV, handle->h, handle->pH, handle->frame_buf_pos, \
              handle->sprbuf_size, handle->spr_ptable, handle->bg_ptable);

/**
 * Frees handle
 */
void ppu_render_handle_free(struct ppu_render_handle* handle);

// Utility macros
#define NEW_FRAME(HANDLE) (HANDLE->h == 0 && HANDLE->v == 0 && HANDLE->pH == 0 && HANDLE->pV == 0)
#define VBLANK(HANDLE) (HANDLE->h == TILE_HMAX - 1 && \
                         HANDLE->v == TILE_VMAX - 1 && \
                         HANDLE->pH == TILE_SIZE - 1 && \
                         HANDLE->pV == TILE_SIZE - 1)
#define NEW_SCANLINE(HANDLE) (HANDLE->h == 0 && HANDLE->pH == 0)
#define NEW_TILE(HANDLE) (HANDLE->pH == 0)

/**
 * Evaluates which sprites are going to appear on current line
 * @param  ppu    ppu state
 * @param  output pointer to output buffer
 * @param  outlen max length of output buffer
 * @param  y_coord     current line index
 * @return        number of sprites found on this position
 */
int ppu_evaluate_sprites(struct ppu_state* ppu, struct ppu_sprite* output, int outlen, int y_coord);

/**
 * Layouts pixel values for sprites that are visible on current line
 * @param ppu     ppu state
 * @param sprites sprite buffer
 * @param sprlen  sprite buffer length
 * @param buffer  pixel buffer
 * @param bufsize pixel buffer length
 */
void ppu_sprite_pixel_layout(struct ppu_state* ppu, struct ppu_sprite* sprites, int sprlen, uint8_t* buffer, int bufsize, int y_coord);

/**
 * Updates frame on specified position with specified color
 * @param  frame pointer to frame buffer
 * @param  color nes color
 * @param  pos   position in frame buffer
 * @return       number of bytes written
 */
int ppu_update_frame(uint8_t* frame, struct ppu_color color, int pos);

/**
 * Executes signle ppu cycle (currently renders single pixel)
 * @param ppu    ppu state
 * @param handle reusable memory for rendering work
 */
void ppu_execute_cycle(struct ppu_state* ppu, struct ppu_render_handle* handle);

#endif /* renderer_h */
