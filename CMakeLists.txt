cmake_minimum_required(VERSION 3.20)
project(Read_Mat_Test)

option(SUITESPARSE_MATRIX_BUILD_EXAMPLES OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("lib")

if(SUITESPARSE_MATRIX_BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()
