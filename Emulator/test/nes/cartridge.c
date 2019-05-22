//
// Arithmetic operations tests
// Copyright (c) 2019 Kacper Rączy
//

#include "nes/console.h"
#include "test.h"

int main() {
  nes_t emulator;
  nes_create(&emulator);
  cartridge c;
  char* cFile = "../../../Donkey Kong Classics (U).nes";
  bool res = true;

  ASSERT_EQ(0, cartridge_from_file(&c, (char*) cFile), "Cartridge file reading", &res);
  if (!res) goto error;
  ASSERT_EQ(0, nes_load_rom(&emulator, &c), "Nes rom loading", &res);
  if (!res) goto error;

  return 0;
  error:
    return 1;
}
