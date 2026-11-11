# ============================================================
# HPS Framework - Root CMake Configuration
# ============================================================
cmake_minimum_required(VERSION 3.21)

project(HPS VERSION 0.1.0 DESCRIPTION "High-Performance and Safe C Framework" LANGUAGES C)

# --- Language modes ----------------------------------------------------------
set(HPS_C_STANDARD "90" CACHE STRING "C standard for HPS verification")
set_property(CACHE HPS_C_STANDARD PROPERTY STRINGS 90 99 11 17 23)
set(HPS_SUPPORTED_C_STANDARDS 90 99 11 17 23)
if(NOT HPS_C_STANDARD IN_LIST HPS_SUPPORTED_C_STANDARDS)
    message(FATAL_ERROR "HPS: Unsupported C standard: ${HPS_C_STANDARD}")
endif()
set(CMAKE_C_STANDARD ${HPS_C_STANDARD})
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Build policies ----------------------------------------------------------
include(cmake/modules/HPSCompilerFeatures.cmake)
include(cmake/modules/HPSCompilerWarnings.cmake)
include(cmake/modules/HPSBuildProfiles.cmake)

message(STATUS "HPS: C standard = C${HPS_C_STANDARD}")
message(STATUS "HPS: Compiler = ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")

# --- Public interface --------------------------------------------------------
add_library(hps_base INTERFACE)
add_library(HPS::base ALIAS hps_base)
target_include_directories(hps_base INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# --- Testing -----------------------------------------------------------------
include(CTest)
if(BUILD_TESTING)
    enable_language(CXX)
    set(HPS_CXX_STANDARD "98" CACHE STRING "C++ standard for HPS interoperability verification")
    set_property(CACHE HPS_CXX_STANDARD PROPERTY STRINGS 98 11 14 17 20 23)
    set(HPS_SUPPORTED_CXX_STANDARDS 98 11 14 17 20 23)
    if(NOT HPS_CXX_STANDARD IN_LIST HPS_SUPPORTED_CXX_STANDARDS)
        message(FATAL_ERROR "HPS: Unsupported C++ standard: ${HPS_CXX_STANDARD}")
    endif()
    message(STATUS "HPS: C++ verification standard = C++${HPS_CXX_STANDARD}")
    find_package(Python3 REQUIRED COMPONENTS Interpreter)




    # --------------------------------------------------------
    # HPS Base Internal Header Access Verification
    #
    # Direct inclusion must fail with the expected error.
    # Public inclusion must succeed without leaking
    # HPS_BASE_INSIDE into the consuming translation unit.
    # --------------------------------------------------------

    foreach(HPS_ACCESS_MODE IN ITEMS direct public)

        add_test(
            NAME hps_internal_header_c_${HPS_ACCESS_MODE}
            COMMAND
                ${Python3_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/tools/test_internal_header_access.py
                --compiler ${CMAKE_C_COMPILER}
                --language c
                --mode ${HPS_ACCESS_MODE}
                --include ${PROJECT_SOURCE_DIR}/include
        )

        add_test(
            NAME hps_internal_header_cpp_${HPS_ACCESS_MODE}
            COMMAND
                ${Python3_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/tools/test_internal_header_access.py
                --compiler ${CMAKE_CXX_COMPILER}
                --language cpp
                --mode ${HPS_ACCESS_MODE}
                --include ${PROJECT_SOURCE_DIR}/include
        )

    endforeach()


    # C test targets share strict language and build-profile policies.
    function(hps_add_c_test target test_name source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE HPS::base)
        set_target_properties(${target} PROPERTIES
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
        )
        hps_enable_strict_warnings(${target})
        hps_apply_build_profile(${target})
        add_test(NAME ${test_name} COMMAND ${target})
    endfunction()

    # C++ test targets share strict language and build-profile policies.
    function(hps_add_cpp_test target test_name source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE HPS::base)
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD ${HPS_CXX_STANDARD}
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -pedantic-errors -Werror
        )
        hps_apply_build_profile(${target})
        add_test(NAME ${test_name} COMMAND ${target})
    endfunction()

    # C portability and base tests.
    hps_add_c_test(hps_test_c89 hps_portability_c89
        tests/portability/test_c89.c)
    hps_add_c_test(hps_test_language_standard hps_language_standard
        tests/portability/test_language_standard.c)
    target_compile_definitions(hps_test_language_standard PRIVATE
        HPS_EXPECTED_C_STANDARD=${HPS_C_STANDARD}
    )
    if(HPS_C_STANDARD STREQUAL "23" AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
        target_compile_definitions(hps_test_language_standard PRIVATE HPS_ALLOW_C23_DRAFT=1)
        message(STATUS "HPS: GCC C23 verification permits draft C2x")
    endif()
    hps_add_c_test(hps_test_base_language hps_base_language
        tests/unit/base/test_language.c)
    target_compile_definitions(hps_test_base_language PRIVATE
        HPS_EXPECTED_C_STANDARD=${HPS_C_STANDARD}
    )
    hps_add_c_test(hps_test_static_assert hps_static_assert
        tests/unit/base/test_static_assert.c)
    hps_add_c_test(hps_test_types hps_types
        tests/unit/base/test_types.c)

    # Test-only C library and C++ consumer verify cross-language linkage.
    add_library(hps_test_c_api STATIC tests/interop/c_api.c)
    target_link_libraries(hps_test_c_api PUBLIC HPS::base)
    set_target_properties(hps_test_c_api PROPERTIES
        C_STANDARD ${HPS_C_STANDARD}
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
    )
    hps_enable_strict_warnings(hps_test_c_api)
    hps_apply_build_profile(hps_test_c_api)

    hps_add_cpp_test(hps_test_cpp_linkage hps_cpp_linkage
        tests/interop/test_cpp_linkage.cpp)
    target_link_libraries(hps_test_cpp_linkage PRIVATE hps_test_c_api)
    target_compile_definitions(hps_test_cpp_linkage PRIVATE
        HPS_EXPECTED_CXX_STANDARD=${HPS_CXX_STANDARD}
    )
    hps_add_cpp_test(hps_test_cpp_standard hps_cpp_standard
        tests/interop/test_cpp_standard.cpp)
    target_compile_definitions(hps_test_cpp_standard PRIVATE
        HPS_EXPECTED_CXX_STANDARD=${HPS_CXX_STANDARD}
    )
    hps_add_cpp_test(hps_test_cpp_types hps_cpp_types
        tests/interop/test_cpp_types.cpp)

    # Intentional compile failures, verified by Python harnesses.
    add_test(NAME hps_language_negative COMMAND
        ${Python3_EXECUTABLE}
        ${PROJECT_SOURCE_DIR}/tools/test_language_negative.py
        ${CMAKE_C_COMPILER}
        ${PROJECT_SOURCE_DIR}/tests/portability/test_language_standard.c
    )
    foreach(lang IN ITEMS c cpp)
        if(lang STREQUAL "c")
            set(compiler "${CMAKE_C_COMPILER}")
        else()
            set(compiler "${CMAKE_CXX_COMPILER}")
        endif()
        add_test(NAME hps_static_assert_negative_${lang} COMMAND
            ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/tools/test_static_assert_negative.py
            --compiler ${compiler}
            --language ${lang}
            --source ${PROJECT_SOURCE_DIR}/tests/portability/test_static_assert_negative.c
            --include ${PROJECT_SOURCE_DIR}/include
        )
    endforeach()

    # Intentional double-free verified by compiler-specific analyzer.
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(analyzer "gcc")
        set(analyzer_test "hps_gcc_analyzer_selfcheck")
    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set(analyzer "clang")
        set(analyzer_test "hps_clang_analyzer_selfcheck")
    endif()
    if(DEFINED analyzer)
        add_test(NAME ${analyzer_test} COMMAND
            ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/tools/test_static_analyzer.py
            --compiler ${CMAKE_C_COMPILER}
            --analyzer ${analyzer}
            --source ${PROJECT_SOURCE_DIR}/tests/static_analysis/test_double_free.c
        )
    endif()

    # Runtime sanitizer self-tests exist only for SANITIZED builds.
    if(HPS_BUILD_PROFILE STREQUAL "SANITIZED")
        add_executable(hps_test_asan_selfcheck tests/sanitizers/test_asan_use_after_free.c)
        hps_enable_strict_warnings(hps_test_asan_selfcheck)
        hps_apply_build_profile(hps_test_asan_selfcheck)
        add_test(NAME hps_asan_selfcheck COMMAND
            ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/tools/test_sanitizer_diagnostic.py
            --sanitizer asan
            --executable $<TARGET_FILE:hps_test_asan_selfcheck>
        )
        add_executable(hps_test_ubsan_selfcheck tests/sanitizers/test_ubsan_signed_overflow.c)
        hps_enable_strict_warnings(hps_test_ubsan_selfcheck)
        hps_apply_build_profile(hps_test_ubsan_selfcheck)
        add_test(NAME hps_ubsan_selfcheck COMMAND
            ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/tools/test_sanitizer_diagnostic.py
            --sanitizer ubsan
            --executable $<TARGET_FILE:hps_test_ubsan_selfcheck>
        )
    endif()

    # Optional standalone static-analysis targets (not part of normal build).
    # Keep intentionally invalid self-tests excluded.
    set(HPS_ANALYSIS_SOURCES
        ${PROJECT_SOURCE_DIR}/tests/portability/test_c89.c
        ${PROJECT_SOURCE_DIR}/tests/portability/test_language_standard.c
    )
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang" AND HPS_C_STANDARD STREQUAL "90")
        find_program(HPS_CLANG_TIDY clang-tidy)
        if(HPS_CLANG_TIDY)
            add_custom_target(hps_analyze_clang_tidy
                COMMAND ${HPS_CLANG_TIDY}
                    -p ${CMAKE_BINARY_DIR}
                    "-checks=-*,clang-analyzer-*,bugprone-*"
                    "-warnings-as-errors=*"
                    ${HPS_ANALYSIS_SOURCES}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                COMMENT "HPS: Running clang-tidy analysis"
                VERBATIM
            )
        endif()
    endif()
    find_program(HPS_CPPCHECK cppcheck)
    if(HPS_CPPCHECK AND HPS_C_STANDARD STREQUAL "90")
        add_custom_target(hps_analyze_cppcheck
            COMMAND ${HPS_CPPCHECK}
                --std=c89
                --enable=warning,style,performance,portability
                --force
                --error-exitcode=1
                ${HPS_ANALYSIS_SOURCES}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "HPS: Running cppcheck analysis"
            VERBATIM
        )
    endif()
endif()