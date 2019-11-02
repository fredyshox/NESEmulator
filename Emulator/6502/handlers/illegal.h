//
// 6502 asm undocumented opcodes 
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_illegal_h
#define asm_illegal_h

#include "6502/addr.h"
#include "6502/asm.h"
#include "6502/state.h"

void bitwise_shift_l_then_or(state6502 *state, asm6502 cmd);
void bitwise_rotate_l_then_and(state6502* state, asm6502 cmd);
void bitwise_shift_r_then_xor(state6502* state, asm6502 cmd);
void bitwise_rotate_r_then_adc(state6502* state, asm6502 cmd);
void decrement_then_compare(state6502* state, asm6502 cmd);
void increment_then_sbc(state6502* state, asm6502 cmd);
void load_acc_then_x(state6502* state, asm6502 cmd);
void bitwise_and_acc_x_then_store(state6502* state, asm6502 cmd);
void bitwise_and_with_carry(state6502* state, asm6502 cmd);
void bitwise_and_then_shift_r(state6502* state, asm6502 cmd);
void bitwise_and_then_rotate_r(state6502* state, asm6502 cmd);
void transfer_x2a_then_and(state6502* state, asm6502 cmd);
void bitwise_and_with_sp_then_store(state6502* state, asm6502 cmd);
void bitwise_and_with_x_then_sbc(state6502* state, asm6502 cmd);
void high_byte_shy(state6502* state, asm6502 cmd);
void high_byte_shx(state6502* state, asm6502 cmd);
void high_byte_tas(state6502* state, asm6502 cmd);
void high_byte_ahx(state6502* state, asm6502 cmd);

#endif /* asm_illegal_h */