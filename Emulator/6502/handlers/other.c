//
// 6502 asm other handlers
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/handlers/other.h"
#include "6502/addr.h"

// Flags

// Affected flags: C
void clear_carry(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CLC_ASM);
  state->status.carry = 0;
}

// Affected flags: C
void set_carry(state6502 *state, asm6502 cmd) {
  assert(cmd.type == SEC_ASM);
  state->status.carry = 1;
}

// Affected flags: I
void clear_interrupt(state6502 *state, asm6502 cmd) {
  assert(cmd.type == SEI_ASM);
  state->status.int_disable = 0;
}

// Affected flags: I
void set_interrupt(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CLI_ASM);
  state->status.int_disable = 1;
}

// Affected flags: V
void clear_overflow(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CLV_ASM);
  state->status.overflow = 0;
}

// Affected flags: D (Decimal mode not supported in other areas)
void clear_decimal(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CLD_ASM);
  state->status.decimal = 0;
}

// Affected flags: D
void set_decimal(state6502 *state, asm6502 cmd) {
  assert(cmd.type == SED_ASM);
  state->status.decimal = 1;
}

// Comparsion

static void compare_register(state6502 *state, asm6502 cmd, uint8_t reg) {
  uint8_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.value;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  int16_t result = (int16_t) reg - (int16_t) value;
  state->status.carry = (result >= 0);
  state->status.zero = (result == 0);
  state->status.negative = (result < 0);
}

void compare_accumulator(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CMP_ASM);
  compare_register(state, cmd, state->reg_a);
  if (state->reg_a & 0x80) {
    state->status.negative = 1;
  }
}

void compare_xreg(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CPX_ASM);
  compare_register(state, cmd, state->reg_x);
}

void compare_yreg(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CPY_ASM);
  compare_register(state, cmd, state->reg_y);
}

// Does nothing
void no_operation(state6502 *state, asm6502 cmd) { /* nothing */ }
