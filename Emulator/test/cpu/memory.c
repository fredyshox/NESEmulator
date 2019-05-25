//
// Memory operations tests
// Copyright (c) 2019 Kacper Rączy
//

#include "cputest.h"

uint8_t load_case[] = {
  /* .db */ 0x7e, 0x65, 0x25, 0x55, 0x01, 0x00, 0xff, 0xf9,
  /* .org(0x0008) */
  /* LDA #$7f */ 0xa9, 0x7f,
  /* .org(0x000a) */
  /* LDA $0000 */ 0xad, 0x00, 0x00,
  /* .org(0x000d) */
  /* LDA $0001, X*/ 0xbd, 0x01, 0x00,
  /* .org(0x0010) */
  /* LDA $0002, Y*/ 0xb9, 0x02, 0x00,
  /* .org(0x0013) */
  /* LDA $03 */ 0xa5, 0x04,
  /* .org(0x0015) */
  /* LDA $03, X */ 0xb5, 0x03,
  /* .org(0x0017) */
  /* LDA ($02, X) */ 0xa1, 0x02,
  /* .org(0x0019) */
  /* LDA ($04), Y*/ 0xb1, 0x04
  /* .org(0x001B) */
};

void test_load() {
  bool res = true;
  // prepare case
  state6502 cpu;
  TEST_PREPARE(cpu, load_case);
  // imm load
  cpu.pc = 0x0008;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x7f, "LDA imm", &res);
  // abs load
  cpu.pc = 0x000a;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x7e, "LDA abs", &res);
  // abs x load
  cpu.pc = 0x000d;
  cpu.reg_x = 0x01;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x25, "LDA abx", &res);
  // abs y load
  cpu.pc = 0x0010;
  cpu.reg_y = 0x01;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x55, "LDA aby", &res);
  // zp load
  cpu.pc = 0x0013;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x01, "LDA zp", &res);
  // zp x load
  cpu.pc = 0x0015;
  cpu.reg_x = 0x03;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0xff, "LDA zpx", &res);
  // indexed indirect load
  cpu.pc = 0x0017;
  cpu.reg_x = 0x02;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x65, "LDA inx", &res);
  // indirect indexed load
  cpu.pc = 0x0019;
  cpu.reg_y = 0x02;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x55, "LDA iny", &res);

  assert(res);
}

void test_load_edge_cases() {
  bool res = true;
  // prepare case
  state6502 cpu;
  memory6502 mem = {0, 0, NULL, NULL, NULL};
  mem.size = 0x200;
  cpu.memory = &mem;
  // indexed indirect zero page wrap
  uint8_t* mptr = malloc(sizeof(uint8_t) * 0x200);
  mem.mptr = mptr;
  mptr[0xff] = 0x10;
  mptr[0x00] = 0x01;
  mptr[0x100] = 0xa1; // LDA ($ff, X)
  mptr[0x101] = 0xff; // ^
  mptr[0x110] = 0xde;
  cpu.pc = 0x0100;
  cpu.reg_x = 0;
  execute_asm(&cpu);
  ASSERT_EQ(0xde, cpu.reg_a, "LDA ($ff,X) - zero page wrap", &res);

  // indirect indexed zero page wrap
  mptr[0x102] = 0xb1; // LDA ($ff), Y
  mptr[0x103] = 0xff; // ^
  mptr[0x112] = 0xab;
  cpu.pc = 0x0102;
  cpu.reg_y = 0x02;
  execute_asm(&cpu);
  ASSERT_EQ(0xab, cpu.reg_a, "LDA ($ff),Y - zero page wrap", &res);

  assert(res);
}

void test_load_flags() {
  // TODO
}

uint8_t store_case[] = {
  0x00, 0x00, 0x00, 0x00,
  // just few addressing modes
  // .org(0x0004)
  0x85, 0x00,  // LDA $00
  // .org(0x0006)
  0x8d, 0x0003 // LDA $0003
};

void test_store() {
  bool res = true;
  // prepare case
  state6502 cpu;
  TEST_PREPARE(cpu, store_case);
  // zp test
  cpu.pc = 0x0004;
  cpu.reg_a = 0xfe;
  execute_asm(&cpu);
  ASSERT_T(store_case[0x0000] == 0xfe, "STA zp", &res);
  // abs test
  cpu.pc = 0x0006;
  cpu.reg_a = 0x91;
  execute_asm(&cpu);
  ASSERT_T(store_case[0x0003] == 0x91, "STA abs", &res);

  assert(res);
}

void test_store_flags() {
  // TODO
}

void test_stack() {
  // TODO
}

int main(int argc, char** argv) {
  if (argc != 2)
    return 1;

  if (strcmp(argv[1], "--load") == 0) {
    test_load();
    test_load_flags();
    test_load_edge_cases();
  } else if (strcmp(argv[1], "--store") == 0) {
    test_store();
    test_store_flags();
  } else if (strcmp(argv[1], "--stack") == 0) {
    test_stack();
  }

  return 0;
}
