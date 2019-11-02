//
// 6502 asm other handlers
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/handlers/other.h"
#include "6502/addr.h"

// Flags

// Affected flags: C
void clear_carry(state6502 *state, asm6502 cmd) {
  state->status.carry = 0;
}

// Affected flags: C
void set_carry(state6502 *state, asm6502 cmd) {
  state->status.carry = 1;
}

// Affected flags: I
void clear_interrupt(state6502 *state, asm6502 cmd) {
  state->status.int_disable = 0;
}

// Affected flags: I
void set_interrupt(state6502 *state, asm6502 cmd) {
  state->status.int_disable = 1;
}

// Affected flags: V
void clear_overflow(state6502 *state, asm6502 cmd) {
  state->status.overflow = 0;
}

// Affected flags: D (Decimal mode not supported in other areas)
void clear_decimal(state6502 *state, asm6502 cmd) {
  state->status.decimal = 0;
}

// Affected flags: D
void set_decimal(state6502 *state, asm6502 cmd) {
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

  int8_t result = (int8_t) reg - (int8_t) value;
  state->status.carry = (reg >= value);
  state->status.zero = (result == 0);
  state->status.negative = (result < 0);
}

void compare_accumulator(state6502 *state, asm6502 cmd) {
  compare_register(state, cmd, state->reg_a);
  // sign of acc?
  // if (state->reg_a & 0x80) {
  //   state->status.negative = 1;
  // }
}

void compare_xreg(state6502 *state, asm6502 cmd) {
  compare_register(state, cmd, state->reg_x);
}

void compare_yreg(state6502 *state, asm6502 cmd) {
  compare_register(state, cmd, state->reg_y);
}

// Does nothing
void no_operation(state6502 *state, asm6502 cmd) { /* nothing */ }
