cmake_minimum_required(VERSION 3.7)

project(nesppu)

set(PPU_SOURCES
  color.c
  ppu.c)

add_library(ppu
  STATIC
  ${PPU_SOURCES})

target_include_directories(ppu
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ppu DESTINATION lib)
