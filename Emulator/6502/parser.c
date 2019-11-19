//
// 6502 binary parser
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/parser.h"
#include "6502/addr.h"
// handlers
#include "6502/handlers/arithmetic.h"
#include "6502/handlers/branch.h"
#include "6502/handlers/memory.h"
#include "6502/handlers/other.h"
#include "6502/handlers/stack.h"
#ifdef ILLEGAL_OPCODES
#include "6502/handlers/illegal.h"
#endif

int execute_asm(state6502 *state) {
  if (state->incoming_int != NONE_INT) {
    //TODO what about handle interrupt cpu cycles?
    interrupt6502_handle(state);
    state->incoming_int = NONE_INT;
  }

  asm6502 operation;
  int consumed = parse_asm(&operation, state->memory, state->pc);
  debug_print_cpu(state, operation);
  //execution
  int cycles = 0;
  if (operation.cycles) {
    state->pc += (uint16_t) consumed;
    asm6502_execute(operation, state);
    cycles = operation.cycles + (state->page_crossed ? 1 : 0);
  }
  state->page_crossed = false;

  return cycles;
}

int parse_asm(asm6502 *cmd, memory6502 *memory, uint16_t pos) {
  uint8_t opcode = memory6502_load(memory, pos);
  uint16_t mpos = pos + 1;
  int consumed = 1, cycles = 2; // default 2 machine cycles
  mem_addr addr;
  // TODO support for page boundary additional cycles and branch taken cycles
  // good compiler will transform this into a jump table, so im leaving this for now
  switch (opcode) {
    // ADC - add with carry
    case 0x69:
      consumed += immediate_addr(&addr, memory, mpos);
      goto adc;
    case 0x6d:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto adc;
    case 0x7d:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto adc;
    case 0x79:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto adc;
    case 0x65:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto adc;
    case 0x75:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto adc;
    case 0x61:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto adc;
    case 0x71:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      adc:
        cmd->type = ADC_ASM;
        cmd->handler = add_with_carry;
        break;
    // AND
    case 0x29:
      consumed += immediate_addr(&addr, memory, mpos);
      goto and;
    case 0x2d:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto and;
    case 0x3d:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto and;
    case 0x39:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto and;
    case 0x25:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto and;
    case 0x35:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto and;
    case 0x21:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto and;
    case 0x31:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      and:
        cmd->type = AND_ASM;
        cmd->handler = bitwise_and;
        break;
    // ASL - arithmentic bitwise shift left
    case 0x0a:
      addr.type = ACC_ADDR;
      goto asl;
    case 0x0e:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto asl;
    case 0x1e:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      goto asl;
    case 0x06:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto asl;
    case 0x16:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      asl:
        cmd->type = ASL_ASM;
        cmd->handler = bitwise_shift_l;
        break;
    // BCC
    case 0x90:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BCC_ASM;
      cmd->handler = branch_carry_c;
      break;
    // BCS
    case 0xb0:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BCS_ASM;
      cmd->handler = branch_carry;
      break;
    // BEQ
    case 0xf0:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BEQ_ASM;
      cmd->handler = branch_eq;
      break;
    // BIT
    case 0x2c:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto bit;
    case 0x24:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      bit:
        cmd->type = BIT_ASM;
        cmd->handler = bitwise_bit_test;
        break;
    // BMI
    case 0x30:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BMI_ASM;
      cmd->handler = branch_minus;
      break;
    // BNE
    case 0xd0:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BNE_ASM;
      cmd->handler = branch_neq;
      break;
    // BPL
    case 0x10:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BPL_ASM;
      cmd->handler = branch_plus;
      break;
    // BRK
    case 0x00:
      cmd->type = BRK_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = break_interrupt;
      cycles = 7;
      break;
    // BVC
    case 0x50:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BVC_ASM;
      cmd->handler = branch_overflow_c;
      break;
    // BVS
    case 0x70:
      consumed += relative_addr(&addr, memory, mpos);
      cmd->type = BVS_ASM;
      cmd->handler = branch_overflow;
      break;
    // CLC
    case 0x18:
      cmd->type = CLC_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = clear_carry;
      break;
    // CLD
    case 0xD8:
      cmd->type = CLD_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = clear_decimal;
      break;
    // CLI
    case 0x58:
      cmd->type = CLI_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = clear_interrupt;
      break;
    // CLV
    case 0xb8:
      cmd->type = CLV_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = clear_overflow;
      break;
    // CMP
    case 0xc9:
      consumed += immediate_addr(&addr, memory, mpos);
      goto cmp;
    case 0xcd:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto cmp;
    case 0xdd:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto cmp;
    case 0xd9:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto cmp;
    case 0xc5:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto cmp;
    case 0xd5:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto cmp;
    case 0xc1:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto cmp;
    case 0xd1:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      cmp:
        cmd->type = CMP_ASM;
        cmd->handler = compare_accumulator;
        break;
    // CPX
    case 0xe0:
      consumed += immediate_addr(&addr, memory, mpos);
      goto cpx;
    case 0xec:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto cpx;
    case 0xe4:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      cpx:
        cmd->type = CPX_ASM;
        cmd->handler = compare_xreg;
        break;
    // CPY
    case 0xc0:
      consumed += immediate_addr(&addr, memory, mpos);
      goto cpy;
    case 0xcc:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto cpy;
    case 0xc4:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      cpy:
        cmd->type = CPY_ASM;
        cmd->handler = compare_yreg;
        break;
    // DEC
    case 0xce:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto dec;
    case 0xde:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      goto dec;
    case 0xc6:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto dec;
    case 0xd6:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      dec:
        cmd->type = DEC_ASM;
        cmd->handler = decrement_memory;
        break;
    // DEX
    case 0xca:
      cmd->type = DEX_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = decrement_x;
      break;
    // DEY
    case 0x88:
      cmd->type = DEY_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = decrement_y;
      break;
    // EOR
    case 0x49:
      consumed += immediate_addr(&addr, memory, mpos);
      goto eor;
    case 0x4d:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto eor;
    case 0x5d:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto eor;
    case 0x59:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto eor;
    case 0x45:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto eor;
    case 0x55:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto eor;
    case 0x41:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto eor;
    case 0x51:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      eor:
        cmd->type = EOR_ASM;
        cmd->handler = bitwise_xor;
        break;
    // INC
    case 0xee:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto inc;
    case 0xfe:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      goto inc;
    case 0xe6:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto inc;
    case 0xf6:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      inc:
        cmd->type = INC_ASM;
        cmd->handler = increment_memory;
        break;
    // INX
    case 0xe8:
      cmd->type = INX_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = increment_x;
      break;
    // INY
    case 0xc8:
      cmd->type = INY_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = increment_y;
      break;
    // JMP
    case 0x4c:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 3;
      goto jmp;
    case 0x6c:
      consumed += indirect_addr(&addr, memory, mpos);
      cycles = 5;
      jmp:
        cmd->type = JMP_ASM;
        cmd->handler = jump;
        break;
    // JSR
    case 0x20:
      consumed += absolute_addr(&addr, memory, mpos);
      cmd->type = JSR_ASM;
      cmd->handler = jump_subroutine;
      cycles = 6;
      break;
    // LDA
    case 0xa9:
      consumed += immediate_addr(&addr, memory, mpos);
      goto lda;
    case 0xad:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto lda;
    case 0xbd:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto lda;
    case 0xb9:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto lda;
    case 0xa5:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto lda;
    case 0xb5:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto lda;
    case 0xa1:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto lda;
    case 0xb1:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      lda:
        cmd->type = LDA_ASM;
        cmd->handler = load_accumulator;
        break;
    // LDX
    case 0xa2:
      consumed += immediate_addr(&addr, memory, mpos);
      goto ldx;
    case 0xae:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto ldx;
    case 0xbe:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto ldx;
    case 0xa6:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto ldx;
    case 0xb6:
      consumed += zeropage_y_addr(&addr, memory, mpos);
      cycles = 4;
      ldx:
        cmd->type = LDX_ASM;
        cmd->handler = load_xreg;
        break;
    // LDY
    case 0xa0:
      consumed += immediate_addr(&addr, memory, mpos);
      goto ldy;
    case 0xac:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto ldy;
    case 0xbc:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto ldy;
    case 0xa4:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto ldy;
    case 0xb4:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      ldy:
        cmd->type = LDY_ASM;
        cmd->handler = load_yreg;
        break;
    // LSR
    case 0x4a:
      addr.type = ACC_ADDR;
      goto lsr;
    case 0x4e:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto lsr;
    case 0x5e:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      goto lsr;
    case 0x46:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto lsr;
    case 0x56:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      lsr:
        cmd->type = LSR_ASM;
        cmd->handler = bitwise_shift_r;
        break;
    // NOP
    #ifdef ILLEGAL_OPCODES
    case 0x0c:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto nop;
    case 0x1c:
    case 0x3c:
    case 0x5c:
    case 0x7c:
    case 0xdc:
    case 0xfc:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto nop;
    case 0x04:
    case 0x44:
    case 0x64:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto nop;
    case 0x14:
    case 0x34:
    case 0x54:
    case 0x74:
    case 0xd4:
    case 0xf4:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto nop;
    case 0x80:
    case 0x82:
    case 0x89:
    case 0xc2:
    case 0xe2:
      consumed += immediate_addr(&addr, memory, mpos);
      goto nop;
    case 0x1a:
    case 0x3a:
    case 0x5a:
    case 0x7a:
    case 0xda:
    case 0xfa:
    #endif
    case 0xea:
      addr.type = IMP_ADDR;
      nop:
        cmd->type = NOP_ASM;
        cmd->handler = no_operation;
        break;
    // ORA
    case 0x09:
      consumed += immediate_addr(&addr, memory, mpos);

      goto ora;
    case 0x0d:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto ora;
    case 0x1d:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto ora;
    case 0x19:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto ora;
    case 0x05:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto ora;
    case 0x15:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto ora;
    case 0x01:
      consumed += indexed_indirect_addr(&addr, memory ,mpos);
      cycles = 6;
      goto ora;
    case 0x11:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      ora:
        cmd->type = ORA_ASM;
        cmd->handler = bitwise_or;
        break;
    // PHA
    case 0x48:
      cmd->type = PHA_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = push_accumulator;
      cycles = 3;
      break;
    // PHP
    case 0x08:
      cmd->type = PHP_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = push_cpu_status;
      cycles = 3;
      break;
    // PLA
    case 0x68:
      cmd->type = PLA_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = pull_accumulator;
      cycles = 4;
      break;
    // PLP
    case 0x28:
      cmd->type = PLP_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = pull_cpu_status;
      cycles = 4;
      break;
    // ROL
    case 0x2a:
      addr.type = ACC_ADDR;
      goto rol;
    case 0x2e:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto rol;
    case 0x3e:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      goto rol;
    case 0x26:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto rol;
    case 0x36:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      rol:
        cmd->type = ROL_ASM;
        cmd->handler = bitwise_rotate_l;
        break;
    // ROR
    case 0x6a:
      addr.type = ACC_ADDR;
      goto ror;
    case 0x6e:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto ror;
    case 0x7e:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      goto ror;
    case 0x66:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto ror;
    case 0x76:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      ror:
        cmd->type = ROR_ASM;
        cmd->handler = bitwise_rotate_r;
        break;
    // RTI
    case 0x40:
      cmd->type = RTI_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = return_interrupt;
      cycles = 6;
      break;
    // RTS
    case 0x60:
      cmd->type = RTS_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = return_subroutine;
      cycles = 6;
      break;
    // SBC
    #ifdef ILLEGAL_OPCODES
    case 0xeb:
    #endif
    case 0xe9:
      consumed += immediate_addr(&addr, memory, mpos);
      goto sbc;
    case 0xed:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto sbc;
    case 0xfd:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto sbc;
    case 0xf9:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto sbc;
    case 0xe5:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto sbc;
    case 0xf5:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto sbc;
    case 0xe1:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto sbc;
    case 0xf1:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      sbc:
        cmd->type = SBC_ASM;
        cmd->handler = subtract_with_carry;
        break;
    // SEC
    case 0x38:
      cmd->type = SEC_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = set_carry;
      break;
    // SED
    case 0xf8:
      cmd->type = SED_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = set_decimal;
      break;
    // SEI
    case 0x78:
      cmd->type = SEI_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = set_interrupt;
      break;
    // STA
    case 0x8d:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto sta;
    case 0x9d:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 5;
      goto sta;
    case 0x99:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 5;
      goto sta;
    case 0x85:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto sta;
    case 0x95:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      goto sta;
    case 0x81:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto sta;
    case 0x91:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 6;
      sta:
        cmd->type = STA_ASM;
        cmd->handler = store_accumulator;
        break;
    // STX
    case 0x8e:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto stx;
    case 0x86:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto stx;
    case 0x96:
      consumed += zeropage_y_addr(&addr, memory, mpos);
      cycles = 4;
      stx:
        cmd->type = STX_ASM;
        cmd->handler = store_xreg;
        break;
    // STY
    case 0x8c:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto sty;
    case 0x84:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto sty;
    case 0x94:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 4;
      sty:
        cmd->type = STY_ASM;
        cmd->handler = store_yreg;
        break;
    // TAX
    case 0xaa:
      cmd->type = TAX_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = transfer_a2x;
      break;
    // TXA
    case 0x8a:
      cmd->type = TXA_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = transfer_x2a;
      break;
    // TAY
    case 0xa8:
      cmd->type = TAY_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = transfer_a2y;
      break;
    // TYA
    case 0x98:
      cmd->type = TYA_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = transfer_y2a;
      break;
    // TSX
    case 0xba:
      cmd->type = TSX_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = transfer_sptr2x;
      break;
    // TXS
    case 0x9a:
      cmd->type = TXS_ASM;
      addr.type = IMP_ADDR;
      cmd->handler = transfer_x2sptr;
      break;
    #ifdef ILLEGAL_OPCODES
    // SLO
    case 0x03:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 8;
      goto slo;
    case 0x13:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 8;
      goto slo;
    case 0x07:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto slo;
    case 0x17:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      goto slo;
    case 0x1b:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 7;
      goto slo;
    case 0x0f:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto slo;
    case 0x1f:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      slo:
        cmd->type = SLO_ASM;
        cmd->handler = bitwise_shift_l_then_or;
        break;
    // RLA
    case 0x23:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 8;
      goto rla;
    case 0x33:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 8;
      goto rla;
    case 0x27:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto rla;
    case 0x37:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      goto rla;
    case 0x3b:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 7;
      goto rla;
    case 0x2f:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto rla;
    case 0x3f:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      rla:
        cmd->type = RLA_ASM;
        cmd->handler = bitwise_rotate_l_then_and;
        break;
    // SRE
    case 0x43:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 8;
      goto sre;
    case 0x53:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 8;
      goto sre;
    case 0x47:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto sre;
    case 0x57:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      goto sre;
    case 0x5b:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 7;
      goto sre;
    case 0x4f:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto sre;
    case 0x5f:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      sre:
        cmd->type = SRE_ASM;
        cmd->handler = bitwise_shift_r_then_xor;
        break;
    // RRA
    case 0x63:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 8;
      goto rra;
    case 0x73:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 8;
      goto rra;
    case 0x67:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto rra;
    case 0x77:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      goto rra;
    case 0x7b:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 7;
      goto rra;
    case 0x6f:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto rra;
    case 0x7f:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      rra:
        cmd->type = RRA_ASM;
        cmd->handler = bitwise_rotate_r_then_adc;
        break;
    // DCP
    case 0xc3:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 8;
      goto dcp;
    case 0xd3:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 8;
      goto dcp;
    case 0xc7:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto dcp;
    case 0xd7:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      goto dcp;
    case 0xdb:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 7;
      goto dcp;
    case 0xcf:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto dcp;
    case 0xdf:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      dcp:
        cmd->type = DCP_ASM;
        cmd->handler = decrement_then_compare;
        break;
    // ISC
    case 0xe3:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 8;
      goto isc;
    case 0xf3:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 8;
      goto isc;
    case 0xe7:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 5;
      goto isc;
    case 0xf7:
      consumed += zeropage_x_addr(&addr, memory, mpos);
      cycles = 6;
      goto isc;
    case 0xfb:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 7;
      goto isc;
    case 0xef:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 6;
      goto isc;
    case 0xff:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 7;
      isc:
        cmd->type = ISC_ASM;
        cmd->handler = increment_then_sbc;
        break;
    // SAX
    case 0x83:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto sax;
    case 0x87:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto sax;
    case 0x97:
      consumed += zeropage_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto sax;
    case 0x8f:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      sax:
        cmd->type = SAX_ASM;
        cmd->handler = bitwise_and_acc_x_then_store;
        break;
    // LAX 
    case 0xa3:
      consumed += indexed_indirect_addr(&addr, memory, mpos);
      cycles = 6;
      goto lax;
    case 0xb3:
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 5;
      goto lax;
    case 0xa7:
      consumed += zeropage_addr(&addr, memory, mpos);
      cycles = 3;
      goto lax;
    case 0xb7:
      consumed += zeropage_y_addr(&addr, memory, mpos);
      cycles = 4;
      goto lax;
    case 0xab:
      consumed += immediate_addr(&addr, memory, mpos);
      cycles = 2;
      goto lax;
    case 0xaf:
      consumed += absolute_addr(&addr, memory, mpos);
      cycles = 4;
      goto lax;
    case 0xbf:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      lax:
        cmd->type = LAX_ASM;
        cmd->handler = load_acc_then_x;
        break;
    // ANC
    case 0x0b:
    case 0x2b:
      consumed += immediate_addr(&addr, memory, mpos);
      cmd->type = ANC_ASM;
      cmd->handler = bitwise_and_with_carry;
      break;
    // ALR
    case 0x4b:
      consumed += immediate_addr(&addr, memory, mpos);
      cmd->type = ALR_ASM;
      cmd->handler = bitwise_and_then_shift_r;
      break;
    // ARR
    case 0x6b:
      consumed += immediate_addr(&addr, memory, mpos);
      cmd->type = ARR_ASM;
      cmd->handler = bitwise_and_then_rotate_r;
      break;
    // XAA
    case 0x8b:
      consumed += immediate_addr(&addr, memory, mpos);
      cmd->type = XAA_ASM;
      cmd->handler = transfer_x2a_then_and;
      break;
    // AXS
    case 0xcb:
      consumed += immediate_addr(&addr, memory, mpos);
      cmd->type = AXS_ASM;
      cmd->handler = bitwise_and_with_x_then_sbc;
      break; 
    // AHX
    case 0x93:
      // FIXME: high byte in this addressing
      consumed += indirect_indexed_addr(&addr, memory, mpos);
      cycles = 6;
      cmd->type = AHX_ASM;
      cmd->handler = high_byte_ahx;
      break;
    case 0x9f:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 5;
      cmd->type = AHX_ASM;
      cmd->handler = high_byte_ahx;
      break;
    // SHY
    case 0x9c:
      consumed += absolute_x_addr(&addr, memory, mpos);
      cycles = 5;
      cmd->type = SHY_ASM;
      cmd->handler = high_byte_shy;
      break;
    // SHX
    case 0x9e:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 5;
      cmd->type = SHX_ASM;
      cmd->handler = high_byte_shx;
      break; 
    // TAS
    case 0x9b:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 5;
      cmd->type = TAS_ASM;
      cmd->handler = high_byte_tas;
      break; 
    // LAS
    case 0xbb:
      consumed += absolute_y_addr(&addr, memory, mpos);
      cycles = 4;
      cmd->type = LAS_ASM;
      cmd->handler = bitwise_and_with_sp_then_store;
      break;
    #endif
    default:
      debug_print("ERROR: unsupported opcode %x at %x\n", opcode, pos);
      return 0;
  }
  cmd->maddr = addr;
  cmd->cycles = cycles;

  return consumed;
}
