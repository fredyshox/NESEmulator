//
// 6502 binary parser
// Copyright (c) 2019 Kacper Rączy
//

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include <stdint.h>
#include "6502/asm.h"
#include "6502/addr.h"
#include "6502/state.h"
#include "6502/handlers/arithmetic.h"
#include "6502/handlers/branch.h"
#include "6502/handlers/memory.h"
#include "6502/handlers/other.h"
#include "6502/handlers/stack.h"

#define debug_print_cpu(state, asm) \
  do { \
    char buf[8]; \
    debug_print("%04x %s %s       A:%02x X:%02x Y:%02x P:%02x SP:%02x\n", \
                state->pc, ASM_STRING[asm.type], addr_to_string(asm.maddr, buf, 8), \
                state->reg_a, state->reg_x, state->reg_y, state->status.byte, state->sp); \
  } while (0)

int execute_asm(state6502 *state);
int parse_asm(asm6502 *cmd, memory6502 *memory, uint16_t pos);

#endif /* parser_h */
