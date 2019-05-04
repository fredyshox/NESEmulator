//
// Abstract nes mapper
// Copyright (c) 2019 Kacper Rączy
//

#include "mapper.h"
#include <stdio.h>

void mapper_write(struct mapper* m, uint16_t address, uint8_t byte) {
  if (m->write != NULL)
    m->write(m, address, byte);
}

void mapper_read(struct mapper* m, uint16_t address, uint8_t* dest) {
  m->read(m, address, dest);
}

void mapper_free(struct mapper* m) {
  free(m->data);
  free(m);
}

struct mapper* mapper_from_id(struct cartridge* c, int id) {
  switch (id) {
    case 0:
      return mapper0_create(c);
    case 3:
      return mapper3_create(c);
    default:
      fprintf(stderr, "Error: Mapper %d is not supported!\n", id);
      return NULL;
  }
}
