//
// NES console emulator
//

#ifndef console_h
#define console_h

#include <stdint.h>
#include <stdio.h>
#include "6502/state.h"
#include "ppu/ppu.h"
#include "ppu/renderer.h"
#include "mapper.h"
#include "cartridge.h"

struct nes_t {
  state6502* cpu;
  ppu_state* ppu;
  ppu_render_handle* ppu_handle;
  mapper* mapper;
};

typedef struct nes_t nes_t;

int nes_create(nes_t* console);
void nes_free(nes_t* console);
int nes_load_rom(nes_t* console, struct cartridge* cartridge);
// memory handlers
uint8_t nes_cpu_memory_read(struct memory6502* memory, uint16_t addr);
void nes_cpu_memory_write(struct memory6502* memory, uint16_t addr, uint8_t byte);
uint8_t nes_ppu_memory_read(struct ppu_memory* memory, uint16_t addr);
void nes_ppu_memory_write(struct ppu_memory* memory, uint16_t addr, uint8_t byte);

#endif /* console_h */
