//
// iNES mapper 1
//
// prg switchable
// chr switchable
// vertical, horizontal or single-screen mirroring
// possible prg ram
//

#ifndef mapper1_h
#define mapper1_h

#include "nes/mapper.h"
#include <stdlib.h>
#include <stdio.h>

struct mapper* mapper1_create(struct cartridge* cartridge);
void mapper1_read(struct mapper* m, uint16_t address, uint8_t* dest);
void mapper1_write(struct mapper* m, uint16_t address, uint8_t byte);

#endif /* mapper1_h */
