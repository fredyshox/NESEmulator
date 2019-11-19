//
// 6502 asm undocumented opcodes 
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/handlers/illegal.h"
#include "6502/handlers/arithmetic.h"
#include "6502/handlers/other.h"
#include "6502/handlers/memory.h"

void bitwise_shift_l_then_or(state6502 *state, asm6502 cmd) {
  bitwise_shift_l(state, cmd);
  bitwise_or(state, cmd);
}

void bitwise_rotate_l_then_and(state6502* state, asm6502 cmd) {
  bitwise_rotate_l(state, cmd);
  bitwise_and(state, cmd);
}

void bitwise_shift_r_then_xor(state6502* state, asm6502 cmd) {
  bitwise_shift_r(state, cmd);
  bitwise_xor(state, cmd);
}

void bitwise_rotate_r_then_adc(state6502* state, asm6502 cmd) {
  bitwise_rotate_r(state, cmd);
  add_with_carry(state, cmd);
}

void decrement_then_compare(state6502* state, asm6502 cmd) {
  decrement_memory(state, cmd);
  compare_accumulator(state, cmd);
}

void increment_then_sbc(state6502* state, asm6502 cmd) {
  increment_memory(state, cmd);
  subtract_with_carry(state, cmd);
}

// +1 cycle if page crossed
void load_acc_then_x(state6502* state, asm6502 cmd) {
  load_accumulator(state, cmd);
  load_xreg(state, cmd);
}

void bitwise_and_acc_x_then_store(state6502* state, asm6502 cmd) {
  uint8_t res = state->reg_a;
  res &= state->reg_x;
  uint16_t addr = handle_addr(state, cmd.maddr, NULL);
  memory6502_store(state->memory, addr, res);
}

void bitwise_and_with_carry(state6502* state, asm6502 cmd) {
  bitwise_and(state, cmd);
  state->status.carry = state->status.negative;
}

void bitwise_and_then_shift_r(state6502* state, asm6502 cmd) {
  bitwise_and(state, cmd);
  bitwise_shift_r(state, cmd);
}

void bitwise_and_then_rotate_r(state6502* state, asm6502 cmd) {
  bitwise_and(state, cmd);
  bitwise_rotate_r(state, cmd);
}

void transfer_x2a_then_and(state6502* state, asm6502 cmd) {
  transfer_x2a(state, cmd);
  bitwise_and(state, cmd);
}

// +1 cycle if page crossed
void bitwise_and_with_sp_then_store(state6502* state, asm6502 cmd) {
  uint16_t address = handle_addr_pbc(state, cmd.maddr);
  uint8_t res = memory6502_load(state->memory, address);
  res &= state->sp;
  state->reg_a = res;
  state->reg_x = res;
  state->sp = res;
  eval_sign_flag(state, res);
  eval_zero_flag(state, res);
}

void bitwise_and_with_x_then_sbc(state6502* state, asm6502 cmd) {
  uint8_t res = state->reg_a & state->reg_x;
  uint8_t areg = state->reg_a;
  uint8_t ovf = state->status.overflow;
  state->reg_a = res;
  state->status.carry = 1;
  subtract_with_carry(state, cmd);
  state->reg_x = state->reg_a;
  state->reg_a = areg;
  state->status.overflow = ovf;
}

void high_byte_shy(state6502* state, asm6502 cmd) {
  uint8_t res = (uint8_t) (cmd.maddr.value >> 8) + 1;
  res &= state->reg_y;
  uint16_t address = handle_addr(state, cmd.maddr, NULL);
  memory6502_store(state->memory, address, res);
}

void high_byte_shx(state6502* state, asm6502 cmd) {
  uint8_t res = (uint8_t) (cmd.maddr.value >> 8) + 1;
  res &= state->reg_x;
  uint16_t address = handle_addr(state, cmd.maddr, NULL);
  memory6502_store(state->memory, address, res);
}

void high_byte_tas(state6502* state, asm6502 cmd) {
  uint8_t hbyte = (uint8_t) (cmd.maddr.value >> 8) + 1;
  uint8_t newSp = state->reg_a & state->reg_x;
  state->sp = newSp;
  hbyte &= newSp;
  uint16_t address = handle_addr(state, cmd.maddr, NULL);
  memory6502_store(state->memory, address, hbyte);
}

void high_byte_ahx(state6502* state, asm6502 cmd) {
  uint8_t hbyte = (uint8_t) (cmd.maddr.value >> 8) + 1; 
  uint8_t res = (state->reg_a & state->reg_x & hbyte);
  uint16_t address = handle_addr(state, cmd.maddr, NULL);
  memory6502_store(state->memory, address, res);
}
