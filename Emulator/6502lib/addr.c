//
// 6502 memory addressing
// Copyright (c) 2019 Kacper Rączy
//

#include "addr.h"

static int full_value(struct mem_addr *maddr, uint8_t *code) {
  // little endian
  uint16_t value = *code | (uint16_t) *(code + 1) << 8;
  maddr->value = value;
  return 2;
}

static int lsb_value(struct mem_addr *maddr, uint8_t *code) {
  uint8_t value = *code;
  maddr->value = 0;
  maddr->lval = value;
  return 1;
}

int immediate_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = IMM_ADDR;
  return lsb_value(maddr, code);
}

int zeropage_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = ZP_ADDR;
  return lsb_value(maddr, code);
}

int zeropage_x_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = ZPX_ADDR;
  return lsb_value(maddr, code);
}

int zeropage_y_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = ZPY_ADDR;
  return lsb_value(maddr, code);
}

int relative_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = REL_ADDR;
  return lsb_value(maddr, code);
}

int absolute_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = ABS_ADDR;
  return full_value(maddr, code);
}

int absolute_x_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = ABX_ADDR;
  return full_value(maddr, code);
}

int absolute_y_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = ABY_ADDR;
  return full_value(maddr, code);
}

int indirect_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = IND_ADDR;
  return full_value(maddr, code);
}

int indexed_indirect_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = IZX_ADDR;
  return lsb_value(maddr, code);
}

int indirect_indexed_addr(struct mem_addr *maddr, uint8_t *code) {
  maddr->type = IZY_ADDR;
  return lsb_value(maddr, code);
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
      value = state->memory[maddr.value] | state->memory[maddr.value + 1] << 8;
      break;
    case IZX_ADDR:
      value = state->memory[maddr.value + state->reg_x] | state->memory[maddr.value + state->reg_x + 1] << 8;
      break;
    case IZY_ADDR:
      value = state->memory[maddr.value] | state->memory[maddr.value + 1] << 8;
      value += state->reg_y;
      break;
    default:
      value = maddr.value;
  }

  return value;
}
