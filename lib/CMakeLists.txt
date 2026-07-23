add_library(mat_utils INTERFACE)
add_library(MatUtils::mat_utils ALIAS mat_utils)

target_include_directories(mat_utils INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

find_package(Matlab REQUIRED MAT_LIBRARY MX_LIBRARY)
target_include_directories(mat_utils INTERFACE ${Matlab_INCLUDE_DIRS})

target_link_libraries(mat_utils INTERFACE
    ${Matlab_MAT_LIBRARY}
    ${Matlab_MX_LIBRARY})

install(TARGETS mat_utils
    EXPORT MatUtilsTargets)
install(EXPORT MatUtilsTargets
    NAMESPACE MatUtils::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatUtils)
