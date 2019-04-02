#!/usr/bin/env elixir
# *.nes file examiner
# Copyright (c) 2019 Kacper Rączy

use Bitwise

pretty_puts =
  fn (lstr, rstr) ->
    IO.puts(String.pad_trailing(lstr, 25) <> rstr)
  end

if length(System.argv) < 1 do
  IO.puts("Usage: nes-dasm <path_to_file>")
  exit(:shutdown)
end

[path | _args] = System.argv
<<header :: bytes-size(16), _rest::binary>> = File.read!(path)
<<nes :: bytes-size(4), prgSize, chrSize, flags6, flags7, flags8, flags9, flags10, _padding::binary>> = header

if nes != <<"NES", 0x1a>> do
  IO.puts("Error: #{Path.basename(path)} is not iNES rom file.")
  exit(:shutdown)
end

pretty_puts.("File: ", "#{Path.basename(path)}")
pretty_puts.("PRG ROM size: ", "#{prgSize * 16} KB")
pretty_puts.("CHR ROM size: ", "#{chrSize * 8} KB")

<<mapperLow :: little-integer-size(4),
  fourScreenMode :: size(1),
  trainer :: size(1),
  ram :: size(1),
  mirroring :: size(1)>> = <<flags6>>

mapperMode =
  cond do
    fourScreenMode == 1 -> "four-screen"
    mirroring == 1 -> "vertical"
    true -> "horizontal"
  end

<<mapperHigh :: little-integer-size(4),
  format :: little-integer-size(2),
  consoleType :: little-integer-size(2)>> = <<flags7>>

consoleTypeStr =
  case consoleType do
    3 -> "Extended Console Type"
    2 -> "Nintendo Playchoice 10"
    1 -> "Nintendo VS. Unisystem"
    _ -> "NES/Famicom"
  end

pretty_puts.("Mapper number: ", "#{bor(mapperLow, mapperHigh <<< 4)}")
pretty_puts.("Mirroring mode: ", "#{mapperMode}")
pretty_puts.("Trainer (512-byte): ", "#{if trainer == 1, do: "present", else: "not present"}")
pretty_puts.("Cartidge RAM: ", "#{if ram == 1, do: "present", else: "not present"}")
pretty_puts.("iNES format: ", "#{if format == 2, do: "2.0", else: "1.0"}")
pretty_puts.("Console type: ", "#{consoleTypeStr}")

if format != 2 do
  # iNES 1.0 stuff
  <<prgRamSize :: little-integer-size(8)>> = <<flags8>>
  <<_reserved :: size(7),
    tvSystem :: size(1)>> = <<flags9>>
  <<_ :: size(3),
    prgRamPresence :: size(1),
    _ :: size(2),
    tvSystemComp :: little-integer-size(2)>> = <<flags10>>

  tvSystemStr =
    cond do
      tvSystemComp == 1 or tvSystemComp == 3 ->
        "NTSC and PAL"
      tvSystem == 1 -> "PAL"
      true -> "NTSC"
    end

  pretty_puts.("TV system: ", "#{tvSystemStr}")
  if ram == 1 do
    pretty_puts.("PRG RAM: ", "#{if prgRamPresence == 1, do: "#{prgRamSize * 8} KB", else: "not present"}")
  end
end
