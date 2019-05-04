//
// 6502 asm other handlers
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_other_h
#define asm_other_h

#include "asm.h"
#include "state.h"

// Flags
void clear_carry(state6502 *state, asm6502 cmd);
void set_carry(state6502 *state, asm6502 cmd);
void clear_interrupt(state6502 *state, asm6502 cmd);
void set_interrupt(state6502 *state, asm6502 cmd);
void clear_overflow(state6502 *state, asm6502 cmd);

// Comparsion
void compare_accumulator(state6502 *state, asm6502 cmd);
void compare_xreg(state6502 *state, asm6502 cmd);
void compare_yreg(state6502 *state, asm6502 cmd);

// Other
void no_operation(state6502 *state, asm6502 cmd);

#endif /* asm_other_h */
