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

macro(add_gtest_exec target source libs) 
  add_test_exec(${target} ${source} ${libs})
  target_link_libraries("test_${target}" ${GTEST_LIBRARY})
  target_include_directories("test_${target}" PRIVATE ${GTEST_INCLUDE_DIRS})
  target_compile_options("test_${target}" PRIVATE -std=c++17)
endmacro()

macro(add_unit_test target name args)
  add_test(NAME "test_${target}_${name}"
           COMMAND "test_${target}" ${args}
           WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")
endmacro()

macro(add_unit_gtest target args)
  add_test(NAME "test_${target}"
           COMMAND "test_${target}" ${args}
           WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")
endmacro()

# GTest

find_package(GTest 1.8.1 REQUIRED)
if (GTEST_FOUND)
  message(STATUS "GTest 1.8.1 found!")
else()
  message(FATAL_ERROR "GTest not found!")
endif()

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

add_gtest_exec(ppu_sprites
  ppu/sprites.cpp
  ppu)
  
add_gtest_exec(ppu_bg
  ppu/bg.cpp
  ppu)

add_gtest_exec(ppu_memory
  ppu/memory.cpp
  ppu)

add_gtest_exec(nes_roms
  nes/romtest.cpp
  nes)

add_gtest_exec(cartridge
  nes/cartridge.cpp
  nes)

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

add_unit_gtest(ppu_sprites "")
add_unit_gtest(ppu_bg "")
add_unit_gtest(ppu_memory "")
add_unit_gtest(nes_roms "")
add_unit_gtest(cartridge "")
