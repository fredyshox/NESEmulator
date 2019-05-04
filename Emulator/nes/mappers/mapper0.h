//
// iNES mapper 0
//
// prg non switchable
// chr non switchable
// vertical or horizontal mirroring
// possible prg ram
// no registers
//

#ifndef mapper0_h
#define mapper0_h

#include "mapper.h"
#include <stdlib.h>
#include <stdio.h>

mapper* mapper0_create(struct cartridge* cartridge);
void mapper0_read(struct mapper* m, uint16_t address, uint8_t* dest);

#endif /* mapper0_h */
