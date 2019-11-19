//
// NES console emulator
//

#ifndef console_h
#define console_h

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "6502/state.h"
#include "6502/parser.h"
#include "ppu/ppu.h"
#include "ppu/renderer.h"
#include "nes/mapper.h"
#include "nes/cartridge.h"
#include "nes/controller.h"

#define NES_CPU_FREQ 1.79E6

struct nes_t {
  state6502* cpu;
  ppu_state* ppu;
  ppu_render_handle* ppu_handle;
  mapper* mapper;
  cartridge* cartridge;
  controller_t controller1;
  controller_t controller2;
};

typedef struct nes_t nes_t;

int nes_create(nes_t* console);
void nes_free(nes_t* console);
int nes_load_rom(nes_t* console, struct cartridge* cartridge);
bool nes_is_loaded(nes_t* console);
void nes_reset(nes_t* console);

/**
 * Performs single nes step by executing single cpu instruction
 * and appropriate amount of ppu cycles. 
 * Error flag values:
 * 0 - no error
 * 1 - cpu error
 * 2 - ppu error
 */
int nes_step(nes_t* console, int* error);
void nes_step_time(nes_t* console, double seconds, int* error);
void nes_mapper_step(nes_t* console);
// memory handlers
uint8_t nes_cpu_memory_read(struct memory6502* memory, uint16_t addr);
void nes_cpu_memory_write(struct memory6502* memory, uint16_t addr, uint8_t byte);
uint8_t nes_ppu_memory_read(struct ppu_memory* memory, uint16_t addr);
void nes_ppu_memory_write(struct ppu_memory* memory, uint16_t addr, uint8_t byte);

#endif /* console_h */
