/**
* C playground to test some features.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

union ppu_mask {
  uint8_t byte;
  struct {
    uint8_t grayscale : 1;
    uint8_t bg_l8 : 1;
    uint8_t spr_l8 : 1;
    uint8_t show_bg : 1;
    uint8_t show_spr : 1;
    uint8_t emp_red : 1;
    uint8_t emp_green : 1;
    uint8_t emp_blue : 1;
  };
};

struct ppu_mask2 {
  bool grayscale;
  bool bg_l8;
  bool spr_l8;
  bool show_bg;
  bool show_spr;
  bool emp_red;
  bool emp_green;
  bool emp_blue;
};

void ppu_mask_update(union ppu_mask* mask, uint8_t byte) {
  mask->byte = byte;
}

void ppu_mask2_update(struct ppu_mask2* mask, uint8_t byte) {
  mask->grayscale = (byte & (0x01));
  mask->bg_l8     = (byte & (0x01 << 1));
  mask->spr_l8    = (byte & (0x01 << 2));
  mask->show_bg   = (byte & (0x01 << 3));
  mask->show_spr  = (byte & (0x01 << 4));
  mask->emp_red   = (byte & (0x01 << 5));
  mask->emp_green = (byte & (0x01 << 6));
  mask->emp_blue  = (byte & (0x01 << 7));
}

int main() {
  clock_t t;
  t = clock();
  union ppu_mask x;
  memset(&x, 0b11110000, 1);
  uint8_t b;
  int val;
  for (int i = 0; i < 1000000; i++) {
    // b = x.grayscale;
    // val += b;
    // b = x.bg_l8;
    // val += b;
    // b = x.spr_l8;
    // val += b;
    // b = x.show_bg;
    // val += b;
    // b = x.show_spr;
    // val += b;
    // b = x.emp_red;
    // val += b;
    // b = x.emp_green;
    // val += b;
    // b = x.emp_blue;
    // val += b;
    ppu_mask_update(&x, (uint8_t) i);
  }
  t = clock() - t;
  double time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Taken %f %d %lu\n", time_taken, val, sizeof(x));

  t = clock();
  struct ppu_mask2 y;
  for (int i = 0; i < 1000000; i++) {
    // b = y.grayscale;
    // val += b;
    // b = y.bg_l8;
    // val += b;
    // b = y.spr_l8;
    // val += b;
    // b = y.show_bg;
    // val += b;
    // b = y.show_spr;
    // val += b;
    // b = y.emp_red;
    // val += b;
    // b = y.emp_green;
    // val += b;
    // b = y.emp_blue;
    // val += b;
    ppu_mask2_update(&y, (uint8_t) i);
  }
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Taken %f %d %lu\n", time_taken, val, sizeof(y));

  return 0;
}
