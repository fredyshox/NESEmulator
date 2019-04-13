/**
* C playground to test some features.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

struct bit_fields {
  uint8_t bit1 : 1;
  uint8_t bits2 : 2;
  uint8_t bits3 : 3;
  uint8_t bit4 : 1;
  uint8_t bit5 : 1;
};

/*
 Layout
 54333221
*/

int main() {
  struct bit_fields bf;
  memset(&bf, 0b11110000, 1);
  printf("Size: %lu \n", sizeof(bf));
  uint8_t* ptr = (uint8_t *) &bf;
  printf("bit1: %s\n", bin(bf.bit1, 1));
  printf("bit2: %s\n", bin(bf.bits2, 2));
  printf("bit3: %s\n", bin(bf.bits3, 3));
  printf("bit4: %s\n", bin(bf.bit4, 1));
  printf("bit5: %s\n", bin(bf.bit5, 1));
  printf("Physical \n");
  for (int i = 0; i < 8; i++) {
    printf("Bit %d - %d \n", i, ((*ptr & (0x01 << i)) != 0));
  }

  return 0;
}
