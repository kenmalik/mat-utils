add_library(mat_reader "mat_reader.cpp")

find_package(Matlab REQUIRED MAT_LIBRARY MX_LIBRARY)
target_include_directories(mat_reader PRIVATE ${Matlab_INCLUDE_DIRS})
target_link_libraries(mat_reader PRIVATE ${Matlab_MAT_LIBRARY} ${Matlab_MX_LIBRARY})

target_include_directories(mat_reader PUBLIC "${PROJECT_SOURCE_DIR}/include")
