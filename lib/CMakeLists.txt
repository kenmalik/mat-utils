add_library(mat_utils "mat_reader.cpp" "mat_writer.cpp")

find_package(Matlab REQUIRED MAT_LIBRARY MX_LIBRARY)
target_include_directories(mat_utils PRIVATE ${Matlab_INCLUDE_DIRS})
target_link_libraries(mat_utils PRIVATE ${Matlab_MAT_LIBRARY} ${Matlab_MX_LIBRARY})

target_include_directories(mat_utils PUBLIC "${PROJECT_SOURCE_DIR}/include")
