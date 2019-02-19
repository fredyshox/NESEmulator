//
// 6502 binary parser
// Copyright (c) 2019 Kacper Rączy
//

int execute_asm(state6502 *state) {
  // TOOD parse and execute instruction
}

// TODO: out of bounds error handling
int parse_asm(asm6502 *cmd, uint8_t *bytes, int bcount) {
  if (bcount < 1) {
    return 0;
  }

  uint8_t opcode = *bytes;
  int consumed = 1;
  mem_addr addr;
  switch (opcode) {
    // ADC - add with carry
    case 0x69:
      consumed += immediate_addr(&addr, bytes + 1);
      goto adc;
    case 0x6d:
      consumed += absolute_addr(&addr, bytes + 1);
      goto adc;
    case 0x7d:
      consumed += absolute_x_addr(&addr, bytes + 1);
      goto adc;
    case 0x79:
      consumed += absolute_y_addr(&addr, bytes + 1);
      goto adc;
    case 0x65:
      consumed += zeropage_addr(&addr, bytes + 1);
      goto adc;
    case 0x75:
      consumed += zeropage_x_addr(&addr, bytes + 1);
      goto adc;
    case 0x61:
      consumed += indexed_indirect_addr(&addr, bytes + 1);
      goto adc;
    case 0x71:
      consumed += indirect_indexed_addr(&addr, bytes + 1);
      adc:
        cmd->type = ADC_ASM;
        cmd->maddr = addr;
        break;
    // AND
    case 0x29:
      consumed += immediate_addr(&addr, bytes + 1);
      goto and;
    case 0x2d:
      consumed += absolute_addr(&addr, bytes + 1);
      goto and;
    case 0x3d:
      consumed += absolute_x_addr(&addr, bytes + 1);
      goto and;
    case 0x39:
      consumed += absolute_y_addr(&addr, bytes + 1);
      goto and;
    case 0x25:
      consumed += zeropage_addr(&addr, bytes + 1);
      goto and;
    case 0x35:
      consumed += zeropage_x_addr(&addr, bytes + 1);
      goto and;
    case 0x21:
      consumed += indexed_indirect_addr(&addr, bytes + 1);
      goto and;
    case 0x31:
      consumed += indirect_indexed_addr(&addr, bytes + 1);
      and:
        cmd->type = AND_ASM;
        cmd->maddr = addr;
        break;
    default:
      fprintf(stderr, "Error: unsupported opcode %x\n", opcode);
      consumed = 0;
      break;
  }

  return consumed;
}
