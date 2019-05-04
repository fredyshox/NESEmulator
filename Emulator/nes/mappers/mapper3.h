//
// iNES Mapper 3
// CNROM
//
// Non switchable PRG bank with 16 or 32 KB size
// Switchable CHR bank with 8 KB size (max 4 banks)
// Mirroring: fixed vertical or horizontal
// One register (writes at $8000-$ffff)
//
// Copyright (c) 2019 Kacper Rączy
//

#ifndef mapper3_h
#define mapper3_h

#include "mapper.h"
#include "mapper0.h"
#include <stdlib.h>

mapper* mapper3_create(struct cartridge* cartridge);
void mapper3_write(struct mapper* m, uint16_t address, uint8_t byte);
void mapper3_read(struct mapper* m, uint16_t address, uint8_t* dest);

#endif /* mapper3_h */
