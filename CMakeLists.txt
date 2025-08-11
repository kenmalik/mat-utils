cmake_minimum_required(VERSION 3.20)
project(Read_Mat_Test)

option(ENABLE_MATLAB "Enable MATLAB support" ON)

add_executable(read_mat "read_mat.cpp")

if(ENABLE_MATLAB)
    find_package(Matlab QUIET COMPONENTS MAT_LIBRARY)
    if(Matlab_FOUND)
        message(STATUS "MATLAB found: ${Matlab_ROOT_DIR}")
        add_definitions(-DWITH_MATLAB)
        target_include_directories(read_mat PRIVATE ${Matlab_INCLUDE_DIRS})
        target_link_libraries(read_mat PRIVATE ${Matlab_LIBRARIES})
    else()
        message(WARNING "MATLAB requested but not found.")
    endif()
endif()