//
// Tests of program counter malipulators
// Copyright (c) 2019 Kacper Rączy
//

// TODO test jsr

#include "test.h"

uint8_t branch_case[] = {
  /* some garbage */  0x01, 0x01,
                      0x01, 0x01,
  /* .org(0x0004) */
  /* BCC $#40 (rel) */0x90, 0x40,
  /* BCS $#ff (rel) */0xb0, 0xff
};

// tests mainly relative addressing
void test_branch() {
  bool res = true;
  state6502 cpu;
  TEST_PREPARE(cpu, branch_case);
  // test1
  cpu.pc = 4;
  cpu.status.carry = 0;
  execute_asm(&cpu);
  ASSERT_T(cpu.pc == 0x04 + 0x40 + 0x02, "(1) BCC (pc: 0x04, step: 0x40, len: 0x02)", &res);
  // test2
  cpu.pc = 6;
  cpu.status.carry = 1;
  execute_asm(&cpu);
  ASSERT_T(cpu.pc == (uint16_t)(0x08 + (int8_t) 0xff), "(2) BCC (pc: 0x06, step: 0xff, len: 0x02)", &res);

  assert(res);
}

// tests mainly indirect addressing
void test_jump() {
  bool res = true;
  // test case
  uint8_t test[0x200];
  test[0x000] = 0x6c; // JMP
  test[0x001] = 0x20; // ($0120)
  test[0x002] = 0x01; // ^
  test[0x120] = 0xff;
  test[0x121] = 0x01;

  state6502 cpu;
  TEST_PREPARE(cpu, test);
  // test jmp
  cpu.pc = 0;
  execute_asm(&cpu);
  ASSERT_T(cpu.pc == 0x01ff, "(1) JMP ($0120) (indirect addressing)", &res);

  assert(res);
}

void test_jsr() {
  bool res = true;

  assert(res);
}

void test_returns() {
  bool res = true;
  // test case
  uint8_t test[0x200];
  test[0] = 0x60; // RTS
  test[1] = 0x40; // RTI

  state6502 cpu;
  TEST_PREPARE(cpu, test);
  // RTS test
  cpu.pc = 0;
  cpu.sp = 0xfd;
  test[0x1ff] = 0xff;
  test[0x1fe] = 0x11;
  execute_asm(&cpu);
  ASSERT_T(cpu.pc == 0x11ff + 1 && cpu.sp == 0xff, "(1) RTS (stack: 0xfe : 0x11, 0xff : 0xff)", &res);
  // RTI test
  cpu.pc = 1;
  cpu.sp = 0xfc;
  // stack: psw,lsb,msb
  test[0x1fd] = 0x01;
  execute_asm(&cpu);
  ASSERT_T(cpu.pc == 0xff11 && cpu.sp == 0xff, "(2) RTI (program counter)", &res);
  ASSERT_T((*(uint8_t*) &cpu.status) == 0x01, "(2) RTI (cpu status)", &res);

  assert(res);
}

int main(int argc, char** argv) {
  if (argc != 2)
    return 1;

  if (strcmp(argv[1], "--branch") == 0) {
    test_branch();
  } else if (strcmp(argv[1], "--jump") == 0) {
    test_jump();
  } else if (strcmp(argv[1], "--jsr") == 0) {
    test_jsr();
  } else if (strcmp(argv[1], "--return") == 0) {
    test_returns();
  }

  return 0;
}
