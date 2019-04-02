//
// ppu colors
// Copyright (c) 2019 Kacper Rączy
//

#ifndef ppu_color
#define ppu_color

#include <stdint.h>

struct ppu_color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

#define NES_COLOR(IDX) nes_color_pallete[IDX]
#define NES_COLOR_COUNT 0x40

extern const struct ppu_color nes_color_pallete[];

#endif /* ppu_color */
