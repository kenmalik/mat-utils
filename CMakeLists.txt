cmake_minimum_required(VERSION 3.20)
project(Read_Mat_Test)

option(SUITESPARSE_MATRIX_BUILD_EXAMPLES OFF)

add_subdirectory("lib")

if(SUITESPARSE_MATRIX_BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()
