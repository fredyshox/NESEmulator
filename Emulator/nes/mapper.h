//
// Abstract nes mapper
// Copyright (c) 2019 Kacper Rączy
//

#ifndef mapper_h
#define mapper_h

#include "nes/cartridge.h"
#include "common/debug.h"

/**
 * Abstract mapper struct
 * data field can be pointer to anything
 */
struct mapper {
  int id;
  struct cartridge* cartridge;
  void* data;
  void (*write)(struct mapper*, uint16_t, uint8_t);
  void (*read)(struct mapper*, uint16_t, uint8_t*);
};

typedef struct mapper mapper;

// registers
void mapper_write(struct mapper* m, uint16_t address, uint8_t byte);
void mapper_read(struct mapper* m, uint16_t address, uint8_t* dest);
void mapper_free(struct mapper* m);

// mapper factory
struct mapper* mapper_from_id(struct cartridge* c, int id);

#endif /* mapper_h */
