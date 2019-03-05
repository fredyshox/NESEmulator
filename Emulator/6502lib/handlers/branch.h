//
// 6502 asm branch handlers
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_branch_h
#define asm_branch_h

#include "addr.h"
#include "asm.h"
#include "state.h"

#define JUMP_SIZE 3
#define BRANCH_SIZE 2

void branch_plus(state6502* state, asm6502 cmd);
void branch_minus(state6502* state, asm6502 cmd);
void branch_carry(state6502* state, asm6502 cmd);
void branch_carry_c(state6502* state, asm6502 cmd);
void branch_overflow(state6502* state, asm6502 cmd);
void branch_overflow_c(state6502* state, asm6502 cmd);
void branch_eq(state6502* state, asm6502 cmd);
void branch_neq(state6502* state, asm6502 cmd);
void jump(state6502* state, asm6502 cmd);
void jump_subroutine(state6502* state, asm6502 cmd);
void return_subroutine(state6502* state, asm6502 cmd);

#endif /* asm_branch_h */
