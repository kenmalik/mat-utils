add_executable(example "example.cpp")
target_link_libraries(example PRIVATE mat_utils)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(example PRIVATE -Wall -Wextra -Werror)
endif()
