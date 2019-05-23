//
// 6502 asm instructions
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/asm.h"
#include <stdbool.h>
#include <assert.h>

const char* ASM_STRING[] = {
  FOREACH_ASM(GENERATE_ASM_STRING)
};

struct asm6502 asm6502_create(int type, struct mem_addr addr, void (*handler)(state6502*, asm6502)) {
  struct asm6502 instruction;
  instruction.type = type;
  instruction.cycles = 0;
  instruction.maddr = addr;
  instruction.handler = handler;

  return instruction;
}

void asm6502_execute(struct asm6502 cmd, struct state6502* state) {
  cmd.handler(state, cmd);
}
