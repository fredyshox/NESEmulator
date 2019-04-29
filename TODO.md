## TODO List
Emulator
instruction.h:
* move common operation code to some #defines
state.h:
all:
* test flags, store, stack, bitwise
* fix endianess issues
* incoming interrupt flag
* replace non-portable struct to bytes convertsion with unions
ppu:
* tests
* scrolling
* ppu loop
mappers:
* mmc0, mmc1, mmc3
* mapper interface
* few mappers
Disassebler
* some data section detection
