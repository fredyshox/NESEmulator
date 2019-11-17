#!/usr/bin/env elixir
# *.nes file examiner
# Copyright (c) 2019 Kacper Rączy

use Bitwise

pretty_puts =
  fn (lstr, rstr) ->
    IO.puts(String.pad_trailing(lstr, 25) <> rstr)
  end

if length(System.argv) < 1 do
  IO.puts("Usage: rom-info <path_to_file>")
  exit(:shutdown)
end

[path | _args] = System.argv
<<header :: bytes-size(16), _rest::binary>> = File.read!(path)
<<nes :: bytes-size(4), prgSize, chrSize, flags6, flags7, flags8, flags9, flags10, rest::binary>> = header

if nes != <<"NES", 0x1a>> do
  IO.puts("Error: #{Path.basename(path)} is not iNES rom file.")
  exit(:shutdown)
end

pretty_puts.("File: ", "#{Path.basename(path)}")
pretty_puts.("PRG ROM size: ", "#{prgSize * 16} KB")
pretty_puts.("CHR ROM size: ", "#{chrSize * 8} KB")
if chrSize == 0 do 
  pretty_puts.("CHR RAM size: ", "8 KB")
end 

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

pretty_puts.("Mirroring mode: ", "#{mapperMode}")
pretty_puts.("Trainer (512-byte): ", "#{if trainer == 1, do: "present", else: "not present"}")
pretty_puts.("Non-volatile memory: ", "#{if ram == 1, do: "present", else: "not present"}")
pretty_puts.("Console type: ", "#{consoleTypeStr}")

uint12to15 = band(:binary.decode_unsigned(rest), 0xffffffff)
cond do
  format == 2 ->
    pretty_puts.("iNES format: ", "2.0") 
    # <<submapper :: size(4), mapperUpper2 :: size(4)>> = <<flags8>>
  format == 0 and uint12to15 == 0 -> 
    <<prgRamSize :: size(8)>> = <<flags8>>
    pretty_puts.("iNES format: ", "1.0")
    pretty_puts.("Mapper number: ", "#{bor(mapperLow, mapperHigh <<< 4)}")
    if ram == 1 do
      pretty_puts.("PRG RAM: ", "#{if prgRamSize == 0, do: "8 KB", else: "#{prgRamSize * 8} KB"}")
    end
    <<_reserved :: size(7),
      tvSystem :: size(1)>> = <<flags9>> 
    <<_ :: size(3),
      _prgRamPresence :: size(1),
      _ :: size(2),
      tvSystemComp :: size(2)>> = <<flags10>> 
    tvSystemStr =
    cond do
      tvSystemComp == 1 or tvSystemComp == 3 ->
        "NTSC and PAL"
      tvSystem == 1 -> "PAL"
      true -> "NTSC"
    end
    pretty_puts.("TV system: ", "#{tvSystemStr}") 
  true ->
    pretty_puts.("iNES format: ", "Archaic")
end
