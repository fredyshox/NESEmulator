#!/usr/bin/env elixir
# *.nes file disassebler script
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
Disassembler6502.disasseble(prgRom)
