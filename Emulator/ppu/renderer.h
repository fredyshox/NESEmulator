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
#include "ppu/ppu.h"
#include "ppu/color.h"

#define HORIZONTAL_RES 256
#define VERTICAL_RES 240
#define TILE_HMAX 32
#define TILE_VMAX 30
#define TILE_SIZE 8
#define MAX_SPRITES_PER_LINE 8

/**
 * Structure with pointers to memory for rendering.
 * Should be reused between rendering calls.
 */
struct ppu_render_handle {
  int frame_buf_pos;
  uint8_t* frame;
  uint8_t* spr_pixel_buf;
  int sprbuf_size;
  struct ppu_sprite* spr_buffer;
  // tables for scanline
  int spr_ptable;
  int bg_ptable;
  uint8_t* nametable;
  uint8_t* attrtable;
  // tile coordinates
  int h;
  int v;
  // coordinates within tile (0..8)
  int pH;
  int pV;
  // current tile buffer
  uint8_t bg_tile_upper;
  uint8_t bg_tile_lower0;
  uint8_t bg_tile_lower1;
};

typedef struct ppu_render_handle ppu_render_handle;

/**
 * Creates handle instance by allocated memory for rendering needs
 * @return pointer to handle
 */
struct ppu_render_handle* ppu_render_handle_create();

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
 * Gets nametable and attribute table addresses according to ppu control flag
 * @param ppu    ppu state struct
 * @param nt_ptr pointer to save nametable addr
 * @param at_ptr pointer to save attrtable addr
 */
void ntat_addr(struct ppu_state* ppu, uint8_t** nt_ptr, uint8_t** at_ptr);

/**
 * Gets sprite and background pattern tables according to ppu control flag
 * @param ppu    ppu state struct
 * @param bg_pt pointer to save background pattern table addr
 * @param spr_pt pointer to save sprite pattern table addr
 */
void pt_addr(struct ppu_state* ppu, uint8_t** bg_pt, uint8_t** spr_pt);

/**
 * Fetches byte from name table - which is index of 8x8 tile in pattern table
 * @param nt_ptr - pointer to name table
 * @param h - horizontal index
 * @param v - vertical index
 */
uint8_t fetch_nt_byte(uint8_t* nt_ptr, int h, int v);

/**
 * Fetches byte from attribute table  - which is upper bits of pallete index for given 4x4 tile group
 * @param at_ptr - pointer to attribute table
 * @param h - horizontal index
 * @param v - vertical index
 */
uint8_t fetch_at_byte(uint8_t* at_ptr, int h, int v);

/**
 * Evaluates which sprites are going to appear on current line
 * @param  ppu    ppu state
 * @param  output pointer to output buffer
 * @param  outlen max length of output buffer
 * @param  pV     current line index
 * @return        number of sprites found on this position
 */
int ppu_evaluate_sprites(struct ppu_state* ppu, struct ppu_sprite* output, int outlen, int pV);

/**
 * Layouts pixel values for sprites that are visible on current line
 * Each pixel value is bit set - nth bit determines visibility of nth sprite from sprite buffer.
 * @param sprites sprite buffer
 * @param sprlen  sprite buffer length
 * @param buffer  pixel buffer
 * @param bufsize pixel buffer length
 */
void ppu_sprite_pixel_layout(struct ppu_sprite* sprites, int sprlen, uint8_t* buffer, int bufsize);

/**
 * Updates frame on specified position with specified color
 * @param  frame pointer to frame buffer
 * @param  color nes color
 * @param  pos   position in frame buffer
 * @return       number of bytes written
 */
int ppu_update_frame(uint8_t* frame, struct ppu_color color, int pos);

/**
 * Evaluates palette index based on bit info from pttrntable and pixel location.
 * @param  tile_lower0 bit 0th's for tile
 * @param  tile_lower1 bit 1st's for tile
 * @param  tile_upper  upper bit value (2 bit)
 * @param  pV          pixel vertical coord
 * @param  pH          pixel horizontal coord
 * @return             index in palette
 */
uint8_t ppu_color_idx(uint8_t tile_lower0, uint8_t tile_lower1, uint8_t tile_upper, int pV, int pH);

/**
 * Renders single frame based on ppu state
 * @param ppu   ppu state
 * @param handle reusable memory for rendering work
 */
void ppu_render(struct ppu_state* ppu, struct ppu_render_handle* handle);

/**
 * Executes signle ppu cycle (currently renders single pixel)
 * @param ppu    ppu state
 * @param handle reusable memory for rendering work
 */
void ppu_execute_cycle(struct ppu_state* ppu, struct ppu_render_handle* handle);

#endif /* renderer_h */
