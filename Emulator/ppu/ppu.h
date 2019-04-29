//
// Struct definitions for representing ppu state
// Copyright (c) 2019 Kacper Rączy
//

#ifndef ppu_h
#define ppu_h

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

struct ppu_sprite {
  uint8_t y_coord;
  uint8_t index;
  union {
    struct {
      uint8_t palette_msb : 2;
      uint8_t unimpl      : 3;
      uint8_t priority    : 1;
      uint8_t hflip       : 1;
      uint8_t vflip       : 1;
    };
    uint8_t attrs;
  };
  uint8_t x_coord;
};

typedef struct ppu_sprite ppu_sprite;

union ppu_status {
  uint8_t byte;
  struct {
    uint8_t last_lsb    : 5;
    uint8_t sprite_ovf  : 1;
    uint8_t sprite_zhit : 1;
    uint8_t vblank      : 1;
  };
};

union ppu_control {
  uint8_t byte;
  struct {
    uint8_t nametable_addr : 2;
    uint8_t addr_inc32     : 1;
    uint8_t spr_pttrntable : 1;
    uint8_t bg_pttrntable  : 1;
    uint8_t spr_size       : 1;
    uint8_t ms_select      : 1;
    uint8_t gen_nmi        : 1;
  };
};

union ppu_mask {
  uint8_t byte;
  struct {
    uint8_t grayscale : 1;
    uint8_t bg_l8     : 1;
    uint8_t spr_l8    : 1;
    uint8_t show_bg   : 1;
    uint8_t show_spr  : 1;
    uint8_t emp_red   : 1;
    uint8_t emp_green : 1;
    uint8_t emp_blue  : 1;
  };
};

#define PPU_SPRRAM_SIZE 64
#define PPU_SPRRAM_BYTE_SIZE 256
#define PPU_PALETTE_SIZE 16
#define PPU_NAMETABLE_SIZE 960
#define PPU_ATTRTABLE_SIZE 64
#define PPU_PTTRNTABLE_SIZE 4096

struct ppu_memory {
  // nametables - one buffer and pointers
  uint8_t* nt_buf;
  uint8_t* nametable0;
  uint8_t* attrtable0;
  uint8_t* nametable1;
  uint8_t* attrtable1;
  uint8_t* nametable2;
  uint8_t* attrtable2;
  uint8_t* nametable3;
  uint8_t* attrtable3;
  // palettes
  uint8_t* image_palette;
  uint8_t* sprite_palette;
  // oam
  struct ppu_sprite* sprite_ram;
  // rom stuff
  void* io;
  uint8_t (*load_handler)(struct ppu_memory*, uint16_t);
  void (*store_handler)(struct ppu_memory*, uint16_t, uint8_t);
};

typedef struct ppu_memory ppu_memory;

void ppu_memory_create(struct ppu_memory* mem);
uint8_t ppu_memory_fetch_pt(struct ppu_memory* mem, uint16_t address, int pt_index);

struct ppu_state {
  // vram
  struct ppu_memory* memory;
  // registers
  // ppustatus, ppumask, ppuctrl
  union ppu_status status;
  union ppu_mask mask;
  union ppu_control control;
  // ppu address register
  uint16_t reg_addr;
  uint8_t reg_sr_addr;
  // scrolling storage
  uint8_t scroll_x;
  uint8_t scroll_y;
};

typedef struct ppu_state ppu_state;

void ppu_state_create(struct ppu_state* ppu, struct ppu_memory* mem);

// PPU Registers
#define PPUCTRL   0x2000
#define PPUMASK   0x2001
#define PPUSTATUS 0x2002
#define OAMADDR   0x2003
#define OAMDATA   0x2004
#define PPUSCROLL 0x2005
#define PPUADDR   0x2006
#define PPUDATA   0x2007
#define OAMDMA    0x4014

// PPUCTRL register: $2000
void ppu_ctrl_write(struct ppu_state* state, uint8_t byte);
// PPUMASK register: $2001
void ppu_mask_write(struct ppu_state* state, uint8_t byte);
// PPUSTATUS register: $2002
void ppu_status_read(struct ppu_state* state, uint8_t* ptr);
// OAMADDR register: $2003
void ppu_sr_addr_write(struct ppu_state* state, uint8_t addr);
// OAMDATA register - write: $2004
void ppu_sr_data_write(struct ppu_state* state, uint8_t byte);
// OAMDATA register - read: $2004
void ppu_sr_data_read(struct ppu_state* state, uint8_t* ptr);
// PPUSCROLL register (x2): $2005
void ppu_scroll_write(struct ppu_state* state, uint8_t coord);
// PPUADDR register (x2): $2006
void ppu_addr_write(struct ppu_state* state, uint8_t byte);
// PPUDATA register - write: $2007
void ppu_data_write(struct ppu_state* state, uint8_t byte);
// PPUDATA register - read: $2007
void ppu_data_read(struct ppu_state* state, uint8_t* ptr);
// OAMDMA register: $4014
void ppu_sr_dma_write(struct ppu_state* state, uint8_t* data, int count);

#endif /*ppu_h*/
