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
    prefix = if label != nil, do: "    ", else: ""
    if (pointer < position) do
      if (label != :data) do
        IO.puts("section .data:")
      end
      IO.puts("#{prefix}.byte 0x#{to_hex(0xff)}")
      write(kv, pointer + 1, device, :data)
    else
      new_label = 
        case vertex.label do
          nil -> label
          _   ->
            IO.puts("#{vertex.label}:")
            vertex.label
        end
      IO.puts("#{prefix}#{vertex}", device)
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
    
    IO.inspect(rest ++ new_edges, [charlists: :as_lists])
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
    {vertex, consumed} =
      case opcode do
        0x00 ->
          {%Vertex{instruction: "BRK"}, 0}
        0x01 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ORA", 
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0x05 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ORA",
                  args: "$#{to_hex(addr)}"}, 1}
        0x06 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ASL",
                  args: "$#{to_hex(addr)}"}, 1}
        0x08 ->
          {%Vertex{instruction: "PHP"}, 0}
        0x09 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ORA",
                  args: "#$#{to_hex(addr)}"}, 1}
        0x0a ->
          {%Vertex{instruction: "ASL",
                  args: "A"}, 0}
        0x0d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ORA",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x0e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ASL",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x10 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BPL",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0x11 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ORA",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0x15 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ORA",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x16 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ASL",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x18 ->
          {%Vertex{instruction: "CLC"}, 0}
        0x19 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ORA",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0x1d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ORA",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x1e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ASL",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x20 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          edges = [pos + 2] ++ (if (addr16 - mem_offset < 0), do: [], else: [addr16 - mem_offset])
          IO.warn("addr: #{addr16 - mem_offset}")
          {%Vertex{instruction: "JSR",
                  args: "$#{to_hex(addr16, 4)}",
                  edges: edges}, 2}
        0x21 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "AND",
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0x24 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BIT",
                  args: "$#{to_hex(addr)}"}, 1}
        0x25 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "AND",
                  args: "$#{to_hex(addr)}"}, 1}
        0x26 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ROL",
                  args: "$#{to_hex(addr)}"}, 1}
        0x28 ->
          {%Vertex{instruction: "PLP"}, 0}
        0x29 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "AND",
                  args: "#$#{to_hex(val)}"}, 1}
        0x2a ->
          {%Vertex{instruction: "ROL",
                  args: "A"}, 0}
        0x2c ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "BIT",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x2d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "AND",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x2e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ROL",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x30 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BMI",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0x31 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "AND",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0x35 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "AND",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x36 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ROL",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x38 ->
          {%Vertex{instruction: "SEC"}, 0}
        0x39 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "AND",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0x3d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "AND",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x3e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ROL",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x40 ->
          {%Vertex{instruction: "RTI",
                  edges: []}, 0}
        0x41 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "EOR",
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0x45 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "EOR",
                  args: "$#{to_hex(addr)}"}, 1}
        0x46 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LSR",
                  args: "$#{to_hex(addr)}"}, 1}
        0x48 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "PHA"}, 1}
        0x49 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "EOR",
                  args: "#$#{to_hex(addr)}"}, 1}
        0x4a ->
          {%Vertex{instruction: "LSR",
                  args: "A"}, 0}
        0x4c ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          edges = (if (addr16 - mem_offset < 0), do: [], else: [addr16 - mem_offset])
          IO.warn("addr #{addr16 - mem_offset}")
          {%Vertex{instruction: "JMP",
                  args: "$#{to_hex(addr16, 4)}",
                  edges: edges}, 2}
        0x4d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "EOR",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x4e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LSR",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x50 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BVC",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0x51 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "EOR",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0x55 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "EOR",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x56 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LSR",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x58 ->
          {%Vertex{instruction: "CLI"}, 0}
        0x59 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "EOR",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0x5d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "EOR",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x5e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LSR",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x60 ->
          {%Vertex{instruction: "RTS",
                  edges: []}, 0}
        0x61 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ADC",
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0x65 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ADC",
                  args: "$#{to_hex(addr)}"}, 1}
        0x66 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ROR",
                  args: "$#{to_hex(addr)}"}, 1}
        0x68 ->
          {%Vertex{instruction: "PLA"}, 0}
        0x69 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ADC",
                  args: "#$#{to_hex(addr)}"}, 1}
        0x6a ->
          {%Vertex{instruction: "ROR",
                  args: "A"}, 0}
        0x6c ->
          <<indirect_addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          edges = 
            cond do
              (indirect_addr16 - mem_offset) >= 0 ->
                <<addr16::little-integer-size(16)>> = get_bytes(arr, indirect_addr16 - mem_offset, 2)
                (if (addr16 - mem_offset < 0), do: [], else: [addr16 - mem_offset])
              true -> []
            end
          {%Vertex{instruction: "JMP",
                  args: "($#{to_hex(indirect_addr16)})",
                  edges: edges}, 2}
        0x6d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ADC",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x6e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ROR",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x70 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BVS",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0x71 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ADC",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0x75 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ADC",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x76 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "ROR",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x78 ->
          {%Vertex{instruction: "SEI"}, 0}
        0x79 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ADC",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0x7d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ADC",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0x7e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "ROR",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x81 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STA",
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0x84 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STY",
                  args: "$#{to_hex(addr)}"}, 1}
        0x85 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STA",
                  args: "$#{to_hex(addr)}"}, 1}
        0x86 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STX",
                  args: "$#{to_hex(addr)}"}, 1}
        0x88 ->
          {%Vertex{instruction: "DEY"}, 0}
        0x8a ->
          {%Vertex{instruction: "TXA"}, 0}
        0x8c ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "STY",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x8d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "STA",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x8e ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "STX",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0x90 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BCC",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0x91 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STA",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0x94 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STY",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x95 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STA",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0x96 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "STX",
                  args: "$#{to_hex(addr)}, Y"}, 1}
        0x98 ->
          {%Vertex{instruction: "TYA"}, 0}
        0x99 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "STA",
                  args: "($#{to_hex(addr16, 4)}), Y"}, 2}
        0x9a ->
          {%Vertex{instruction: "TXS"}, 0}
        0x9d ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "STA",
                  args: "($#{to_hex(addr16, 4)}), X"}, 2}
        0xa0 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDY",
                  args: "#$#{to_hex(val)}"}, 1}
        0xa1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDA",
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0xa2 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDX",
                  args: "#$#{to_hex(val)}"}, 1}
        0xa4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDY",
                  args: "$#{to_hex(addr)}"}, 1}
        0xa5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDA",
                  args: "$#{to_hex(addr)}"}, 1}
        0xa6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDX",
                  args: "$#{to_hex(addr)}"}, 1}
        0xa8 ->
          {%Vertex{instruction: "TAY"}, 0}
        0xa9 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDA",
                  args: "#$#{to_hex(val)}"}, 1}
        0xaa ->
          {%Vertex{instruction: "TAX"}, 0}
        0xac ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LDY",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xad ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LDA",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xae ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LDX",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xb0 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BCS",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0xb1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDA",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0xb4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDY",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0xb5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDA",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0xb6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "LDX",
                  args: "$#{to_hex(addr)}, Y"}, 1}
        0xb8 ->
          {%Vertex{instruction: "CLV"}, 0}
        0xb9 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LDA",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0xba ->
          {%Vertex{instruction: "TSX"}, 0}
        0xbc ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LDY",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0xbd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LDA",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0xbe ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "LDX",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0xc0 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CPY",
                  args: "#$#{to_hex(val)}"}, 1}
        0xc1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CMP",
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0xc4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CPY",
                  args: "$#{to_hex(addr)}"}, 1}
        0xc5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CMP",
                  args: "$#{to_hex(addr)}"}, 1}
        0xc6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "DEC",
                  args: "$#{to_hex(addr)}"}, 1}
        0xc8 ->
          {%Vertex{instruction: "INY"}, 0}
        0xc9 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CMP",
                  args: "#$#{to_hex(val)}"}, 1}
        0xca ->
          {%Vertex{instruction: "DEX"}, 0}
        0xcc ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "CPY",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xcd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "CMP",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xce ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "DEC",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xd0 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "BNE",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0xd1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CMP",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0xd5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CMP",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0xd6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "DEC",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0xd8 ->
          {%Vertex{instruction: "CLD"}, 0}
        0xd9 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "CMP",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0xdd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "CMP",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0xde ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "DEC",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0xe0 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CPX",
                  args: "#$#{to_hex(val)}"}, 1}
        0xe1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "SBC",
                  args: "($#{to_hex(addr)}, X)"}, 1}
        0xe4 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "CPX",
                  args: "$#{to_hex(addr)}"}, 1}
        0xe5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "SBC",
                  args: "$#{to_hex(addr)}"}, 1}
        0xe6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "INC",
                  args: "$#{to_hex(addr)}"}, 1}
        0xe8 ->
          {%Vertex{instruction: "INX"}, 0}
        0xe9 ->
          <<val::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "SBC",
                  args: "#$#{to_hex(val)}"}, 1}
        0xea ->
          {%Vertex{instruction: "NOP"}, 0}
        0xec ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "CPX",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xed ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "SBC",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xee ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "INC",
                  args: "$#{to_hex(addr16, 4)}"}, 2}
        0xf0 ->
          <<offset::signed-integer-size(8)>> = get_bytes(arr, pos, 1)        
          {%Vertex{instruction: "BEQ",
                  args: "$#{to_hex(offset)}",
                  edges: [pos + 1, pos + offset + 1]}, 1}
        0xf1 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "SBC",
                  args: "($#{to_hex(addr)}), Y"}, 1}
        0xf5 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "SBC",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0xf6 ->
          <<addr::size(8)>> = get_bytes(arr, pos, 1)
          {%Vertex{instruction: "INC",
                  args: "$#{to_hex(addr)}, X"}, 1}
        0xf8 ->
          {%Vertex{instruction: "SED"}, 0}
        0xf9 ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "SBC",
                  args: "$#{to_hex(addr16, 4)}, Y"}, 2}
        0xfd ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "SBC",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        0xfe ->
          <<addr16::little-integer-size(16)>> = get_bytes(arr, pos, 2)
          {%Vertex{instruction: "INC",
                  args: "$#{to_hex(addr16, 4)}, X"}, 2}
        _ ->
          # Unknown operation (could be data or illegal operation)
          {%Vertex{instruction: "UNK"}, 0}
      end
      
      edges = if vertex.edges == nil, do: [pos + consumed], else: vertex.edges
      vertex = %{vertex | :edges => edges, :length => (1 + consumed)}
      {vertex, false}
  end

end
