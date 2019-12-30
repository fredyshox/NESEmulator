# NESEmulator

Nintendo Entertainment System emulator (still under development).

Always wanted to write some console emulator, so here it is.

## Requirements

* CMake 3.7+ 
* clang/clang++
* Google Test 1.8.1+
* macOS 10.14+ (for Cocoa frontend)

## How to build

Just emulation library:
```
> mkdir Build/
> cd Build/
> cmake ../Emulator
> make
```

Emulation library and Cocoa frontend (only for macOS):
```
> cd Drivers/Cocoa
> ./build.sh
```
Then open `nescocoa.xcodeproj`, build and run.

## What Works
- 6502 cpu emulation (no decimal mode)
- PPU emulation
- iNES memory mappers: 0, 2, 3
- Cocoa frontend
- nes rom disassembler [nes-dasm.exs](./Disassembler/nes-dasm.exs)
- nes rom examiner: [rom-info.exs](./Disassembler/rom-info.exs)
- prg rom extractor: [extract-prg.exs](./Disassembler/extract-prg.exs)
