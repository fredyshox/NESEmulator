//
// 6502 asm arithmetic handlers
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/handlers/arithmetic.h"
#include "6502/addr.h"
#include <stdbool.h>

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

void bitwise_shift_l(state6502 *state, asm6502 cmd) {
  uint16_t value;
  uint16_t addr;
  if (cmd.maddr.type == ACC_ADDR) {
    value = state->reg_a;
  } else {
    addr = handle_addr(state, cmd.maddr);
    value = (uint16_t) memory6502_load(state->memory, addr);
  }

  value = value << 1;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);
  eval_carry_flag(state, value);

  if (cmd.maddr.type == ACC_ADDR) {
    state->reg_a = (uint8_t) value;
  } else {
    memory6502_store(state->memory, addr, (uint8_t) value);
  }
}

void bitwise_shift_r(state6502 *state, asm6502 cmd) {
  uint8_t value;
  uint16_t addr;
  if (cmd.maddr.type == ACC_ADDR) {
    value = state->reg_a;
  } else {
    addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  state->status.carry = value & 0x01;
  value = value >> 1;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  if (cmd.maddr.type == ACC_ADDR) {
    state->reg_a = value;
  } else {
    memory6502_store(state->memory, addr, value);
  }
}

void bitwise_and(state6502 *state, asm6502 cmd) {
  uint8_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.lval;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }
  value = value & state->reg_a;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  state->reg_a = value;
}

void bitwise_xor(state6502 *state, asm6502 cmd) {
  uint8_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.lval;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }
  value = value ^ state->reg_a;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  state->reg_a = value;
}

void bitwise_or(state6502 *state, asm6502 cmd) {
  uint8_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.lval;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }
  value = value | state->reg_a;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  state->reg_a = value;
}

void bitwise_rotate_l(state6502 *state, asm6502 cmd) {
  uint16_t value;
  uint16_t addr;
  if (cmd.maddr.type == ACC_ADDR) {
    value = state->reg_a;
  } else {
    addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  value = (value << 1) + state->status.carry;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);
  eval_carry_flag(state, value);

  if (cmd.maddr.type == ACC_ADDR) {
    state->reg_a = (uint8_t) value;
  } else {
    memory6502_store(state->memory, addr, (uint8_t) value);
  }
}

void bitwise_rotate_r(state6502 *state, asm6502 cmd) {
  uint16_t value;
  uint16_t addr;
  if (cmd.maddr.type == ACC_ADDR) {
    value = state->reg_a;
  } else {
    addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  value = value + (state->status.carry) ? 0x0100 : 0;
  state->status.carry = value & 0x0001;
  value = value >> 1;
  eval_zero_flag(state, value);
  eval_sign_flag(state, value);

  if (cmd.maddr.type == ACC_ADDR) {
    state->reg_a = (uint8_t) value;
  } else {
    memory6502_store(state->memory, addr, (uint8_t) value);
  }
}

void bitwise_bit_test(state6502 *state, asm6502 cmd) {
  uint8_t value;
  if (cmd.maddr.type == IMM_ADDR) {
    value = cmd.maddr.lval;
  } else {
    uint16_t addr = handle_addr(state, cmd.maddr);
    value = memory6502_load(state->memory, addr);
  }

  state->status.negative = ((value & 0x80) != 0);
  state->status.overflow = ((value & 0x40) != 0);
  state->status.zero = ((value & state->reg_a) == 0);
}
