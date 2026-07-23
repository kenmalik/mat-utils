cmake_minimum_required(VERSION 3.25)
project(MatUtils VERSION 0.4.0)

option(MAT_UTILS_BUILD_EXAMPLES OFF)
option(MAT_UTILS_BUILD_TESTS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library targets

add_library(mat_utils INTERFACE)
add_library(MatUtils::mat_utils ALIAS mat_utils)

target_include_directories(mat_utils INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

find_package(Matlab REQUIRED MAT_LIBRARY MX_LIBRARY)
target_include_directories(mat_utils INTERFACE ${Matlab_INCLUDE_DIRS})

target_link_libraries(mat_utils INTERFACE
    ${Matlab_MAT_LIBRARY}
    ${Matlab_MX_LIBRARY})

# Extras

if(MAT_UTILS_BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()

if(MAT_UTILS_BUILD_TESTS)
    add_subdirectory("test")
endif()

# Packaging

install(TARGETS mat_utils
    EXPORT MatUtilsTargets)
install(EXPORT MatUtilsTargets
    NAMESPACE MatUtils::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatUtils)

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
