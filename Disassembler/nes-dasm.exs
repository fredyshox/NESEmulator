#!/usr/bin/env elixir
# *.nes file disassembler script
# Copyright (c) 2019 Kacper Rączy

dir = Path.dirname(__ENV__.file)
Code.require_file(Path.join(dir, "disassembler6502.ex"))

if length(System.argv) < 1 do
  IO.puts("Usage: nes-dasm <path_to_file>")
  exit(:shutdown)
end

[path | _args] = System.argv
<<header :: bytes-size(16), rest :: binary>> = File.read!(path)
<<_nes :: bytes-size(4), prgSize, _chrSize, _flags6, _flags7, _flags8, _flags9, _flags10, _padding::binary>> = header

prgSize = 16384 * prgSize
<<prgRom :: bytes-size(prgSize), rest :: binary>> = rest
bytearr = :array.from_list(:binary.bin_to_list(prgRom))
offset = 65536 - prgSize
xxxx = prgSize - 6
<<_x :: bytes-size(xxxx), word1 :: little-integer-size(16), word2 :: little-integer-size(16), word3 :: little-integer-size(16)>> = prgRom
IO.puts("NMI #{word1}")
IO.puts("RST #{word2}")
IO.puts("IRQ #{word3}")
asm_graph = Disassembler6502.disassemble(bytearr, offset, [word2 - offset, word1 - offset])
IO.inspect(Enum.sort(asm_graph))
IO.puts(map_size(asm_graph))
