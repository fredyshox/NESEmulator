//
// Test interrupts
// Copyright (c) 2019 Kacper Rączy
//

#include "cputest.h"

uint8_t load_handler(struct memory6502* mem, uint16_t addr)  {
    if (addr < 0xfff) {
      return mem->mptr[addr];
    } else if (addr == 0xfffa) {
      return 0x00;
    } else if (addr == 0xfffb) {
      return 0x0c;
    }

    return 0x00;
}

int main(int argc, char** argv) {
  bool res = true;
  state6502 cpu;
  memory6502 mem;

  memory6502_create(&mem, 0xfff);
  mem.mptr[0xc00] = 0xe8; // INX
  mem.mptr[0xc01] = 0x40; // RTI
  mem.load_handler = load_handler;
  state6502_create(&cpu, &mem);
  cpu.pc = 0x100;
  // nmi interrupt
  cpu.incoming_int = NMI_INT;
  execute_asm(&cpu);
  ASSERT_EQ(0xc01, cpu.pc, "(1) nmi interrupt pc", &res);
  ASSERT_EQ(0x01, cpu.reg_x, "(2) nmi instruction exec (inx)", &res);
  ASSERT_EQ(NONE_INT, cpu.incoming_int, "(3) incoming int flag", &res);
  if (!res) goto ret;
  // return from int
  execute_asm(&cpu);
  ASSERT_EQ(0x100, cpu.pc, "(4) rti pc", &res);
  ASSERT_EQ(0xff, cpu.sp, "(5) rti stack pointer", &res);

  ret:
    free(mem.mptr);
    assert(res);
    return 0;
}
