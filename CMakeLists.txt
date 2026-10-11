cmake_minimum_required(VERSION 3.16)
project(HPS VERSION 0.1.0 LANGUAGES C)
include(GNUInstallDirs)

add_library(hps_base INTERFACE)
add_library(HPS::base ALIAS hps_base)
target_include_directories(hps_base INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(hps_base INTERFACE c_std_99)

add_library(hps_memory STATIC src/memory/pool.c)
add_library(HPS::memory ALIAS hps_memory)
target_link_libraries(hps_memory PUBLIC HPS::base)
set_target_properties(hps_memory PROPERTIES EXPORT_NAME memory
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

add_library(hps_bytes STATIC src/bytes/view.c src/bytes/buffer.c src/bytes/endian.c
    src/bytes/reader.c)
add_library(HPS::bytes ALIAS hps_bytes)
target_link_libraries(hps_bytes PUBLIC HPS::base)
set_target_properties(hps_bytes PROPERTIES EXPORT_NAME bytes
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

include(CTest)
if(BUILD_TESTING)
    add_executable(hps_tests tests/main.c tests/base/test_base.c
        tests/memory/test_pool.c tests/bytes/test_bytes.c tests/bytes/test_reader.c)
    target_link_libraries(hps_tests PRIVATE HPS::base HPS::memory HPS::bytes)
    set_target_properties(hps_tests PROPERTIES
        C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    # Test assertions must remain active in Release builds.
    if(MSVC)
        target_compile_options(hps_tests PRIVATE /UNDEBUG)
    else()
        target_compile_options(hps_tests PRIVATE -UNDEBUG)
    endif()
    add_test(NAME hps_tests COMMAND hps_tests)
    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_test(NAME hps_base_portability
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/base/check_portability.py
                --compiler ${CMAKE_C_COMPILER})
    endif()
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/HPSConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HPSConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HPS)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/HPSConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY ExactVersion)
set_target_properties(hps_base PROPERTIES EXPORT_NAME base)
install(TARGETS hps_base hps_memory hps_bytes EXPORT HPSTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/hps DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT HPSTargets NAMESPACE HPS:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HPS)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/HPSConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/HPSConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HPS)
