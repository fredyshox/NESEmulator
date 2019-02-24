//
// 6502 asm instructions
// Copyright (c) 2019 Kacper Rączy
//

#include "instruction.h"
#include <stdbool.h>
#include <assert.h>

struct asm6502 asm6502_create(int type, struct mem_addr addr, void (*handler)(state6502*, asm6502)) {
  struct asm6502 instruction;
  instruction.type = type;
  instruction.maddr = addr;
  instruction.handler = handler;

  return instruction;
}
 
void asm6502_execute(struct asm6502 cmd, struct state6502* state) {
  cmd.handler(state, cmd);
}

static inline void eval_zero_flag(state6502 *state, uint16_t value) {
  if ((value & 0xff) == 0) {
    state->status.zero = 1;
  } else {
    state->status.zero = 0;
  }
}

static inline void eval_sign_flag(state6502 *state, uint16_t value) {
  if (value & 0x80) {
    state->status.negative = 1;
  } else {
    state->status.negative = 0;
  }
}

static inline void eval_carry_flag(state6502 *state, uint16_t value) {
  if (value > 0xff) {
    state->status.carry = 1;
  } else {
    state->status.carry = 0;
  }
}

// Add value to accumulator with carry
// Affected flags: Z, N, C, V
void add_with_carry(state6502 *state, asm6502 cmd) {
  assert(cmd.type == ADC_ASM);
  uint16_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.value;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  uint16_t result = (uint16_t) state->reg_a + value + state->status.carry;

  //flags
  eval_zero_flag(state, result);
  eval_sign_flag(state, result);
  eval_carry_flag(state, result);
  // evaluate overflow flag
  // set if both operands are positive and result negative or
  // both operands are negative and result positive
  bool neg = state->status.negative;
  bool a_neg = (state->reg_a > 0x7f);
  bool op_neg = (value > 0x7f);
  if ((!a_neg && !op_neg && neg) || (a_neg && op_neg && !neg)) {
    state->status.overflow = 1;
  } else {
    state->status.overflow = 0;
  }

  state->reg_a = (uint8_t) result;
}

void increment_memory(state6502 *state, asm6502 cmd) {
  assert(cmd.type == INC_ASM);
  
  uint16_t addr = handle_addr(state, cmd.maddr);
  uint16_t value = memory6502_load(state->memory, addr);
  value += 1;
  
  // flags
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  memory6502_store(state->memory, addr, (uint8_t) value);
}


void increment_x(state6502 *state, asm6502 cmd) {
  assert(cmd.type == INX_ASM);
  uint16_t value = state->reg_x + 1;
  
  // flags 
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);
  
  state->reg_x = (uint8_t) value;
}

void increment_y(state6502 *state, asm6502 cmd) {
  assert(cmd.type == INY_ASM);
  uint16_t value = state->reg_y + 1;
  
  // flags
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  state->reg_y = (uint8_t) value;
}

void subtract_with_carry(state6502 *state, asm6502 cmd) {
  assert(cmd.type == SBC_ASM);
  uint16_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.value;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  uint16_t result = (uint16_t) state->reg_a - value - (!state->status.carry);

  // flags 
  eval_zero_flag(state, result);
  eval_sign_flag(state, result);
  eval_carry_flag(state, result);
  // evaluate overflow flag
  // set if operand1 is negative, operand2 is positive and result positive or
  // operand1 is positive, operand2 is negative and result is negative
  bool neg = state->status.negative;
  bool a_neg = (state->reg_a > 0x7f);
  bool op_neg = (value > 0x7f);
  if ((a_neg && !op_neg && !neg) || (!a_neg && op_neg && neg)) {
    state->status.overflow = 1;
  } else {
    state->status.overflow = 0;
  }

  state->reg_a = (uint8_t) result;
}

void decrement_memory(state6502 *state, asm6502 cmd) {
  assert(cmd.type == DEC_ASM);
  
  uint16_t addr = handle_addr(state, cmd.maddr);
  uint16_t value = memory6502_load(state->memory, addr);
  value -= 1;
  
  // flags
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  memory6502_store(state->memory, addr, (uint8_t) value);
}

