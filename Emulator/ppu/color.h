//
// ppu colors
// Copyright (c) 2019 Kacper Rączy
//

#ifndef ppu_color_h
#define ppu_color_h

#include <stdint.h>

struct ppu_color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

typedef struct ppu_color ppu_color;

#define NES_COLOR(IDX) nes_color_palette[IDX]
#define NES_COLOR_COUNT 0x40
#define NES_COLOR_TRANSPARENT(COLOR) (COLOR.red == 0x00 && COLOR.green == 0x00 && COLOR.blue == 0x00)

extern const struct ppu_color nes_color_palette[];

#endif /* ppu_color_h */
