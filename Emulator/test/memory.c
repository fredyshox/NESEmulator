//
// Memory operations tests
// Copyright (c) 2019 Kacper Rączy
//

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "parser.h"
#include "state.h"  

#define ASSERT_T(EXPR, MSG, RES)  *RES = (EXPR); \
                                  if (EXPR) \
                                    fprintf(stderr, "\033[0;32mTest passed: %s\033[0m\n", MSG); \
                                  else \
                                    fprintf(stderr, "\033[0;31mTest failed: %s\033[0m\n", MSG);

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
  memset(&cpu, 0, sizeof(state6502));
  memory6502 memory = {0, 0, NULL, NULL, NULL, NULL};
  memory.mptr = load_case;
  memory.size = sizeof(load_case) / sizeof(uint8_t);
  cpu.memory = &memory;
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

void test_load_flags() {

}

void test_store() {

}

void test_store_flags() {

}

int main() {
  test_load();
  test_load_flags();
  test_store();
  test_store_flags();
  return 0;
}

