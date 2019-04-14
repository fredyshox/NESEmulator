//
// Struct definitions for representing ppu state
// Copyright (c) 2019 Kacper Rączy
//

#include "ppu.h"

void ppu_ctrl_write(struct ppu_state* state, uint8_t byte) {
  state->control.byte = byte;
}

void ppu_mask_write(struct ppu_state* state, uint8_t byte) {
  state->mask.byte = byte;
}

void ppu_status_read(struct ppu_state* state, uint8_t* ptr) {
  *ptr = state->status.byte;
}

void ppu_sr_addr_write(struct ppu_state* state, uint8_t addr) {
  state->reg_sr_addr = addr;
}

void ppu_sr_data_write(struct ppu_state* state, uint8_t byte) {
  uint8_t si = state->reg_sr_addr >> 4; // fast div
  uint8_t bi = state->reg_sr_addr & 0b11; // fast mod
  struct ppu_sprite* spr = &state->sprite_ram[si];
  switch (bi) {
    case 0:
      spr->y_coord = byte;
      break;
    case 1:
      spr->index = byte;
      break;
    case 2:
      spr->attrs = byte;
      break;
    case 3:
      spr->x_coord = byte;
      break;
    default: break;
  }
  state->reg_sr_addr += 1;
}

void ppu_sr_data_read(struct ppu_state* state, uint8_t* ptr) {
  uint8_t si = state->reg_sr_addr >> 2; // fast div
  uint8_t bi = state->reg_sr_addr & 0b11; // fast mod
  struct ppu_sprite* spr = &state->sprite_ram[si];
  switch (bi) {
    case 0:
      *ptr = spr->y_coord;
      break;
    case 1:
      *ptr = spr->index;
      break;
    case 2:
      *ptr = spr->attrs;
      break;
    case 3:
      *ptr = spr->x_coord;
      break;
    default: break;
  }
}

void ppu_scroll_write(struct ppu_state* state, uint8_t coord) {
  static bool y_mode = false;
  if (y_mode)
    state->scroll_y = coord;
  else
    state->scroll_x = coord;

  y_mode = !y_mode;
}

void ppu_addr_write(struct ppu_state* state, uint8_t byte) {
  // flag which determine wether msb or lsb
  // of ppuaddr will be written
  static bool lsb_mode = false;
  uint16_t addr = state->reg_addr;
  if (lsb_mode) {
    addr = addr & 0xff00;
    addr = addr | byte;
  } else {
    addr = addr & 0x00ff;
    addr = addr | (uint16_t) byte << 8;
  }

  state->reg_addr = addr;
  lsb_mode = !lsb_mode;
}

void ppu_data_write(struct ppu_state* state, uint8_t byte) {
  // evaluate state->addr
  //TODO write byte to appropriate location
  if (state->control.addr_inc32) {
    state->reg_addr += 32;
  } else {
    state->reg_addr += 1;
  }
}

void ppu_data_read(struct ppu_state* state, uint8_t* ptr) {
  // evaluate state->addr
  //TODO read byte
  if (state->control.addr_inc32) {
    state->reg_addr += 32;
  } else {
    state->reg_addr += 1;
  }
}

void ppu_sr_dma_write(struct ppu_state* state, uint8_t* data, int count) {
  assert(count > 0 && count <= PPU_SPRRAM_BYTE_SIZE);
  uint8_t rem;
  struct ppu_sprite* sprite;
  for (uint8_t i = 0; i < count; i++) {
    rem = i & 0b11;
    sprite = &state->sprite_ram[(i >> 2)];
    switch (rem) {
      case 1:
        sprite->y_coord = data[i];
        break;
      case 2:
        sprite->index = data[i];
        break;
      case 3:
        sprite->attrs = data[i];
        break;
      case 4:
        sprite->x_coord = data[i];
        break;
      default: break;
    }
  }
}
