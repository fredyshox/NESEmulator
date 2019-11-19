//
// 6502 memory addressing
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/addr.h"

static int full_value(struct mem_addr *maddr, struct memory6502 *memory, uint16_t pos) {
  // little endian
  uint8_t lsb = memory6502_load(memory, pos);
  uint8_t msb = memory6502_load(memory, pos + 1);
  uint16_t value = (uint16_t) lsb | ((uint16_t) msb << 8);
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

uint16_t handle_addr(struct state6502 *state, struct mem_addr maddr, bool* page_crossed) {
  uint16_t value;
  bool pbc;
  switch (maddr.type) {
    case ZPX_ADDR:
      value = (maddr.lval + state->reg_x) & 0xff;
      break;
    case ZPY_ADDR:
      value = (maddr.lval + state->reg_y) & 0xff;
      break;
    case REL_ADDR:
      value = state->pc + (int8_t) maddr.lval;
      pbc = (value & 0x00ff) < (state->pc & 0x00ff);
      break;
    case ABX_ADDR:
      value = maddr.value + state->reg_x;
      pbc = (value & 0x00ff) < maddr.lval;
      break;
    case ABY_ADDR:
      value = maddr.value + state->reg_y;
      pbc = (value & 0x00ff) < maddr.lval;
      break;
    case IND_ADDR:
      value = 
        (uint16_t) memory6502_load(state->memory, maddr.value) | 
        ((uint16_t) memory6502_load(state->memory, (maddr.value & 0xff00) | (uint16_t) (uint8_t) (maddr.lval + 1)) << 8);
      break;
    // zero page wrapping below
    case IZX_ADDR:
      value = 
        (uint16_t) memory6502_load(state->memory, (maddr.lval + state->reg_x) & 0xff) |
        ((uint16_t) memory6502_load(state->memory, (maddr.lval + state->reg_x + 1) & 0xff) << 8);
      break;
    case IZY_ADDR:
      value = 
        (uint16_t) memory6502_load(state->memory, maddr.value) | 
        ((uint16_t) memory6502_load(state->memory, (maddr.value + 1) & 0xff) << 8);
      pbc = ((value + state->reg_y) & 0x00ff) < (value & 0x00ff);
      value += state->reg_y;
      break;
    default:
      value = maddr.value;
  }

  if (page_crossed != NULL) {
    *page_crossed = pbc;
  }

  return value;
}

char* addr_to_string(struct mem_addr maddr, char* buf, int bufsize)  {
  if (bufsize < 8)
    return NULL;

  switch (maddr.type) {
    case IMM_ADDR:
      sprintf(buf, "#%02x", maddr.lval);
      break;
    case ZP_ADDR:
      sprintf(buf, "$%02x", maddr.lval);
      break;
    case ZPX_ADDR:
      sprintf(buf, "$%02x,X", maddr.lval);
      break;
    case ZPY_ADDR:
      sprintf(buf, "$%02x,Y", maddr.lval);
      break;
    case REL_ADDR:
      sprintf(buf, "*%+d", (int8_t) maddr.lval);
      break;
    case ABS_ADDR:
      sprintf(buf, "$%04x", maddr.value);
      break;
    case ABX_ADDR:
      sprintf(buf, "$%04x,X", maddr.value);
      break;
    case ABY_ADDR:
      sprintf(buf, "$%04x,Y", maddr.value);
      break;
    case IND_ADDR:
      sprintf(buf, "($%04x)", maddr.value);
      break;
    case IZX_ADDR:
      sprintf(buf, "($%02x,X)", maddr.lval);
      break;
    case IZY_ADDR:
      sprintf(buf, "($%02x),Y", maddr.lval);
      break;
    case ACC_ADDR:
      buf[0] = 'A'; buf[1] = '\0';
      break;
    default:
      buf[0] = '\0';
      break;
  }

  return buf;
}
