cmake_minimum_required(VERSION 3.7)

set(6502lib_SOURCES
  addr.c
  asm.c
  parser.c
  state.c
  handlers/arithmetic.c
  handlers/branch.c
  handlers/memory.c
  handlers/other.c
  handlers/stack.c)

add_library(6502
  STATIC
  ${6502lib_SOURCES})

export_headers(6502 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include/6502)

target_include_directories(6502
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS 6502 DESTINATION lib)
