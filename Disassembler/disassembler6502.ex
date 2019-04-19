# 6502 disassembler
# Copyright (c) 2019 Kacper Rączy

defmodule Disassembler6502 do
  
  defmodule Vertex do

    @enforce_keys [:instruction]
    defstruct [:label, :instruction, :edges, length: 1, args: ""] 

    defimpl String.Chars do
      def to_string(%Vertex{instruction: asm, args: args}) do
        "#{asm}    #{args}"
      end
    end

  end

  def write(asm_graph, device \\ :stdio) do
    asm_graph
      |> Enum.sort(&(&1 < &2))
      |> Enum.reduce({0, nil}, fn kv, acc ->
        {pointer, label} = acc
        write(kv, pointer, device, label)
      end)
  end

  defp write(kv = {position, vertex}, pointer, device, label) when pointer <= position do
    prefix = "    "
    if (pointer < position) do
      if (label != :data) do
        IO.puts(device, "section .data:")
      end
      IO.puts(device, "#{prefix}.byte $#{to_hex(0xff)}")
      write(kv, pointer + 1, device, :data)
    else
      new_label = 
        case vertex.label do
          nil -> label
          _   ->
            IO.puts(device, "#{vertex.label}:")
            :code
        end
      IO.puts(device, "#{prefix}#{vertex}")
      {pointer + vertex.length, new_label}
    end
  end

  def disassemble(arr, mem_offset \\ 0, positions \\ [0], label \\ "start", asm_graph \\ %{})
  def disassemble(_, _, [], _, asm_graph), do: asm_graph
  def disassemble(arr, mem_offset, [position | rest], label, asm_graph) do
    {status, {vertex, new_label?}} = find_vertex(arr, position, mem_offset, asm_graph)

    {new_edges, asm_graph} =
      case status do
        :new -> 
          if label != nil do
            vertex = %{vertex | :label => label}
          end
          {vertex.edges, Map.put(asm_graph, position, vertex)}
        :existing ->
          {[], asm_graph}
      end
  
    new_label =
      case new_label? do
        true -> "label#{position}" 
        false -> nil
      end
    
    disassemble(arr, mem_offset, rest ++ new_edges, new_label, asm_graph)
  end

  defp find_vertex(arr, position, mem_offset, asm_graph) do 
    if asm_graph[position] != nil do
      {:existing, {asm_graph[position], false}}
    else
      {:new, disassemble_single(arr, position, mem_offset)}
    end
  end

  defp to_hex(value, bytewidth \\ 2) do
    hexstr = :io_lib.format("~#{bytewidth}.16.0B", [value])
    to_string(hexstr)
  end

  defp get_bytes(_, _, 0), do: <<>>
  defp get_bytes(arr, position, count) when count > 0 do
    if position < 0 or position >= :array.size(arr) do
      raise "Position out of bounds: #{position}"
    end
    byte = :array.get(position, arr)

    <<byte>> <> get_bytes(arr, position + 1, count - 1)
  end

  defp disassemble_single(arr, pos, mem_offset) do
    <<opcode::size(8)>> = get_bytes(arr, pos, 1)
    pos = pos + 1
    {instruction, args, edges, consumed} = 
      case opcode do
        0x00 ->
          {"BRK", "", nil, 0}
        0x01 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ORA", "($#{to_hex(addr)}, X)", nil, 1}
        0x05 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ORA", "$#{to_hex(addr)}", nil, 1}
        0x06 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ASL", "$#{to_hex(addr)}", nil, 1}
        0x08 ->
          {"PHP", "", nil, 0}
        0x09 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ORA", "#$#{to_hex(addr)}", nil, 1}
        0x0a ->
          {"ASL", "A", nil, 0}
        0x0d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ORA", "$#{to_hex(addr16, 4)}", nil, 2}
        0x0e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ASL", "$#{to_hex(addr16, 4)}", nil, 2}
        0x10 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {"BPL", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0x11 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ORA", "($#{to_hex(addr)}), Y", nil, 1}
        0x15 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ORA", "$#{to_hex(addr)}, X", nil, 1}
        0x16 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ASL", "$#{to_hex(addr)}, X", nil, 1}
        0x18 ->
          {"CLC", "", nil, 0}
        0x19 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ORA", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0x1d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ORA", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x1e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ASL", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x20 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          edges = (if (addr16 - mem_offset < 0), do: [], else: [addr16 - mem_offset]) ++ [pos + 2] 
          IO.warn("addr: #{addr16 - mem_offset}")
          {"JSR", "$#{to_hex(addr16, 4)}", edges, 2}
        0x21 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"AND", "($#{to_hex(addr)}, X)", nil, 1}
        0x24 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"BIT", "$#{to_hex(addr)}", nil, 1}
        0x25 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"AND", "$#{to_hex(addr)}", nil, 1}
        0x26 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ROL", "$#{to_hex(addr)}", nil, 1}
        0x28 ->
          {"PLP", "", nil, 0}
        0x29 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"AND", "#$#{to_hex(val)}", nil, 1}
        0x2a ->
          {"ROL", "A", nil, 0}
        0x2c ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"BIT", "$#{to_hex(addr16, 4)}", nil, 2}
        0x2d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"AND", "$#{to_hex(addr16, 4)}", nil, 2}
        0x2e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ROL", "$#{to_hex(addr16, 4)}", nil, 2}
        0x30 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {"BMI", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0x31 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"AND", "($#{to_hex(addr)}), Y", nil, 1}
        0x35 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"AND", "$#{to_hex(addr)}, X", nil, 1}
        0x36 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ROL", "$#{to_hex(addr)}, X", nil, 1}
        0x38 ->
          {"SEC", "", nil, 0}
        0x39 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"AND", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0x3d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"AND", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x3e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ROL", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x40 ->
          {"RTI", "", [], 0}
        0x41 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"EOR", "($#{to_hex(addr)}, X)", nil, 1}
        0x45 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"EOR", "$#{to_hex(addr)}", nil, 1}
        0x46 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LSR", "$#{to_hex(addr)}", nil, 1}
        0x48 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"PHA", "", nil, 1}
        0x49 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"EOR", "#$#{to_hex(addr)}", nil, 1}
        0x4a ->
          {"LSR", "A", nil, 0}
        0x4c ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          edges = (if (addr16 - mem_offset < 0), do: [], else: [addr16 - mem_offset])
          IO.warn("addr #{addr16 - mem_offset}")
          {"JMP", "$#{to_hex(addr16, 4)}", edges, 2}
        0x4d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"EOR", "$#{to_hex(addr16, 4)}", nil, 2}
        0x4e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LSR", "$#{to_hex(addr16, 4)}", nil, 2}
        0x50 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {"BVC", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0x51 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"EOR", "($#{to_hex(addr)}), Y", nil, 1}
        0x55 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"EOR", "$#{to_hex(addr)}, X", nil, 1}
        0x56 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LSR", "$#{to_hex(addr)}, X", nil, 1}
        0x58 ->
          {"CLI", "", nil, 0}
        0x59 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"EOR", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0x5d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"EOR", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x5e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LSR", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x60 ->
          {"RTS", "", [], 0}
        0x61 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ADC", "($#{to_hex(addr)}, X)", nil, 1}
        0x65 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ADC", "$#{to_hex(addr)}", nil, 1}
        0x66 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ROR", "$#{to_hex(addr)}", nil, 1}
        0x68 ->
          {"PLA", "", nil, 0}
        0x69 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ADC", "#$#{to_hex(addr)}", nil, 1}
        0x6a ->
          {"ROR", "A", nil, 0}
        0x6c ->
          <<indirect_addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          edges = 
            cond do
              (indirect_addr16 - mem_offset) >= 0 ->
                <<addr16::little-integer-size(16)>> = get_bytes(arr, indirect_addr16 - mem_offset, 2)
                (if (addr16 - mem_offset < 0), do: [], else: [addr16 - mem_offset])
              true -> []
            end
          {"JMP", "($#{to_hex(indirect_addr16)})", edges, 2}
        0x6d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ADC", "$#{to_hex(addr16, 4)}", nil, 2}
        0x6e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ROR", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x70 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {"BVS", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0x71 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ADC", "($#{to_hex(addr)}), Y", nil, 1}
        0x75 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ADC", "$#{to_hex(addr)}, X", nil, 1}
        0x76 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"ROR", "$#{to_hex(addr)}, X", nil, 1}
        0x78 ->
          {"SEI", "", nil, 0}
        0x79 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ADC", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0x7d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ADC", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0x7e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"ROR", "$#{to_hex(addr16, 4)}", nil, 2}
        0x81 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STA", "($#{to_hex(addr)}, X)", nil, 1}
        0x84 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STY", "$#{to_hex(addr)}", nil, 1}
        0x85 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STA", "$#{to_hex(addr)}", nil, 1}
        0x86 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STX", "$#{to_hex(addr)}", nil, 1}
        0x88 ->
          {"DEY", "", nil, 0}
        0x8a ->
          {"TXA", "", nil, 0}
        0x8c ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"STY", "$#{to_hex(addr16, 4)}", nil, 2}
        0x8d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"STA", "$#{to_hex(addr16, 4)}", nil, 2}
        0x8e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"STX", "$#{to_hex(addr16, 4)}", nil, 2}
        0x90 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {"BCC", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0x91 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STA", "($#{to_hex(addr)}), Y", nil, 1}
        0x94 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STY", "$#{to_hex(addr)}, X", nil, 1}
        0x95 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STA", "$#{to_hex(addr)}, X", nil, 1}
        0x96 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"STX", "$#{to_hex(addr)}, Y", nil, 1}
        0x98 ->
          {"TYA", "", nil, 0}
        0x99 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"STA", "($#{to_hex(addr16, 4)}), Y", nil, 2}
        0x9a ->
          {"TXS", "", nil, 0}
        0x9d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"STA", "($#{to_hex(addr16, 4)}), X", nil, 2}
        0xa0 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"LDY", "#$#{to_hex(val)}", nil, 1}
        0xa1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDA", "($#{to_hex(addr)}, X)", nil, 1}
        0xa2 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"LDX", "#$#{to_hex(val)}", nil, 1}
        0xa4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDY", "$#{to_hex(addr)}", nil, 1}
        0xa5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDA", "$#{to_hex(addr)}", nil, 1}
        0xa6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDX", "$#{to_hex(addr)}", nil, 1}
        0xa8 ->
          {"TAY", "", nil, 0}
        0xa9 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"LDA", "#$#{to_hex(val)}", nil, 1}
        0xaa ->
          {"TAX", "", nil, 0}
        0xac ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LDY", "$#{to_hex(addr16, 4)}", nil, 2}
        0xad ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LDA", "$#{to_hex(addr16, 4)}", nil, 2}
        0xae ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LDX", "$#{to_hex(addr16, 4)}", nil, 2}
        0xb0 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {"BCS", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0xb1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDA", "($#{to_hex(addr)}), Y", nil, 1}
        0xb4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDY", "$#{to_hex(addr)}, X", nil, 1}
        0xb5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDA", "$#{to_hex(addr)}, X", nil, 1}
        0xb6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"LDX", "$#{to_hex(addr)}, Y", nil, 1}
        0xb8 ->
          {"CLV", "", nil, 0}
        0xb9 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LDA", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0xba ->
          {"TSX", "", nil, 0}
        0xbc ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LDY", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0xbd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LDA", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0xbe ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"LDX", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0xc0 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"CPY", "#$#{to_hex(val)}", nil, 1}
        0xc1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"CMP", "($#{to_hex(addr)}, X)", nil, 1}
        0xc4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"CPY", "$#{to_hex(addr)}", nil, 1}
        0xc5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"CMP", "$#{to_hex(addr)}", nil, 1}
        0xc6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"DEC", "$#{to_hex(addr)}", nil, 1}
        0xc8 ->
          {"INY", "", nil, 0}
        0xc9 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"CMP", "#$#{to_hex(val)}", nil, 1}
        0xca ->
          {"DEX", "", nil, 0}
        0xcc ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"CPY", "$#{to_hex(addr16, 4)}", nil, 2}
        0xcd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"CMP", "$#{to_hex(addr16, 4)}", nil, 2}
        0xce ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"DEC", "$#{to_hex(addr16, 4)}", nil, 2}
        0xd0 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {"BNE", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0xd1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"CMP", "($#{to_hex(addr)}), Y", nil, 1}
        0xd5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"CMP", "$#{to_hex(addr)}, X", nil, 1}
        0xd6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"DEC", "$#{to_hex(addr)}, X", nil, 1}
        0xd8 ->
          {"CLD", "", nil, 0}
        0xd9 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"CMP", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0xdd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"CMP", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0xde ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"DEC", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0xe0 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"CPX", "#$#{to_hex(val)}", nil, 1}
        0xe1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"SBC", "($#{to_hex(addr)}, X)", nil, 1}
        0xe4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"CPX", "$#{to_hex(addr)}", nil, 1}
        0xe5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"SBC", "$#{to_hex(addr)}", nil, 1}
        0xe6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"INC", "$#{to_hex(addr)}", nil, 1}
        0xe8 ->
          {"INX", "", nil, 0}
        0xe9 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {"SBC", "#$#{to_hex(val)}", nil, 1}
        0xea ->
          {"NOP", "", nil, 0}
        0xec ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"CPX", "$#{to_hex(addr16, 4)}", nil, 2}
        0xed ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"SBC", "$#{to_hex(addr16, 4)}", nil, 2}
        0xee ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"INC", "$#{to_hex(addr16, 4)}", nil, 2}
        0xf0 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)        
          {"BEQ", "$#{to_hex(offset)}", [pos + offset + 1, pos + 1], 1}
        0xf1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"SBC", "($#{to_hex(addr)}), Y", nil, 1}
        0xf5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"SBC", "$#{to_hex(addr)}, X", nil, 1}
        0xf6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {"INC", "$#{to_hex(addr)}, X", nil, 1}
        0xf8 ->
          {"SED", "", nil, 0}
        0xf9 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"SBC", "$#{to_hex(addr16, 4)}, Y", nil, 2}
        0xfd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"SBC", "$#{to_hex(addr16, 4)}, X", nil, 2}
        0xfe ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {"INC", "$#{to_hex(addr16, 4)}, X", nil, 2}
        _ ->
          # Unknown operation (could be data or illegal operation)
          {"UNK", "", nil, 0}
      end
      following = pos + consumed
      {edges, label} =  
        case edges do
          nil -> {[following], false}
          []  -> {[], false}
          [following] -> {edges, false}
          _   -> {edges, true}
        end

      vertex = %Vertex{instruction: instruction,
                      args: args, 
                      edges: edges, 
                      length: (1 + consumed)}
      {vertex, label}
  end

end
