cmake_minimum_required(VERSION 3.7)

set(PPU_SOURCES
  color.c
  ppu.c
  renderer.c)

add_library(ppu
  STATIC
  ${PPU_SOURCES})

target_include_directories(ppu
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ppu DESTINATION lib)
