//
// Test op which manipulate flags (compares etc.)
// Copyright (c) 2019 Kacper Rączy
//

#include "cputest.h"

//TODO test flag operation and compares

void test_flags() {
  // TODO
}

uint8_t cmp_case[] = {
  /* .org(0x0000) */
  /* CMP #$ff */ 0xc9, 0xff,
  /* CMP #$7f */ 0xc9, 0x7f,
  /* CMP #$00 */ 0xc9, 0x00,
  /* CPX #$11 */ 0xe0, 0x11,
  /* CPY #$22 */ 0xc0, 0x22
};

void test_compares() {
  bool res = true;
  state6502 cpu;
  TEST_PREPARE(cpu, cmp_case);
  // test1a cmp gt
  cpu.reg_a = 0x00;
  cpu.pc = 0x0000;
  execute_asm(&cpu);
  ASSERT_T(cpu.status.negative == 0 && cpu.status.zero == 0 && cpu.status.carry == 0, "(1) (a) CMP (reg a is greater)", &res);
  PRINT_STATUS(cpu, stderr);
  // test1b cmp eq
  cpu.reg_a = 0x00;
  cpu.pc = 0x0004;
  execute_asm(&cpu);
  ASSERT_T(cpu.status.negative == 0 && cpu.status.zero == 1 && cpu.status.carry == 1, "(1) (b) CMP (reg a is equal)", &res);
  PRINT_STATUS(cpu, stderr);
  // test1c cmp lt
  cpu.reg_a = 0x10;
  cpu.pc = 0x0006;
  execute_asm(&cpu);
  ASSERT_T(cpu.status.negative == 1 && cpu.status.zero == 0 && cpu.status.carry == 0, "(1) (c) CMP (reg a is lesser)", &res);
  PRINT_STATUS(cpu, stderr);

  assert(res);
}

int main(int argc, char** argv) {
  if (argc != 2)
    return 1;

  if (strcmp(argv[1], "--flag") == 0) {
    test_flags();
  } else if (strcmp(argv[1], "--cmp") == 0) {
    test_compares();
  }

  return 0;
}
