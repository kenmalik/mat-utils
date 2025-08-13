add_library(suitesparse_matrix "suitesparse_matrix.cpp")

find_package(Matlab REQUIRED MAT_LIBRARY MX_LIBRARY)
target_include_directories(suitesparse_matrix PRIVATE ${Matlab_INCLUDE_DIRS})
target_link_libraries(suitesparse_matrix PRIVATE ${Matlab_MAT_LIBRARY} ${Matlab_MX_LIBRARY})

target_include_directories(suitesparse_matrix PUBLIC "${PROJECT_SOURCE_DIR}/include")
