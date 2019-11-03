//
// Struct definitions for representing ppu state
// Copyright (c) 2019 Kacper Rączy
//

#include "ppu/ppu.h"

// PPU memory

void ppu_memory_create(struct ppu_memory* mem) {
  mem->image_palette = malloc(PPU_PALETTE_SIZE * sizeof(uint8_t));
  mem->sprite_palette = malloc(PPU_PALETTE_SIZE * sizeof(uint8_t));
  mem->sprite_ram = malloc(PPU_SPRRAM_SIZE * sizeof(struct ppu_sprite));
  // alloc memory for 4 nametables (normally ppu has vram for only 2)
  mem->nt_buf = malloc(4 * (PPU_NTAT_SIZE) * sizeof(uint8_t));
  mem->nt_buf_size = 4 * (PPU_NTAT_SIZE);
  ppu_memory_set_mirroring(mem, SINGLE_SCREEN);
  // memory allocated by user
  mem->io = NULL;
  mem->load_handler = NULL;
  mem->store_handler = NULL;
}

void ppu_memory_set_mirroring(struct ppu_memory* mem, enum ppu_mirroring mirroring_type) {
  switch (mirroring_type) {
    case HORIZONTAL:
      mem->ntat_block0 = mem->nt_buf;
      mem->ntat_block1 = mem->ntat_block0;
      mem->ntat_block2 = (mem->nt_buf + PPU_NTAT_SIZE);
      mem->ntat_block3 = mem->ntat_block2;
      break;
    case VERTICAL:
      mem->ntat_block0 = mem->nt_buf;
      mem->ntat_block1 = (mem->nt_buf + PPU_NTAT_SIZE);
      mem->ntat_block2 = mem->ntat_block0;
      mem->ntat_block3 = mem->ntat_block1;
      break;
    case FOUR_SCREEN:
      mem->ntat_block0 = mem->nt_buf;
      mem->ntat_block1 = (mem->ntat_block0 + PPU_NTAT_SIZE);
      mem->ntat_block2 = (mem->ntat_block1 + PPU_NTAT_SIZE);
      mem->ntat_block3 = (mem->ntat_block2 + PPU_NTAT_SIZE);
      break;
    case SINGLE_SCREEN:
      mem->ntat_block0 = mem->nt_buf;
      mem->ntat_block1 = mem->ntat_block0;
      mem->ntat_block2 = mem->ntat_block0;
      mem->ntat_block3 = mem->ntat_block0;
      break;
  }
}

uint8_t ppu_memory_load(struct ppu_memory* mem, uint16_t idx) {
  idx = idx & 0x3fff; // idx mod 0x4000
  
  if (idx >= 0x2000 && idx < 0x3f00) {
    uint16_t address = idx & 0x03ff;
    int block_index = (idx >> 10) & 0x0003; 
    switch (block_index) {
      case 0: 
        return mem->ntat_block0[address];
      case 1:
        return mem->ntat_block1[address];
      case 2:
        return mem->ntat_block2[address];
      case 3:
        return mem->ntat_block3[address];
      default: break; /* never gonna happen */
    }
  } else if (idx >= 0x3f00 && idx < 0x4000) {
    int type = (idx >> 4) & 1;
    uint8_t c = idx & 0x000f;
    if ((c & 0x03) == 0) {
      return mem->image_palette[0];
    }

    if (!type) { // even type
      return mem->image_palette[c];
    } else { // odd type
      return mem->sprite_palette[c];
    }
  }

  return mem->load_handler(mem, idx);
}

void ppu_memory_store(struct ppu_memory* mem, uint16_t idx, uint8_t value) {
  idx = idx & 0x3fff; // idx mod 0x4000

  if (idx >= 0x2000 && idx < 0x3f00) {
    int address = idx & 0x03ff;
    int block_index = (idx >> 10) & 0x0003;
    switch (block_index) {
      case 0:
        mem->ntat_block0[address] = value;
        break;
      case 1:
        mem->ntat_block1[address] = value;
        break;
      case 2:
        mem->ntat_block2[address] = value;
        break;
      case 3:
        mem->ntat_block3[address] = value;
        break;
      default: break; /* never gonna happen */
    }
  } else if (idx >= 0x3f00 && idx < 0x4000) {
    int type = (idx >> 4) & 1;
    uint8_t c = idx & 0x000f;

    if (!type || (type && (c & 0x03) == 0)) {
      mem->image_palette[c] = value;
    } else {
      mem->sprite_palette[c] = value;
    }
  } else {
    mem->store_handler(mem, idx, value);
  }
}

// PPU state

