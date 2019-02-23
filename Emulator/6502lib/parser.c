//
// 6502 binary parser
// Copyright (c) 2019 Kacper Rączy
//

#include "parser.h"

int execute_asm(state6502 *state) {
  // TOOD parse and execute instruction
  asm6502 operation; 
  int consumed = parse_asm(&operation, state->memory, state->pc);
  state->pc += (uint16_t) consumed;
  return 0;
}

// TODO: out of bounds error handling
int parse_asm(asm6502 *cmd, memory6502 *memory, uint16_t pos) {
  uint8_t opcode = memory6502_load(memory, pos);
  uint16_t mpos = pos + 1;
  int consumed = 1;
  mem_addr addr;
  switch (opcode) {
    // ADC - add with carry
    case 0x69:
      consumed += immediate_addr(&addr, memory, mpos);
      goto adc;
    case 0x6d:
      consumed += absolute_addr(&addr, memory, mpos);
      goto adc;
    case 0x7d:
      consumed += absolute_x_addr(&addr, memory, mpos);
      goto adc;
    case 0x79:
      consumed += absolute_y_addr(&addr, memory, mpos);
      goto adc;
    case 0x65:
      consumed += zeropage_addr(&addr, memory, mpos);
      goto adc;
    case 0x75:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      goto adc;
    case 0x61:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      goto adc;
    case 0x71:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      adc:
        cmd->type = ADC_ASM;
        cmd->maddr = addr;
        cmd->handler = add_with_carry;
        break;
    // AND
    case 0x29:
      consumed += immediate_addr(&addr, memory, mpos);
      goto and;
    case 0x2d:
      consumed += absolute_addr(&addr, memory, mpos);
      goto and;
    case 0x3d:
      consumed += absolute_x_addr(&addr, memory, mpos);
      goto and;
    case 0x39:
      consumed += absolute_y_addr(&addr, memory, mpos);
      goto and;
    case 0x25:
      consumed += zeropage_addr(&addr, memory, mpos);
      goto and;
    case 0x35:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      goto and;
    case 0x21:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      goto and;
    case 0x31:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      and:
        cmd->type = AND_ASM;
        cmd->maddr = addr;
        // cmd->handler = 
        break;
    // ASL - arithmentic bitwise shift left
    // BCC
    // BCS
    // BEQ
    // BIT
    // BMI
    // BNE
    // BPL 
    // BRK
    // BVC
    // BVS
    // CLC
    // CLD
    // CLI
    // CLV
    // CMP
    // CPX
    // CPY
    // DEC 
    case 0xce:
      consumed += absolute_addr(&addr, memory, mpos);
      goto dec;
    case 0xde:
      consumed += absolute_x_addr(&addr, memory, mpos);
      goto dec;
    case 0xc6:
      consumed += zeropage_addr(&addr, memory, mpos);
      goto dec;
    case 0xd6:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      dec:
        cmd->type = DEC_ASM;
        cmd->maddr = addr;
        cmd->handler = decrement_memory;
        break;
    // DEX
    case 0xca:
      cmd->type = DEX_ASM;
      cmd->maddr.type = IMP_ADDR;
      cmd->handler = decrement_x;
    // DEY
    case 0x88:
      cmd->type = DEY_ASM;
      cmd->maddr.type = IMP_ADDR;
      cmd->handler = decrement_y;
    // EOR
    // INC
    case 0xee:
      consumed += absolute_addr(&addr, memory, mpos);
      goto inc;
    case 0xfe:
      consumed += absolute_x_addr(&addr, memory, mpos);
      goto inc;
    case 0xe6:
      consumed += zeropage_addr(&addr, memory, mpos);
      goto inc;
    case 0xf6:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      inc:
        cmd->type = INC_ASM;
        cmd->maddr = addr;
        cmd->handler = increment_memory;
        break;
    // INX
    case 0xe8:
      cmd->type = INX_ASM;
      cmd->maddr.type = IMP_ADDR;
      cmd->handler = increment_x;
      break;
    // INY
    case 0xc8:
      cmd->type = INY_ASM;
      cmd->maddr.type = IMP_ADDR;
      cmd->handler = increment_y;
      break;
    // JMP
    // JSR
    // LDA
    case 0xa9:
      consumed += immediate_addr(&addr, memory, mpos);
      goto lda;
    case 0xad:
      consumed += absolute_addr(&addr, memory, mpos);
      goto lda;
    case 0xbd:
      consumed += absolute_x_addr(&addr, memory, mpos);
      goto lda;
    case 0xb9:
      consumed += absolute_y_addr(&addr, memory, mpos);
      goto lda;
    case 0xa5:
      consumed += zeropage_addr(&addr, memory, mpos);
      goto lda;
    case 0xb5:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      goto lda;
    case 0xa1:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      goto lda;
    case 0xb1:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      lda:
        cmd->type = LDA_ASM;
        cmd->maddr = addr;
        cmd->handler = load_accumulator;
        break;
    // LDX
    case 0xa2:
      consumed += immediate_addr(&addr, memory, mpos);
      goto ldx;
    case 0xae:
      consumed += absolute_addr(&addr, memory, mpos);
      goto ldx;
    case 0xbe:
      consumed += absolute_y_addr(&addr, memory, mpos);
      goto ldx;
    case 0xa6:
      consumed += zeropage_addr(&addr, memory, mpos);
      goto ldx;
    case 0xb6:
      consumed += zeropage_y_addr(&addr, memory, mpos);
      ldx:
        cmd->type = LDX_ASM;
        cmd->maddr = addr;
        cmd->handler = load_xreg;
        break;
    // LDY
    // LSR
    // NOP
    // ORA
    // PHA
    // PHP
    // PLA
    // PLP
    // ROL
    // ROR
    // RTI
    // RTS
    // SBC
    case 0xe9:
      consumed += immediate_addr(&addr, memory, mpos);
      goto sbc;
    case 0xed:
      consumed += absolute_addr(&addr, memory, mpos);
      goto sbc;
    case 0xfd:
      consumed += absolute_x_addr(&addr, memory, mpos);
      goto sbc;
    case 0xf9:
      consumed += absolute_y_addr(&addr, memory, mpos);
      goto sbc;
    case 0xe5:
      consumed += zeropage_addr(&addr, memory, mpos);
      goto sbc;
    case 0xf5:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      goto sbc;
    case 0xe1:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      goto sbc;
    case 0xf1:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      sbc:
        cmd->type = SBC_ASM;
        cmd->maddr = addr;
        cmd->handler = subtract_with_carry;
        break;
    // SEC
    // SED
    // SEI
    // STA
    // STX
    // STY
    // TAX
    // TAY
    // TSX
    // TXA
    // TXS
    // TYA
    default:
      fprintf(stderr, "Error: unsupported opcode %x\n", opcode);
      consumed = 0;
      break;
  }

  return consumed;
}
