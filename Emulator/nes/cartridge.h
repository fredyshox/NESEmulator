//
// nes cartridge
// Copyright (c) 2019 Kacper Rączy
//

#ifndef cartridge_h
#define cartridge_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ppu/ppu.h"

#define TV_SYSTEM_NTSC 0
#define TV_SYSTEM_PAL  1
#define TV_SYSTEM_BOTH 2

#define INES_HEADER_SIZE 16
#define INES_TRAINER_BLOCK 512
#define INES_PRG_BLOCK 16384
#define INES_CHR_BLOCK 8192

#define CARTRIDGE_PRGRAM_SIZE 8192

struct cartridge {
    int prg_rom_size;
    uint8_t* prg_rom;
    int chr_rom_size;
    uint8_t* chr_rom;
    bool chr_ram_mode;
    uint8_t* prg_ram;
    int prg_ram_size;
    ppu_mirroring mirroring_type;
    int mapper;
    int tv_system;
};

typedef struct cartridge cartridge;

int cartridge_from_file(struct cartridge* c, char* path);
void cartridge_free(struct cartridge* c);

#endif /* cartridge_h */
