//
// Arithmetic operations tests
// Copyright (c) 2019 Kacper Rączy
//

#include "test.h"

uint8_t adc_case[] = {
  /* .org(0x0000) */
  /* ADC #$00*/ 0x69, 0x00,
  /* ADC #$10 */ 0x69, 0x10,
  /* ADC #$ff */ 0x69, 0xff,
  /* ADC #$7f */ 0x69, 0x7f
};

uint8_t sbc_case[] = {
  /* .org(0x0000) */
  /* SBC */
};

void test_adc() {
  bool res = true;
  // prepare case 
  state6502 cpu;
  TEST_PREPARE(cpu, adc_case);
  // zero  
  cpu.pc = 0x0000;
  cpu.reg_a = 0x00;
  execute_asm(&cpu);  
  ASSERT_T(cpu.reg_a == 0x00, "(1) ADC: 0 + 0", &res);
  ASSERT_T(cpu.status.zero == 1, "(1) ADC: status zero", &res);
  // sth
  cpu.pc = 0x0002;
  cpu.reg_a = 0x23;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x33, "(2) ADC: 0x10 + 0x23", &res);
  ASSERT_T(cpu.status.negative == 0, "(2) ADC: status positive", &res);
  // carry 
  cpu.pc = 0x0004;
  cpu.reg_a = 0x02;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0x01, "(3) ADC: 0xff + 0x02", &res);
  ASSERT_T(cpu.status.carry == 1, "(3) ADC: status carry", &res);
  // overflow (carry is 1)
  cpu.pc = 0x0006;
  cpu.reg_a = 0x23;
  execute_asm(&cpu);
  ASSERT_T(cpu.reg_a == 0xa3, "(4) ADC: 0x7f + 0x23 + C", &res);
  ASSERT_T(cpu.status.overflow == 1 && cpu.status.negative == 1, "(4) ADC: status overflow", &res);

  assert(res);
}

void test_sbc() {
  // just carry and overflow 
}

void test_inc() {

}

void test_dec() {

}

int main() {
  test_adc();
  test_sbc();
  test_inc();
  test_dec();
  return 0;
}