void ppu_state_create(struct ppu_state* ppu, struct ppu_memory* mem) {
  assert(ppu != NULL && mem != NULL);
  ppu->memory = mem;
  ppu->status.byte = 0x00;
  ppu->mask.byte = 0x00;
  ppu->control.byte = 0x00;
  ppu->v.address = 0x0000;
  ppu->t.address = 0x0000;
  ppu->w = false;
  ppu->fine_x = 0;
  ppu->fine_y = 0;
  ppu->temp_fine_y = 0;
  // setup
}

// PPU registers

void ppu_ctrl_write(struct ppu_state* state, uint8_t byte) {
  debug_print("PPUCTRL W: ");
  debug_print_ppu(state);
  state->control.byte = byte;
  state->t.nametable_addr = (byte & 0x03);
  byte >>= 2;
  state->control.addr_inc32 = (byte & 0x01);
  byte >>= 1;
  state->control.spr_pttrntable = (byte & 0x01);
  byte >>= 1;
  state->control.bg_pttrntable = (byte & 0x01);
  byte >>= 1;
  state->control.spr_size_16x8 = (byte & 0x01);
  byte >>= 1;
  state->control.ms_select = (byte & 0x01);
  byte >>= 1;
  state->control.gen_nmi = (byte & 0x01);
}

void ppu_mask_write(struct ppu_state* state, uint8_t byte) {
  debug_print("PPUMASK W: ");
  debug_print_ppu(state);
  state->mask.byte = byte;
}

void ppu_status_read(struct ppu_state* state, uint8_t* ptr) {
  debug_print("PPUSTATUS R: ");
  debug_print_ppu(state);
  state->w = false;
  *ptr = state->status.byte;
  state->status.vblank = 0;
}

void ppu_sr_addr_write(struct ppu_state* state, uint8_t addr) {
  debug_print("OAMADDR W: ");
  debug_print_ppu(state);
  state->reg_sr_addr = addr;
}

void ppu_sr_data_write(struct ppu_state* state, uint8_t byte) {
  debug_print("OAMDATA W: ");
  debug_print_ppu(state);
  uint8_t si = state->reg_sr_addr >> 2; // fast div
  uint8_t bi = state->reg_sr_addr & 0b11; // fast mod
  struct ppu_sprite* spr = &state->memory->sprite_ram[si];
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
  debug_print("OAMDATA R: ");
  debug_print_ppu(state);
  uint8_t si = state->reg_sr_addr >> 2; // fast div
  uint8_t bi = state->reg_sr_addr & 0b11; // fast mod
  struct ppu_sprite* spr = &state->memory->sprite_ram[si];
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
  debug_print("PPUSCROLL W: ");
  debug_print_ppu(state);

  if (!state->w) {
    state->fine_x = (coord & 0x07);
    state->t.x_scroll = (coord >> 3);
    state->w = true;
  } else {
    state->temp_fine_y = (coord & 0x07);
    state->t.y_scroll = (coord >> 3);
    state->w = false;
  }
}

void ppu_addr_write(struct ppu_state* state, uint8_t byte) {
  debug_print("PPUADDR W: ");
  debug_print_ppu(state);
  uint16_t value = state->t.address;
  if (!state->w) {
    value &= 0xc0ff;
    value |= ((uint16_t) (byte & 0x3f)) << 8;
    state->t.address = value;
    state->temp_fine_y = (byte >> 4) & 0x03;
    state->w = true;
  } else {
    value &= 0xff00;
    value |= (uint16_t) byte;
    state->t.address = value;
    state->v.address = value;
    state->fine_y = state->temp_fine_y;
    state->w = false;
  }
}

void ppu_data_write(struct ppu_state* state, uint8_t byte) {
  debug_print("PPUDATA W: ");
  debug_print_ppu(state);

  struct ppu_memory* mem = state->memory;
  ppu_memory_store(mem, state->v.address, byte);
  //TODO v address during rendering
  if (state->control.addr_inc32) {
    state->v.address += 32;
  } else {
    state->v.address += 1;
  }
}

void ppu_data_read(struct ppu_state* state, uint8_t* ptr) {
  debug_print("PPUDATA R: ");
  debug_print_ppu(state);

  struct ppu_memory* mem = state->memory;
  *ptr = ppu_memory_load(mem, state->v.address);
  //TODO v address during rendering
  if (state->control.addr_inc32) {
    state->v.address += 32;
  } else {
    state->v.address += 1;
  }
}

void ppu_sr_dma_write(struct ppu_state* state, uint8_t* data, int count) {
  debug_print("OAMDMA W: ");
  debug_print_ppu(state);
  assert(count > 0 && count <= PPU_SPRRAM_BYTE_SIZE);
  uint8_t rem;
  struct ppu_sprite* sprite;
  for (uint8_t i = 0; i < count; i++) {
    rem = i & 0b11;
    sprite = &state->memory->sprite_ram[(i >> 2)];
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
