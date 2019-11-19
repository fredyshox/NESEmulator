/**
* C playground to test some features.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*
 Testing bitfield portability
 things like layout, order
*/

char* bin(uint8_t byte, int size) {
  char* str = malloc((size + 1) * sizeof(char));
  for (int i = 0; i < size; i++) {
    if ((byte & (0x01 << (size - 1 - i))) != 0)
      str[i] = '1';
    else
      str[i] = '0';
  }
  str[size] = '\0';
  return str;
}

char* bin16(uint16_t byte, int size) {
  char* str = malloc((size + 1) * sizeof(char));
  for (int i = 0; i < size; i++) {
    if ((byte & (0x01 << (size - 1 - i))) != 0)
      str[i] = '1';
    else
      str[i] = '0';
  }
  str[size] = '\0';
  return str;
}

struct bit_fields {
  uint8_t bit1 : 1;
  uint8_t bits2 : 2;
  uint8_t bits3 : 3;
  uint8_t bit4 : 1;
  uint8_t bit5 : 1;
};

union bits {
  struct {
    uint8_t a : 5;
    uint8_t b : 2;
    uint8_t c : 1;
  };
  uint8_t byte;
};

union ppu_internal_register {
  uint16_t address;
  struct {
    uint16_t x_scroll : 5;
    uint16_t y_scroll : 5;
    uint16_t nametable_addr : 2;
    uint16_t _unused : 4;
  };
};

/*
 Layout
 54333221
*/

int main() {
  union bits bb;
  bb.byte = 0xff;
  printf("a %s\n", bin(bb.a, 5));
  printf("b %s\n", bin(bb.b, 2));
  printf("c %s\n", bin(bb.c, 1));
  printf("byte: %s\n", bin(bb.byte, 8));
  bb.a += 1;
  printf("a %s\n", bin(bb.a, 5));
  printf("b %s\n", bin(bb.b, 2));
  printf("c %s\n", bin(bb.c, 1));
  printf("byte: %s\n", bin(bb.byte, 8));
  bb.c += 1;
  printf("a %s\n", bin(bb.a, 5));
  printf("b %s\n", bin(bb.b, 2));
  printf("c %s\n", bin(bb.c, 1));
  printf("byte: %s\n", bin(bb.byte, 8));

  clock_t t = clock();
  for (int i = 0; i < 10000000; i++) {
    bb.a += 1;
  }
  t = clock() - t;
  double time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Taken %lf\n", time_taken);

  uint8_t val = 0;
  t = clock();
  for (int i = 0; i < 10000000; i++) {
    val = (val == 31) ? 0 : val + 1;
  }
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Taken %lf\n", time_taken); 

  val = 0;
  t = clock();
  for (int i = 0; i < 10000000; i++) {
    val = (val + 1) & 0x1f;
  } 
  t = clock() - t;
  time_taken = ((double)t)/CLOCKS_PER_SEC;
  printf("Taken %lf\n", time_taken);

  union ppu_internal_register x;
  x.x_scroll = 0b10101;
  x.y_scroll = 0b10101;
  x.nametable_addr = 0b11;
  printf("x: %s\n", bin16(x.x_scroll, 5));
  printf("x: %s\n", bin16(x.y_scroll, 5));
  printf("x: %s\n", bin16(x.nametable_addr, 2));
  printf("byte: %s\n", bin16(x.address, 16));

  uint16_t value = 0x10ff;
  uint8_t lval = 0xff;
  printf("%04x\n", (value & 0xff00) | (uint16_t) (uint8_t) (lval + 1));

  long long int ints[5] = {3,4,5,6,7};
  printf("1 %lld\n", *(ints));
  printf("2 %lld\n", *(ints + 1));

  return 0;
}
