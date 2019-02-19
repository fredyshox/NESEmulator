//
// 6502 memory addressing
// Copyright (c) 2019 Kacper Rączy
//

#include <stdint.h>
#include "state.h"

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

// Helpers

static int full_value(struct mem_addr *maddr, uint8_t *code);
static int lsb_value(struct mem_addr *maddr, uint8_t *code);

// Addressing modes

int immediate_addr(struct mem_addr *maddr, uint8_t *code);
int zeropage_addr(struct mem_addr *maddr, uint8_t *code);
int zeropage_x_addr(struct mem_addr *maddr, uint8_t *code);
int zeropage_y_addr(struct mem_addr *maddr, uint8_t *code);
int relative_addr(struct mem_addr *maddr, uint8_t *code);
int absolute_addr(struct mem_addr *maddr, uint8_t *code);
int absolute_x_addr(struct mem_addr *maddr, uint8_t *code);
int absolute_y_addr(struct mem_addr *maddr, uint8_t *code);
int indirect_addr(struct mem_addr *maddr, uint8_t *code);
int indexed_indirect_addr(struct mem_addr *maddr, uint8_t *code);
int indirect_indexed_addr(struct mem_addr *maddr, uint8_t *code);

// Handler

uint16_t handle_addr(struct state6502 *state, struct mem_addr maddr);
