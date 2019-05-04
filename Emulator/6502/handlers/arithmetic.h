//
// 6502 asm arithmetic handlers
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_arithmetic_h
#define asm_arithmetic_h

#include "asm.h"
#include "state.h"

// Arithmetic
void add_with_carry(state6502 *state, asm6502 cmd);
void increment_memory(state6502 *state, asm6502 cmd);
void increment_x(state6502 *state, asm6502 cmd);
void increment_y(state6502 *state, asm6502 cmd);
void subtract_with_carry(state6502 *state, asm6502 cmd);
void decrement_memory(state6502 *state, asm6502 cmd);
void decrement_x(state6502 *state, asm6502 cmd);
void decrement_y(state6502 *state, asm6502 cmd);

// Bitwise
void bitwise_shift_l(state6502 *state, asm6502 cmd);
void bitwise_shift_r(state6502 *state, asm6502 cmd);
void bitwise_and(state6502 *state, asm6502 cmd);
void bitwise_xor(state6502 *state, asm6502 cmd);
void bitwise_or(state6502 *state, asm6502 cmd);
void bitwise_rotate_l(state6502 *state, asm6502 cmd);
void bitwise_rotate_r(state6502 *state, asm6502 cmd);
void bitwise_bit_test(state6502 *state, asm6502 cmd);

#endif /* arithmetic_h */
