add_library(suitesparse_matrix "suitesparse_matrix.cpp")

find_package(Matlab REQUIRED MAT_LIBRARY)
target_include_directories(suitesparse_matrix PUBLIC ${Matlab_INCLUDE_DIRS})
target_link_libraries(suitesparse_matrix PRIVATE ${Matlab_LIBRARIES})

target_include_directories(suitesparse_matrix PRIVATE "${PROJECT_SOURCE_DIR}/include")