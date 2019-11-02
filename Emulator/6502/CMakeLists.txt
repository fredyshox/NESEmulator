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
  handlers/stack.c
  handlers/illegal.c)

add_library(6502
  STATIC
  ${6502lib_SOURCES})

export_headers(6502 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include/6502)

target_include_directories(6502
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# debug mode
#target_compile_definitions(6502 PUBLIC DEBUG=1)

# illegal opcodes on by default
target_compile_definitions(6502 PUBLIC ILLEGAL_OPCODES=1)

install(TARGETS 6502 DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")
