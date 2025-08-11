cmake_minimum_required(VERSION 3.20)
project(Read_Mat_Test)

add_subdirectory("lib")

add_executable(main "main.cpp")
target_include_directories(main PRIVATE "include")
target_link_libraries(main PRIVATE suitesparse_matrix)