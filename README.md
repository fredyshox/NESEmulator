# NESEmulator

Nintendo Entertainment System emulator (still under development).

Always wanted to write some console emulator, so here it is.

Works:
- 6502 cpu emulation (no interrupts, no decimal mode)
- nes rom disassembler [nes-dasm.exs](./Disassembler/nes-dasm.exs)
- nes rom examiner: [rom-info.exs](./Disassembler/rom-info.exs)

In progress:
- PPU emulation
- memory mappers
- cocoa driver

TODO:
- threading
- cocoa touch driver
- APU emulation
