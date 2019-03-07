cmake_minimum_required(VERSION 3.7)

macro(add_test_exec target source libs)
  add_executable("test_${target}"
    ${source})

  target_link_libraries("test_${target}"
    ${libs})
endmacro()

macro(add_unit_test target name args)
  add_test(NAME "test_${target}_${name}"
           COMMAND "test_${target}" ${args}
           WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endmacro()

# test execs
add_test_exec(memory_op
  memory.c
  6502)

add_test_exec(arithmetic_op
  arithmetic.c
  6502)

add_test_exec(program_counter_op
  pc.c
  6502)

add_test_exec(flags_op
  flags.c
  6502)

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
