cmake_minimum_required(VERSION 3.16)
project(C_Hash_Table C)

set(CMAKE_C_STANDARD 11)

add_executable(c_hash_table main.c prime.c prime.h hash_table.c hash_table.h hash_functions.c hash_functions.h)

target_link_libraries(c_hash_table m) # link the math library
