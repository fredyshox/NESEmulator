//
// 6502 memory addressing
// Copyright (c) 2019 Kacper Rączy
//

#include "addr.h"
uint8_t memory6502_load(struct memory6502 *memory, uint16_t idx);
static int full_value(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  // little endian
  uint8_t lsb = memory6502_load(memory, pos);
  uint8_t msb = memory6502_load(memory, pos + 1);
  uint16_t value = lsb | (uint16_t) msb << 8;
  maddr->value = value;
  return 2;
}

static int lsb_value(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  uint8_t value = memory6502_load(memory, pos);
  maddr->value = 0;
  maddr->lval = value;
  return 1;
}

int immediate_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = IMM_ADDR;
  return lsb_value(maddr, memory, pos);
}

int zeropage_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = ZP_ADDR;
  return lsb_value(maddr, memory, pos);
}

int zeropage_x_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = ZPX_ADDR;
  return lsb_value(maddr, memory, pos);
}

int zeropage_y_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = ZPY_ADDR;
  return lsb_value(maddr, memory, pos);
}

int relative_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = REL_ADDR;
  return lsb_value(maddr, memory, pos);
}

int absolute_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = ABS_ADDR;
  return full_value(maddr, memory, pos);
}

int absolute_x_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = ABX_ADDR;
  return full_value(maddr, memory, pos);
}

int absolute_y_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = ABY_ADDR;
  return full_value(maddr, memory, pos);
}

int indirect_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = IND_ADDR;
  return full_value(maddr, memory, pos);
}

int indexed_indirect_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = IZX_ADDR;
  return lsb_value(maddr, memory, pos);
}

int indirect_indexed_addr(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  maddr->type = IZY_ADDR;
  return lsb_value(maddr, memory, pos);
}

uint16_t handle_addr(struct state6502 *state, struct mem_addr maddr) {
  uint16_t value;
  switch (maddr.type) {
    case ZPX_ADDR:
      value = maddr.lval + state->reg_x;
      break;
    case ZPY_ADDR:
      value = maddr.lval + state->reg_y;
      break;
    case REL_ADDR:
      value = state->pc + (int8_t) maddr.lval;
      break;
    case ABX_ADDR:
      value = maddr.value + state->reg_x;
      break;
    case ABY_ADDR:
      value = maddr.value + state->reg_y;
      break;
    case IND_ADDR:
      value = memory6502_load(state->memory, maddr.value) | memory6502_load(state->memory, maddr.value + 1) << 8;
      break;
    case IZX_ADDR:
      value = memory6502_load(state->memory, maddr.value + state->reg_x) | memory6502_load(state->memory, maddr.value + state->reg_x + 1) << 8;
      break;
    case IZY_ADDR:
      value = memory6502_load(state->memory, maddr.value) | memory6502_load(state->memory, maddr.value + 1) << 8;
      value += state->reg_y;
      break;
    default:
      value = maddr.value;
  }

  return value;
}