void decrement_x(state6502 *state, asm6502 cmd) {
  assert(cmd.type == DEX_ASM);
  uint16_t value = state->reg_x - 1;
  
  // flags 
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);
  
  state->reg_x = (uint8_t) value;
}

void decrement_y(state6502 *state, asm6502 cmd) {
  assert(cmd.type == DEY_ASM);
  uint16_t value = state->reg_y - 1;
  
  // flags 
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);
  
  state->reg_y = (uint8_t) value;
}

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
  assert(cmd.type == LDA_ASM);
  state->reg_a = load_value(state, cmd);
}

// Load value into X register
// Affected flags: Z, N
void load_xreg(state6502 *state, asm6502 cmd) {
  assert(cmd.type == LDX_ASM);
  state->reg_x = load_value(state, cmd);
}

// Load value into Y register
// Affected flags: Z, N
void load_yreg(state6502 *state, asm6502 cmd) {
  assert(cmd.type == LDY_ASM);
  state->reg_y = load_value(state, cmd);
}

// Store acc value
// Affected flags: none
void store_accumulator(state6502 *state, asm6502 cmd) {
  assert(cmd.type == STA_ASM);
  uint16_t addr = handle_addr(state, cmd.maddr);
  memory6502_store(state->memory, addr, state->reg_a);
}

// Store X register value
// Affected flags: none
void store_xreg(state6502 *state, asm6502 cmd) { 
  assert(cmd.type == STX_ASM);
  uint16_t addr = handle_addr(state, cmd.maddr);
  memory6502_store(state->memory, addr, state->reg_x);
}

// Store Y register value
// Affected flags: none
void store_yreg(state6502 *state, asm6502 cmd) {
  assert(cmd.type == STY_ASM);
  uint16_t addr = handle_addr(state, cmd.maddr);
  memory6502_store(state->memory, addr, state->reg_y);
}

static void transfer(state6502 *state, uint8_t *src, uint8_t *dest) {
  uint8_t value = *src;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);
  *dest = value;
}

// Copy value from a to x
// Affected flags: Z, N
void transfer_a2x(state6502 *state, asm6502 cmd) {
  assert(cmd.type == TAX_ASM);
  transfer(state, &state->reg_a, &state->reg_x);
}

// Copy value from x to a
// Affected flags: Z, N
void transfer_x2a(state6502 *state, asm6502 cmd) {
  assert(cmd.type == TXA_ASM);
  transfer(state, &state->reg_x, &state->reg_a);
}

// Copy value from a to y
// Affected flags: Z, N
void transfer_a2y(state6502 *state, asm6502 cmd) {
  assert(cmd.type == TAY_ASM);
  transfer(state, &state->reg_a, &state->reg_y);
}

// Copy value from y to a
// Affected flags: Z, N
void transfer_y2a(state6502 *state, asm6502 cmd) {
  assert(cmd.type == TYA_ASM);
  transfer(state, &state->reg_y, &state->reg_a);
}

// Clear carry flag
// Affected flags: C
void clear_carry(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CLC_ASM);
  state->status.carry = 0;
}

// Set carry flag
// Affected flags: C
void set_carry(state6502 *state, asm6502 cmd) {
  assert(cmd.type == SEC_ASM);
  state->status.carry = 1;
}

// Clear interrupt flag
// Affected flags: I
void clear_interrupt(state6502 *state, asm6502 cmd) {
  assert(cmd.type == SEI_ASM);
  state->status.interrupt = 0;
}

// Set interrupt flag
// Affected flags: I
void set_interrupt(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CLI_ASM);
  state->status.interrupt = 1;
}

// Clear overflow flag
// Affected flags: V
void clear_overflow(state6502 *state, asm6502 cmd) {
  assert(cmd.type == CLV_ASM);
  state->status.overflow = 0;
}

