cmake_minimum_required(VERSION 3.20)
project(Mat_Utils VERSION 0.2.1)

option(MAT_UTILS_BUILD_EXAMPLES OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("lib")

if(MAT_UTILS_BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()
