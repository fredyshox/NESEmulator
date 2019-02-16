# 6502 disassebler
# Copyright (c) 2019 Kacper Rączy

defmodule Disassebler6502 do

  defp to_hex(value, bytewidth \\ 2) do
    hexstr = :io_lib.format("~#{bytewidth}.16.0B", [value])
    to_string(hexstr)
  end

  def disasseble(<<>>), do: :ok
  def disasseble(bytes) do
    {asm, rest} = disassebleSingle(bytes)
    IO.puts(asm)
    disasseble(rest)
  end

  defp disassebleSingle(bytes) when byte_size(bytes) > 0 do
    <<opcode::size(8), rest::binary>> = bytes
    asm =
      case opcode do
        0x00 ->
          "BRK"
        0x01 ->
          <<addr::size(8), rest::binary>> = rest
          "ORA    ($#{to_hex(addr)}, X)"
        0x05 ->
          <<addr::size(8), rest::binary>> = rest
          "ORA    $#{to_hex(addr)}"
        0x06 ->
          <<addr::size(8), rest::binary>> = rest
          "ASL    $#{to_hex(addr)}"
        0x08 ->
          "PHP"
        0x09 ->
          <<addr::size(8), rest::binary>> = rest
          "ORA    #$#{to_hex(addr)}"
        0x0a ->
          "ASL    A"
        0x0d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ORA    $#{to_hex(addr16, 4)}"
        0x0e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ASL    $#{to_hex(addr16, 4)}"
        0x10 ->
          <<addr::size(8), rest::binary>> = rest
          "BPL    $#{to_hex(addr)}"
        0x11 ->
          <<addr::size(8), rest::binary>> = rest
          "ORA    ($#{to_hex(addr)}), Y"
        0x15 ->
          <<addr::size(8), rest::binary>> = rest
          "ORA    $#{to_hex(addr)}, X"
        0x16 ->
          <<addr::size(8), rest::binary>> = rest
          "ASL    $#{to_hex(addr)}, X"
        0x18 ->
          "CLC"
        0x19 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ORA    $#{to_hex(addr16, 4)}, Y"
        0x1d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ORA    $#{to_hex(addr16, 4)}, X"
        0x1e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ASL    $#{to_hex(addr16, 4)}, X"
        0x20 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "JSR    $#{to_hex(addr16, 4)}"
        0x21 ->
          <<addr::size(8), rest::binary>> = rest
          "AND    ($#{to_hex(addr)}, X)"
        0x24 ->
          <<addr::size(8), rest::binary>> = rest
          "BIT    $#{to_hex(addr)}"
        0x25 ->
          <<addr::size(8), rest::binary>> = rest
          "AND    $#{to_hex(addr)}"
        0x26 ->
          <<addr::size(8), rest::binary>> = rest
          "ROL    $#{to_hex(addr)}"
        0x28 ->
          "PLP"
        0x29 ->
          <<val::size(8), rest::binary>> = rest
          "AND    #$#{to_hex(val)}"
        0x2a ->
          "ROL    A"
        0x2c ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "BIT    $#{to_hex(addr16, 4)}"
        0x2d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "AND    $#{to_hex(addr16, 4)}"
        0x2e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ROL    $#{to_hex(addr16, 4)}"
        0x30 ->
          <<addr::size(8), rest::binary>> = rest
          "BMI    $#{to_hex(addr)}"
        0x31 ->
          <<addr::size(8), rest::binary>> = rest
          "AND    ($#{to_hex(addr)}), Y"
        0x35 ->
          <<addr::size(8), rest::binary>> = rest
          "AND    $#{to_hex(addr)}, X"
        0x36 ->
          <<addr::size(8), rest::binary>> = rest
          "ROL    $#{to_hex(addr)}, X"
        0x38 ->
          "SEC"
        0x39 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "AND    $#{to_hex(addr16, 4)}, Y"
        0x3d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "AND    $#{to_hex(addr16, 4)}, X"
        0x3e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ROL    $#{to_hex(addr16, 4)}, X"
        0x40 ->
          "RTI"
        0x41 ->
          <<addr::size(8), rest::binary>> = rest
          "EOR    ($#{to_hex(addr)}, X)"
        0x45 ->
          <<addr::size(8), rest::binary>> = rest
          "EOR    $#{to_hex(addr)}"
        0x46 ->
          <<addr::size(8), rest::binary>> = rest
          "LSR    $#{to_hex(addr)}"
        0x48 ->
          <<addr::size(8), rest::binary>> = rest
          "PHA"
        0x49 ->
          <<addr::size(8), rest::binary>> = rest
          "EOR    #$#{to_hex(addr)}"
        0x4a ->
          "LSR    A"
        0x4c ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "JMP    $#{to_hex(addr16, 4)}"
        0x4d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "EOR    $#{to_hex(addr16, 4)}"
        0x4e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LSR    $#{to_hex(addr16, 4)}"
        0x50 ->
          <<addr::size(8), rest::binary>> = rest
          "BVC    $#{to_hex(addr)}"
        0x51 ->
          <<addr::size(8), rest::binary>> = rest
          "EOR    ($#{to_hex(addr)}), Y"
        0x55 ->
          <<addr::size(8), rest::binary>> = rest
          "EOR    $#{to_hex(addr)}, X"
        0x56 ->
          <<addr::size(8), rest::binary>> = rest
          "LSR    $#{to_hex(addr)}, X"
        0x58 ->
          "CLI"
        0x59 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "EOR    $#{to_hex(addr16, 4)}, Y"
        0x5d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "EOR    $#{to_hex(addr16, 4)}, X"
        0x5e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LSR    $#{to_hex(addr16, 4)}, X"
        0x60 ->
          "RTS"
        0x61 ->
          <<addr::size(8), rest::binary>> = rest
          "ADC    ($#{to_hex(addr)}, X)"
        0x65 ->
          <<addr::size(8), rest::binary>> = rest
          "ADC    $#{to_hex(addr)}"
        0x66 ->
          <<addr::size(8), rest::binary>> = rest
          "ROR    $#{to_hex(addr)}"
        0x68 ->
          "PLA"
        0x69 ->
          <<addr::size(8), rest::binary>> = rest
          "ADC    #$#{to_hex(addr)}"
        0x6a ->
          "ROR    A"
        0x6c ->
          <<addr::size(8), rest::binary>> = rest
          "JML    $#{to_hex(addr)}"
        0x6d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ADC    $#{to_hex(addr16, 4)}"
        0x6e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ROR    $#{to_hex(addr16, 4)}, X"
        0x70 ->
          <<addr::size(8), rest::binary>> = rest
          "BVS    $#{to_hex(addr)}"
        0x71 ->
          <<addr::size(8), rest::binary>> = rest
          "ADC    ($#{to_hex(addr)}), Y"
        0x75 ->
          <<addr::size(8), rest::binary>> = rest
          "ADC    $#{to_hex(addr)}, X"
        0x76 ->
          <<addr::size(8), rest::binary>> = rest
          "ROR    $#{to_hex(addr)}, X"
        0x78 ->
          "SEI"
        0x79 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ADC    $#{to_hex(addr16, 4)}, Y"
        0x7d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ADC    $#{to_hex(addr16, 4)}, X"
        0x7e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "ROR    $#{to_hex(addr16, 4)}"
        0x81 ->
          <<addr::size(8), rest::binary>> = rest
          "STA    ($#{to_hex(addr)}, X)"
        0x84 ->
          <<addr::size(8), rest::binary>> = rest
          "STY    $#{to_hex(addr)}"
        0x85 ->
          <<addr::size(8), rest::binary>> = rest
          "STA    $#{to_hex(addr)}"
        0x86 ->
          <<addr::size(8), rest::binary>> = rest
          "STX    $#{to_hex(addr)}"
        0x88 ->
          "DEY"
        0x8a ->
          "TXA"
        0x8c ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "STY    $#{to_hex(addr16, 4)}"
        0x8d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "STA    $#{to_hex(addr16, 4)}"
        0x8e ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "STX    $#{to_hex(addr16, 4)}"
        0x90 ->
          <<addr::size(8), rest::binary>> = rest
          "BCC    $#{to_hex(addr)}"
        0x91 ->
          <<addr::size(8), rest::binary>> = rest
          "STA    ($#{to_hex(addr)}), Y"
        0x94 ->
          <<addr::size(8), rest::binary>> = rest
          "STY    $#{to_hex(addr)}, X"
        0x95 ->
          <<addr::size(8), rest::binary>> = rest
          "STA    $#{to_hex(addr)}, X"
        0x96 ->
          <<addr::size(8), rest::binary>> = rest
          "STX    $#{to_hex(addr)}, Y"
        0x98 ->
          "TYA"
        0x99 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "STA    ($#{to_hex(addr16, 4)}), Y"
        0x9a ->
          "TXS"
        0x9d ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "STA    ($#{to_hex(addr16, 4)}), X"
        0xa0 ->
          <<val::size(8), rest::binary>> = rest
          "LDY    #$#{to_hex(val)}"
        0xa1 ->
          <<addr::size(8), rest::binary>> = rest
          "LDA    ($#{to_hex(addr)}, X)"
        0xa2 ->
          <<val::size(8), rest::binary>> = rest
          "LDX    #$#{to_hex(val)}"
        0xa4 ->
          <<addr::size(8), rest::binary>> = rest
          "LDY    $#{to_hex(addr)}"
        0xa5 ->
          <<addr::size(8), rest::binary>> = rest
          "LDA    $#{to_hex(addr)}"
        0xa6 ->
          <<addr::size(8), rest::binary>> = rest
          "LDX    $#{to_hex(addr)}"
        0xa8 ->
          "TAY"
        0xa9 ->
          <<val::size(8), rest::binary>> = rest
          "LDA    #$#{to_hex(val)}"
        0xaa ->
          "TAX"
        0xac ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LDY    $#{to_hex(addr16, 4)}"
        0xad ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LDA    $#{to_hex(addr16, 4)}"
        0xae ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LDX    $#{to_hex(addr16, 4)}"
        0xb0 ->
          <<addr::size(8), rest::binary>> = rest
          "BCS    $#{to_hex(addr)}"
        0xb1 ->
          <<addr::size(8), rest::binary>> = rest
          "LDA    ($#{to_hex(addr)}), Y"
        0xb4 ->
          <<addr::size(8), rest::binary>> = rest
          "LDY    $#{to_hex(addr)}, X"
        0xb5 ->
          <<addr::size(8), rest::binary>> = rest
          "LDA    $#{to_hex(addr)}, X"
        0xb6 ->
          <<addr::size(8), rest::binary>> = rest
          "LDX    $#{to_hex(addr)}, Y"
        0xb8 ->
          "CLV"
        0xb9 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LDA    $#{to_hex(addr16, 4)}, Y"
        0xba ->
          "TSX"
        0xbc ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LDY    $#{to_hex(addr16, 4)}, X"
        0xbd ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LDA    $#{to_hex(addr16, 4)}, X"
        0xbe ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "LDX    $#{to_hex(addr16, 4)}, Y"
        0xc0 ->
          <<val::size(8), rest::binary>> = rest
          "CPY    #$#{to_hex(val)}"
        0xc1 ->
          <<addr::size(8), rest::binary>> = rest
          "CMP    ($#{to_hex(addr)}, X)"
        0xc4 ->
          <<addr::size(8), rest::binary>> = rest
          "CPY    $#{to_hex(addr)}"
        0xc5 ->
          <<addr::size(8), rest::binary>> = rest
          "CMP    $#{to_hex(addr)}"
        0xc6 ->
          <<addr::size(8), rest::binary>> = rest
          "DEC    $#{to_hex(addr)}"
        0xc8 ->
          "INY"
        0xc9 ->
          <<val::size(8), rest::binary>> = rest
          "CMP    #$#{to_hex(val)}"
        0xca ->
          "DEX"
        0xcc ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "CPY    $#{to_hex(addr16, 4)}"
        0xcd ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "CMP    $#{to_hex(addr16, 4)}"
        0xce ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "DEC    $#{to_hex(addr16, 4)}"
        0xd0 ->
          <<addr::size(8), rest::binary>> = rest
          "BNE    $#{to_hex(addr)}"
        0xd1 ->
          <<addr::size(8), rest::binary>> = rest
          "CMP    ($#{to_hex(addr)}), Y"
        0xd5 ->
          <<addr::size(8), rest::binary>> = rest
          "CMP    $#{to_hex(addr)}, X"
        0xd6 ->
          <<addr::size(8), rest::binary>> = rest
          "DEC    $#{to_hex(addr)}, X"
        0xd8 ->
          "CLD"
        0xd9 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "CMP    $#{to_hex(addr16, 4)}, Y"
        0xdd ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "CMP    $#{to_hex(addr16, 4)}, X"
        0xde ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "DEC    $#{to_hex(addr16, 4)}, X"
        0xe0 ->
          <<val::size(8), rest::binary>> = rest
          "CPX    #$#{to_hex(val)}"
        0xe1 ->
          <<addr::size(8), rest::binary>> = rest
          "SBC    ($#{to_hex(addr)}, X)"
        0xe4 ->
          <<addr::size(8), rest::binary>> = rest
          "CPX    $#{to_hex(addr)}"
        0xe5 ->
          <<addr::size(8), rest::binary>> = rest
          "SBC    $#{to_hex(addr)}"
        0xe6 ->
          <<addr::size(8), rest::binary>> = rest
          "INC    $#{to_hex(addr)}"
        0xe8 ->
          "INX"
        0xe9 ->
          <<val::size(8), rest::binary>> = rest
          "SBC    #$#{to_hex(val)}"
        0xea ->
          "NOP"
        0xec ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "CPX    $#{to_hex(addr16, 4)}"
        0xed ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "SBC    $#{to_hex(addr16, 4)}"
        0xee ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "INC    $#{to_hex(addr16, 4)}"
        0xf0 ->
          <<addr::size(8), rest::binary>> = rest
          "BEQ    $#{to_hex(addr)}"
        0xf1 ->
          <<addr::size(8), rest::binary>> = rest
          "SBC    ($#{to_hex(addr)}), Y"
        0xf5 ->
          <<addr::size(8), rest::binary>> = rest
          "SBC    $#{to_hex(addr)}, X"
        0xf6 ->
          <<addr::size(8), rest::binary>> = rest
          "INC    $#{to_hex(addr)}, X"
        0xf8 ->
          "SED"
        0xf9 ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "SBC    $#{to_hex(addr16, 4)}, Y"
        0xfd ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "SBC    $#{to_hex(addr16, 4)}, X"
        0xfe ->
          <<addr16::little-integer-size(16), rest::binary>> = rest
          "INC    $#{to_hex(addr16, 4)}, X"
        _ ->
          # Unknown operation (could be data or illegal operation)
          "UNK"
      end
      {asm, rest}
  end

end
