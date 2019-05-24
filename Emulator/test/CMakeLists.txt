cmake_minimum_required(VERSION 3.7)

macro(add_test_exec target source libs)
  add_executable("test_${target}"
    ${source})

  target_link_libraries("test_${target}"
    ${libs})

  target_include_directories("test_${target}"
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  install(TARGETS "test_${target}" DESTINATION test)
endmacro()

macro(add_unit_test target name args)
  add_test(NAME "test_${target}_${name}"
           COMMAND "test_${target}" ${args}
           WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endmacro()

# test execs
add_test_exec(memory_op
  cpu/memory.c
  6502)

add_test_exec(arithmetic_op
  cpu/arithmetic.c
  6502)

add_test_exec(program_counter_op
  cpu/pc.c
  6502)

add_test_exec(flags_op
  cpu/flags.c
  6502)

add_test_exec(ints
  cpu/int.c
  6502)

add_test_exec(ppu_sprites
  ppu/sprites.c
  ppu)

add_test_exec(cartridge
  nes/cartridge.c
  nes)

# debugger

add_executable(debugger
  nes/debug.c)
target_compile_definitions(debugger PUBLIC DEBUG=1)
target_link_libraries(debugger
  nes 6502)
install(TARGETS debugger DESTINATION test)

# unit tests

add_unit_test(memory_op load "--load")
add_unit_test(memory_op store "--store")
add_unit_test(memory_op stack "--stack")

add_unit_test(arithmetic_op adc "--adc")
add_unit_test(arithmetic_op sbc "--sbc")

add_unit_test(program_counter_op branch "--branch")
add_unit_test(program_counter_op jump "--jump")
add_unit_test(program_counter_op jsr "--jsr")
add_unit_test(program_counter_op return "--return")

add_unit_test(flags_op flag "--flag")
add_unit_test(flags_op cmp "--cmp")

add_unit_test(ints interrupts "")

add_unit_test(ppu_sprites eval "--eval")
add_unit_test(ppu_sprites layout "--layout")

add_unit_test(cartridge loading "${CMAKE_CURRENT_SOURCE_DIR}/../../testsuite/Donkey\ Kong\ Classics\ \(U\).nes")
