cmake_minimum_required(VERSION 3.0)
project(P7 C)

set(CMAKE_C_STANDARD 99)

link_directories(./)

add_executable(linker linker.c my_assembler_utils.c my_linker_utils.c)
add_executable(assembler assembler.c my_assembler_utils.c)

target_link_libraries(assembler libP7.a)
target_link_libraries(linker libP7.a)
