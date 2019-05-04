//
// 6502 memory addressing
// Copyright (c) 2019 Kacper Rączy
//

#ifndef addr_h
#define addr_h

#include <stdint.h>
#include "6502/state.h"

#define IMM_ADDR  0x0
#define ZP_ADDR   0x1
#define ZPX_ADDR  0x2
#define ZPY_ADDR  0x3
#define REL_ADDR  0x4
#define ABS_ADDR  0x5
#define ABX_ADDR  0x6
#define ABY_ADDR  0x7
#define IND_ADDR  0x8
#define IZX_ADDR  0x9
#define IZY_ADDR  0xA
#define ACC_ADDR  0xB
#define IMP_ADDR  0xC

struct mem_addr {
  int type;
  union {
    uint16_t value;
    uint8_t lval;
    uint8_t hval;
  };
};

typedef struct mem_addr mem_addr;

// Addressing modes

int immediate_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int zeropage_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int zeropage_x_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int zeropage_y_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int relative_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int absolute_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int absolute_x_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int absolute_y_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int indirect_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int indexed_indirect_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);
int indirect_indexed_addr(struct mem_addr *maddr, struct memory6502* memory, uint16_t pos);

// Handler

uint16_t handle_addr(struct state6502 *state, struct mem_addr maddr);

#endif /* addr_h */
