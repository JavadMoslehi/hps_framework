cmake_minimum_required(VERSION 3.16)
project(HPS VERSION 0.1.0 LANGUAGES C)
include(GNUInstallDirs)

add_library(hps_base INTERFACE)
add_library(HPS::base ALIAS hps_base)
target_include_directories(hps_base INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(hps_base INTERFACE c_std_99)

add_library(hps_memory STATIC src/memory/pool.c src/memory/operations.c
    src/memory/array.c)
add_library(HPS::memory ALIAS hps_memory)
target_link_libraries(hps_memory PUBLIC HPS::base)
set_target_properties(hps_memory PROPERTIES EXPORT_NAME memory
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

add_library(hps_bytes STATIC src/bytes/view.c src/bytes/buffer.c src/bytes/codec.c src/bytes/endian.c
    src/bytes/reader.c)
add_library(HPS::bytes ALIAS hps_bytes)
target_link_libraries(hps_bytes PUBLIC HPS::base)
set_target_properties(hps_bytes PROPERTIES EXPORT_NAME bytes
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

add_library(hps_string STATIC src/string/view.c src/string/buffer.c
    src/string/char.c src/string/builder.c src/string/policy.c
    src/string/tokenizer.c src/string/indent.c)
add_library(HPS::string ALIAS hps_string)
target_link_libraries(hps_string PUBLIC HPS::base)
set_target_properties(hps_string PROPERTIES EXPORT_NAME string
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

add_library(hps_cstr STATIC src/cstr/core.c src/cstr/convert.c
    src/cstr/algorithm.c src/cstr/format.c)
add_library(HPS::cstr ALIAS hps_cstr)
target_link_libraries(hps_cstr PUBLIC HPS::string)
set_target_properties(hps_cstr PROPERTIES EXPORT_NAME cstr
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

add_library(hps_json STATIC src/json/tokenizer.c src/json/reader.c
    src/json/writer.c src/json/string.c)
add_library(HPS::json ALIAS hps_json)
target_link_libraries(hps_json PUBLIC HPS::string)
set_target_properties(hps_json PROPERTIES EXPORT_NAME json
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

add_library(hps_validation STATIC src/validation/national_code.c)
add_library(HPS::validation ALIAS hps_validation)
target_link_libraries(hps_validation PUBLIC HPS::string)
set_target_properties(hps_validation PROPERTIES EXPORT_NAME validation
    C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

option(HPS_BUILD_HOSTED_DEBUG "Build the optional hosted stderr/abort adapter" OFF)
if(HPS_BUILD_HOSTED_DEBUG)
    add_library(hps_hosted_debug STATIC src/platform/hosted/debug.c)
    add_library(HPS::hosted_debug ALIAS hps_hosted_debug)
    target_link_libraries(hps_hosted_debug PUBLIC HPS::base)
    set_target_properties(hps_hosted_debug PROPERTIES EXPORT_NAME hosted_debug
        C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    install(TARGETS hps_hosted_debug EXPORT HPSTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(hps_tests tests/main.c tests/base/test_base.c
        tests/memory/test_pool.c tests/memory/test_memory_operations.c
        tests/memory/test_memory_clone.c tests/bytes/test_bytes.c tests/bytes/test_reader.c
        tests/enum/test_enum.c tests/string/test_string.c
        tests/string/test_string_extended.c tests/string/test_string_builder.c
        tests/string/test_string_tokenizer.c tests/string/test_string_indent.c
        tests/span/test_span.c tests/data_structure/test_data_structure.c
        tests/data_structure/test_collection_operations.c tests/json/test_json.c
        tests/cstr/test_cstr.c tests/cstr/test_cstr_convert.c
        tests/cstr/test_cstr_format.c tests/validation/test_national_code.c
        tests/enum/test_cstr_enum.c tests/base/test_macro_utilities.c
        tests/memory/test_memory_utilities.c tests/result/test_result.c)
    target_link_libraries(hps_tests PRIVATE HPS::base HPS::memory HPS::bytes
        HPS::string HPS::cstr HPS::json HPS::validation)
    if(HPS_BUILD_HOSTED_DEBUG)
        target_sources(hps_tests PRIVATE tests/platform/test_hosted_debug.c)
        target_link_libraries(hps_tests PRIVATE HPS::hosted_debug)
        target_compile_definitions(hps_tests PRIVATE HPS_TEST_HOSTED_DEBUG=1)
    endif()
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
        add_test(NAME hps_environment_detection
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/base/check_detection.py
                --compiler ${CMAKE_C_COMPILER})
        add_test(NAME hps_attribute_contracts
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/base/check_attributes.py
                --compiler ${CMAKE_C_COMPILER})
        add_test(NAME hps_json_oracle
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/json/check_json.py
                --compiler ${CMAKE_C_COMPILER})
        if(HPS_BUILD_HOSTED_DEBUG)
            add_test(NAME hps_hosted_debug_contracts
                COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/platform/check_hosted_debug.py
                    --compiler ${CMAKE_C_COMPILER})
        endif()
    endif()
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/HPSConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HPSConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HPS)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/HPSConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY ExactVersion)
set_target_properties(hps_base PROPERTIES EXPORT_NAME base)
install(TARGETS hps_base hps_memory hps_bytes hps_string hps_cstr
    hps_json hps_validation EXPORT HPSTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/hps DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT HPSTargets NAMESPACE HPS:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HPS)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/HPSConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/HPSConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HPS)
