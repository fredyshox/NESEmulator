//
// iNES mapper 2
//
// prg switchable + fixed
// chr non switchable
// vertical or horizontal mirroring, non switchable
// no prg ram
// 1 register
//

#ifndef mapper2_h
#define mapper2_h

#include "nes/mapper.h"
#include <stdlib.h>
#include <stdio.h>

struct mapper* mapper2_create(struct cartridge* cartridge);
void mapper2_read(struct mapper* m, uint16_t address, uint8_t* dest);
void mapper2_write(struct mapper* m, uint16_t address, uint8_t byte);

#endif /* mapper2_h */
