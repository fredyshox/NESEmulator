//
// 6502 asm memory handlers
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/handlers/memory.h"
#include "6502/addr.h"

// Affected flags: Z, N
static uint8_t load_value(state6502 *state, asm6502 cmd) {
  uint8_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.lval;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  // flags
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  return value;
}

// Load value into accumulator register
// Affected flags: Z, N
void load_accumulator(state6502 *state, asm6502 cmd) {
  state->reg_a = load_value(state, cmd);
}

// Load value into X register
// Affected flags: Z, N
void load_xreg(state6502 *state, asm6502 cmd) {
  state->reg_x = load_value(state, cmd);
}

// Load value into Y register
// Affected flags: Z, N
void load_yreg(state6502 *state, asm6502 cmd) {
  state->reg_y = load_value(state, cmd);
}

// Store acc value
// Affected flags: none
void store_accumulator(state6502 *state, asm6502 cmd) {
  uint16_t addr = handle_addr(state, cmd.maddr);
  memory6502_store(state->memory, addr, state->reg_a);
}

// Store X register value
// Affected flags: none
void store_xreg(state6502 *state, asm6502 cmd) {
  uint16_t addr = handle_addr(state, cmd.maddr);
  memory6502_store(state->memory, addr, state->reg_x);
}

// Store Y register value
// Affected flags: none
void store_yreg(state6502 *state, asm6502 cmd) {
  uint16_t addr = handle_addr(state, cmd.maddr);
  memory6502_store(state->memory, addr, state->reg_y);
}

// Copy value from src to dest
// Affected flags: Z, N
static void transfer(state6502 *state, uint8_t *src, uint8_t *dest) {
  uint8_t value = *src;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);
  *dest = value;
}

void transfer_a2x(state6502 *state, asm6502 cmd) {
  transfer(state, &state->reg_a, &state->reg_x);
}

void transfer_x2a(state6502 *state, asm6502 cmd) {
  transfer(state, &state->reg_x, &state->reg_a);
}

void transfer_a2y(state6502 *state, asm6502 cmd) {
  transfer(state, &state->reg_a, &state->reg_y);
}

void transfer_y2a(state6502 *state, asm6502 cmd) {
  transfer(state, &state->reg_y, &state->reg_a);
}
