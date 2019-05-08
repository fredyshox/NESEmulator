#!/usr/bin/env elixir
# PRG Rom extractor

if length(System.argv) < 2 do
  IO.puts("Usage: nes-dasm <path_to_rom> <path_to_outfile>")
  exit(:shutdown)
end

[inPath | args] = System.argv
[outPath | _] = args
<<header :: bytes-size(16), rest :: binary>> = File.read!(inPath)
<<_nes :: bytes-size(4), prgSize, _chrSize, _flags6, _flags7, _flags8, _flags9, _flags10, _padding::binary>> = header
prgSize = 16384 * prgSize
<<prgRom :: bytes-size(prgSize), rest :: binary>> = rest

File.write!(outPath, prgRom, [:binary])
