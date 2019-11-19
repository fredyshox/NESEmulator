//
// Struct definitions for representing cpu state
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/state.h"

// state6502

void state6502_create(struct state6502 *state, struct memory6502 *memory) {
  assert(memory != NULL && state != NULL);

  state->reg_a = 0x00;
  state->reg_x = 0x00;
  state->reg_y = 0x00;
  state->sp = 0xff;
  state->pc = 0x0000;
  state->status.byte = 0x34;
  state->memory = memory;
  state->incoming_int = NONE_INT;
  state->page_crossed = false;
}

void state6502_free(struct state6502* state) {
  memory6502_free(state->memory);
  free(state->memory);
}

void state6502_request_interrupt(struct state6502* state, enum interrupt6502 i) {
  state->incoming_int = i;
}

void interrupt6502_handle(struct state6502 *state) {
  enum interrupt6502 intnum = state->incoming_int;
  union flags6502 status = state->status;
  debug_print("Interrupt happend %d disable %d\n", intnum, state->status.int_disable);

  uint16_t vec_addr;
  // vars for pc addr
  uint8_t msb, lsb;

  if ((state->status.int_disable == 1 && intnum != NMI_INT) || intnum == NONE_INT)
    return;

  if (intnum == RST_INT) {
    vec_addr = STATE6502_RESET_VECTOR;
  } else {
    status.bflag5 = 1;
    if (intnum == IRQ_INT) {
      status.bflag4 = 0;
      vec_addr = STATE6502_IRQ_VECTOR;
    } else if (intnum == BRK_INT) {
      status.bflag4 = 1;
      vec_addr = STATE6502_IRQ_VECTOR;
    } else if (intnum == NMI_INT) {
      status.bflag4 = 0;
      vec_addr = STATE6502_NMI_VECTOR;
    }

    // TODO Use uint_16_to_8
    msb = (uint8_t) (state->pc >> 8); lsb = (uint8_t) (state->pc & 0xff);
    STATE6502_STACK_PUSH(state, &msb, 1);
    STATE6502_STACK_PUSH(state, &lsb, 1);
    STATE6502_STACK_PUSH(state, &status.byte, 1);
    state->status.int_disable = 1;
  }

  uint16_t vector = (uint16_t) memory6502_load(state->memory, vec_addr) | ((uint16_t) memory6502_load(state->memory, vec_addr + 1) << 8);
  state->pc = vector;
}

// memory6502

void memory6502_create(struct memory6502 *memory, uint16_t size) {
  uint8_t *buffer = malloc(size * sizeof(uint8_t));
  memory->mptr = buffer;
  memory->size = size;
  memory->io = NULL;
  memory->store_handler = NULL;
  memory->load_handler = NULL;
}

void memory6502_free(struct memory6502 *memory) {
  free(memory->mptr);
}

void memory6502_store(struct memory6502 *memory, uint16_t idx, uint8_t value) {
  if (memory->store_handler == NULL) {
    // default
    if (idx >= memory->size) {
      return;
    }

    memory->mptr[idx] = value;
    return;
  }

  memory->store_handler(memory, idx, value);
}

uint8_t memory6502_load(struct memory6502 *memory, uint16_t idx) {
  if (memory->load_handler == NULL) {
    // default
    if (idx >= memory->size) {
      return 0;
    }

    return memory->mptr[idx];
  }

  return memory->load_handler(memory, idx);
}
