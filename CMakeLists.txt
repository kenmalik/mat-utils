cmake_minimum_required(VERSION 3.25)
project(MatUtils VERSION 0.4.0)

option(MAT_UTILS_BUILD_EXAMPLES OFF)
option(MAT_UTILS_BUILD_TESTS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("lib")

include(GNUInstallDirs)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/MatUtilsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MatUtilsConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatUtils
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MatUtilsConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatUtils)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(MAT_UTILS_BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()

if(MAT_UTILS_BUILD_TESTS)
    add_subdirectory("test")
endif()
