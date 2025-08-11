add_executable(example "example.cpp")
target_link_libraries(example PRIVATE suitesparse_matrix)
target_include_directories(example PRIVATE "${PROJECT_SOURCE_DIR}/include")