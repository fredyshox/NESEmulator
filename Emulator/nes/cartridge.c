//
// nes cartridge
// Copyright (c) 2019 Kacper Rączy
//

#include "nes/cartridge.h"

int cartridge_from_file(struct cartridge* c, char* path) {
  uint8_t header[INES_HEADER_SIZE];
  uint8_t flags6, flags7, flags8, flags9, flags10;
  int prg_rom_size, chr_rom_size;
  uint8_t *prg_rom = NULL, *chr_rom = NULL;
  int err = 0;
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", path);
    return 1;
  }

  if (fread(header, sizeof(uint8_t), INES_HEADER_SIZE, file) != INES_HEADER_SIZE) {
    fprintf(stderr, "Cannot read iNES header\n");
    return 2;
  }

  if (strncmp("NES\x1a", (char*) header, 4) != 0) {
    fprintf(stderr, "Provided file is not in iNES format: %s\n", path);
    return 2;
  }

  prg_rom_size = (int) header[4] * INES_PRG_BLOCK;
  chr_rom_size = (int) header[5] * INES_CHR_BLOCK;
  flags6 = header[6];
  flags7 = header[7];
  flags8 = header[8];
  flags9 = header[9];
  flags10 = header[10];

  // header parsing
  ppu_mirroring mirroring;
  if (flags6 & 0x08) {
    mirroring = PPU_MIRRORING_FOUR_SCREEN;
  } else if (flags6 & 0x01) {
    mirroring = PPU_MIRRORING_VERTICAL;
  } else {
    mirroring = PPU_MIRRORING_HORIZONTAL;
  }
  bool trainer_presence = (flags6 & 0x04) != 0;
  int ines_version = ((flags7 & 0x0c) == 0x0c) ? 2 : 1;
  int mapper_no = (flags7 & 0xf0) | (flags6 >> 4);
  int tv_system = (flags9 & 0x01) ? TV_SYSTEM_PAL : TV_SYSTEM_NTSC;
  bool prg_ram_presence = (flags9 & 0x10) != 0;
  bool chr_ram_presence = chr_rom_size == 0; 
  int prg_ram_size = (int) flags8 * 8192;

  if (trainer_presence) {
    if (fseek(file, INES_TRAINER_BLOCK, 1)) {
      fprintf(stderr, "Cannot read trainer\n");
      err = 2; goto catch_error;
    }
  }

  if (prg_rom_size != 0) {
    prg_rom = malloc(sizeof(uint8_t) * prg_rom_size);
    if (fread(prg_rom, sizeof(uint8_t), prg_rom_size, file) != prg_rom_size) {
      fprintf(stderr, "Cannot read prg rom\n");
      err = 2; goto catch_error;
    }
  }

  if (chr_ram_presence) {
    chr_rom_size = INES_CHR_BLOCK;
    chr_rom = calloc(chr_rom_size, sizeof(uint8_t));
  } else if (chr_rom_size != 0) {
    chr_rom = malloc(sizeof(uint8_t) * chr_rom_size);
    if (fread(chr_rom, sizeof(uint8_t), chr_rom_size, file) != chr_rom_size) {
      fprintf(stderr, "Cannot read chr rom\n");
      err = 2; goto catch_error;
    } 
  }

  c->prg_rom = prg_rom;
  c->prg_rom_size = prg_rom_size;
  c->chr_rom = chr_rom;
  c->chr_rom_size = chr_rom_size;
  c->chr_ram_mode = chr_ram_presence;
  c->prg_ram = calloc(CARTRIDGE_PRGRAM_SIZE, sizeof(uint8_t));
  c->prg_ram_size = CARTRIDGE_PRGRAM_SIZE;
  c->mapper = mapper_no;
  c->mirroring_type = mirroring;
  c->tv_system = tv_system;

  fclose(file);
  return 0;
  catch_error:
    c->prg_rom = NULL;
    c->prg_ram = NULL;
    c->chr_rom = NULL;
    fclose(file);
    free(prg_rom);
    free(chr_rom);
    return err;
}

void cartridge_free(struct cartridge* c) {
  free(c->prg_rom);
  free(c->chr_rom);
  free(c->prg_ram);
}
