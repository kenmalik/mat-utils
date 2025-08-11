add_library(suitesparse_matrix "suitesparse_matrix.cpp")

find_package(Matlab QUIET COMPONENTS MAT_LIBRARY)
if(Matlab_FOUND)
    message(STATUS "MATLAB found: ${Matlab_ROOT_DIR}")
    target_include_directories(suitesparse_matrix PUBLIC ${Matlab_INCLUDE_DIRS})
    target_link_libraries(suitesparse_matrix PRIVATE ${Matlab_LIBRARIES})
else()
    message(WARNING "MATLAB requested but not found.")
endif()

target_include_directories(suitesparse_matrix PRIVATE "${PROJECT_SOURCE_DIR}/include")