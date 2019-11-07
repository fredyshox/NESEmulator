cmake_minimum_required(VERSION 3.7)

# debugger

add_executable(debugger
  debug.c)
target_compile_definitions(debugger PUBLIC DEBUG=1)
target_link_libraries(debugger
  nes 6502)
install(TARGETS debugger DESTINATION test)

add_executable(run
  run.c)
target_compile_definitions(run PUBLIC DEBUG=1)
target_link_libraries(run
  nes)
install(TARGETS run DESTINATION test)

# renderer

find_package(OpenCV)
if( OpenCV_FOUND )
  message(STATUS "OpenCV found!")
  add_executable(renderer
    render.cpp)
  target_compile_definitions(renderer PUBLIC DEBUG=1)
  target_compile_options(renderer PRIVATE -std=c++11)
  target_include_directories(renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(renderer ${OpenCV_LIBS})
  target_link_libraries(renderer ppu)
  install(TARGETS renderer DESTINATION test)
else()
  message(WARNING "OpenCV not found!")
endif()
