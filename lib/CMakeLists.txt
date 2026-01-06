add_library(mat_utils "mat_reader.cpp" "mat_writer.cpp")

target_include_directories(mat_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

find_package(Matlab REQUIRED MAT_LIBRARY MX_LIBRARY)
target_include_directories(mat_utils PRIVATE ${Matlab_INCLUDE_DIRS})

target_link_libraries(mat_utils PRIVATE
    ${Matlab_MAT_LIBRARY}
    ${Matlab_MX_LIBRARY})

install(TARGETS mat_utils
    EXPORT MatUtilsTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(EXPORT MatUtilsTargets
    NAMESPACE MatUtils::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatUtils)
