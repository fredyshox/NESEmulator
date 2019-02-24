cmake_minimum_required(VERSION 3.7)

macro(add_unit_test target source libs)
  add_executable("test_${target}"
    ${source})

  target_link_libraries("test_${target}"
    ${libs})

  add_test(NAME "test_${target}"
           COMMAND "test_${target}"
           WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endmacro()

add_unit_test(memory_op
  memory.c
  6502)

add_unit_test(arithmetic_op
  arithmetic.c
  6502)
