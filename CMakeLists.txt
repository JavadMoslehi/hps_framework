#1. Minimum CMake Version
#2. Project Definition
#3. Language Standard Configuration
#4. Compiler Detection
#5. Compiler Feature Validation    ← اینجا
#6. Testing Infrastructure
#7. Library Targets
#8. Executable Test Targets
#9. CTest Registration






# ============================================================
# HPS Framework
# High-Performance and Safe C Framework
#
# Root CMake configuration.
#
# Responsibilities:
#   - Configure the C language standard.
#   - Validate mandatory compiler capabilities.
#   - Define library targets.
#   - Configure verification targets.
#   - Register tests with CTest.
#
# Compiler-specific warning policies and other build logic
# belong to dedicated CMake modules.
# ============================================================


# ------------------------------------------------------------
# 1. Minimum supported CMake version
# ------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)


# ------------------------------------------------------------
# 2. Project definition
# ------------------------------------------------------------

project(
        HPS
        VERSION 0.1.0
        DESCRIPTION "High-Performance and Safe C Framework"
        LANGUAGES C
)


# ------------------------------------------------------------
# 3. C language standard configuration
#
# HPS_C_STANDARD is selected through CMake Presets.
#
# Supported values:
#   90 -> C89/C90
#   99 -> C99
#   11 -> C11
#   17 -> C17
#   23 -> C23 (compiler-dependent completeness)
# ------------------------------------------------------------

set(
        HPS_C_STANDARD
        "90"
        CACHE STRING
        "ISO C standard used for HPS verification"
)

set_property(
        CACHE HPS_C_STANDARD
        PROPERTY STRINGS
        90 99 11 17 23
)

set(HPS_SUPPORTED_C_STANDARDS
        90
        99
        11
        17
        23
)

if(NOT HPS_C_STANDARD IN_LIST HPS_SUPPORTED_C_STANDARDS)
    message(FATAL_ERROR
            "HPS: Unsupported C standard: ${HPS_C_STANDARD}"
    )
endif()


# Request the selected standard for new C targets.

set(CMAKE_C_STANDARD ${HPS_C_STANDARD})

# Do not silently fall back to an older C standard.
set(CMAKE_C_STANDARD_REQUIRED ON)

# Request ISO C mode instead of GNU extensions.
set(CMAKE_C_EXTENSIONS OFF)


# ------------------------------------------------------------
# 4. Build configuration diagnostics
# ------------------------------------------------------------

message(STATUS
        "HPS: C standard = C${HPS_C_STANDARD}"
)

message(STATUS
        "HPS: Compiler = ${CMAKE_C_COMPILER_ID}"
)

message(STATUS
        "HPS: Compiler version = ${CMAKE_C_COMPILER_VERSION}"
)


# ------------------------------------------------------------
# 5. Compiler feature and warning validation
#
# All mandatory compiler flags must be validated before
# configuring the library and verification targets.
# ------------------------------------------------------------

include(cmake/modules/HPSCompilerFeatures.cmake)






# ------------------------------------------------------------
# 6. Compiler warning policy
#
# This module defines hps_enable_strict_warnings().
# ------------------------------------------------------------

include(cmake/modules/HPSCompilerWarnings.cmake)





include(cmake/modules/HPSBuildProfiles.cmake)


# ------------------------------------------------------------
# 7. Compilation database
#
# compile_commands.json is useful for:
#   - Clang-based static analysis
#   - IDE code navigation
#   - Code inspection tools
# ------------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ------------------------------------------------------------
# 8. Testing infrastructure
#
# CTest provides the BUILD_TESTING option.
# ------------------------------------------------------------

include(CTest)


# ------------------------------------------------------------
# Enable C++ only for interoperability verification.
#
# HPS itself remains a C project.
# C++ is required only when BUILD_TESTING is enabled.
# ------------------------------------------------------------

if(BUILD_TESTING)
    enable_language(CXX)
endif()






# ------------------------------------------------------------
# C++ interoperability verification configuration
#
# HPS remains a C framework. C++ is enabled only for tests.
#
# Supported test modes:
#   C++98, C++11, C++14, C++17, C++20, C++23
# ------------------------------------------------------------

if(BUILD_TESTING)

    set(HPS_CXX_STANDARD "98" CACHE STRING
            "C++ standard for HPS interoperability verification"
    )

    set_property(CACHE HPS_CXX_STANDARD PROPERTY STRINGS
            98 11 14 17 20 23
    )

    set(HPS_SUPPORTED_CXX_STANDARDS
            98 11 14 17 20 23
    )

    if(NOT HPS_CXX_STANDARD IN_LIST HPS_SUPPORTED_CXX_STANDARDS)
        message(FATAL_ERROR
                "HPS: Unsupported C++ standard: ${HPS_CXX_STANDARD}"
        )
    endif()

    message(STATUS
            "HPS: C++ verification standard = C++${HPS_CXX_STANDARD}"
    )

endif()



# ------------------------------------------------------------
# 9. HPS Base interface target
#
# No implementation has been written yet.
#
# This is currently a header-only interface target.
# ------------------------------------------------------------

add_library(hps_base INTERFACE)

add_library(HPS::base ALIAS hps_base)

target_include_directories(hps_base INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)


# ------------------------------------------------------------
# 10. Verification targets
# ------------------------------------------------------------

if(BUILD_TESTING)

    # --------------------------------------------------------
    # 10.1 C environment smoke test
    #
    # This source uses C89-compatible syntax and is compiled
    # under the standard selected by HPS_C_STANDARD.
    # --------------------------------------------------------

    add_executable(hps_test_c89
            tests/portability/test_c89.c
    )

    set_target_properties(hps_test_c89 PROPERTIES
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )

    hps_enable_strict_warnings(hps_test_c89)
    hps_apply_build_profile(hps_test_c89)

    add_test(
            NAME hps_portability_c89
            COMMAND hps_test_c89
    )


    # --------------------------------------------------------
    # 10.2 Language standard verification
    #
    # Compare the compiler-reported language version with
    # the standard requested by the HPS build configuration.
    # --------------------------------------------------------

    add_executable(hps_test_language_standard
            tests/portability/test_language_standard.c
    )

    set_target_properties(
            hps_test_language_standard
            PROPERTIES
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )

    # Pass the expected C standard to the preprocessor.
    target_compile_definitions(
            hps_test_language_standard
            PRIVATE
            HPS_EXPECTED_C_STANDARD=${HPS_C_STANDARD}
    )


    # --------------------------------------------------------
    # 10.3 GCC C2x draft compatibility
    #
    # GCC 13 uses the draft C2x mode.
    # Passing this test does not establish full C23 compliance.
    # --------------------------------------------------------

    if(
            HPS_C_STANDARD STREQUAL "23"
            AND CMAKE_C_COMPILER_ID STREQUAL "GNU"
    )

        target_compile_definitions(
                hps_test_language_standard
                PRIVATE
                HPS_ALLOW_C23_DRAFT=1
        )

        message(STATUS
                "HPS: GCC C23 verification permits draft C2x"
        )

    endif()

    hps_enable_strict_warnings(hps_test_language_standard)
    hps_apply_build_profile(hps_test_language_standard)

    add_test(
            NAME hps_language_standard
            COMMAND hps_test_language_standard
    )





    # --------------------------------------------------------
    # HPS Base: Language Abstraction Unit Test
    #
    # Verifies the public language abstraction header.
    #
    # The test must compile under the selected C standard.
    # --------------------------------------------------------

    add_executable(hps_test_base_language
            tests/unit/base/test_language.c
    )

    # Enforce the language standard selected by the preset.
    set_target_properties(hps_test_base_language PROPERTIES
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )

    # Inherit the public include paths from HPS::base.
    target_link_libraries(hps_test_base_language PRIVATE
            HPS::base
    )

    # Supply the expected standard to the C preprocessor.
    target_compile_definitions(hps_test_base_language PRIVATE
            HPS_EXPECTED_C_STANDARD=${HPS_C_STANDARD}
    )

    # Apply mandatory compiler warnings.
    hps_enable_strict_warnings(hps_test_base_language)

    # Enable ASan/UBSan if the SANITIZED profile is selected.
    hps_apply_build_profile(hps_test_base_language)

    # Register the executable as a CTest test.
    add_test(
            NAME hps_base_language
            COMMAND hps_test_base_language
    )






    # --------------------------------------------------------
    # 10.4 Python test utilities
    #
    # Python is required for verification scripts.
    # It is not a runtime dependency of the HPS library.
    # --------------------------------------------------------

    find_package(
            Python3
            REQUIRED
            COMPONENTS Interpreter
    )











    # --------------------------------------------------------
    # HPS Static Assertion Negative Verification
    #
    # The same intentionally invalid source is checked in
    # both C90 and C++98 language modes.
    #
    # The Python harness validates the compiler diagnostic.
    # --------------------------------------------------------

    add_test(
            NAME hps_static_assert_negative_c
            COMMAND
            ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/tools/test_static_assert_negative.py
            --compiler ${CMAKE_C_COMPILER}
            --language c
            --source
            ${PROJECT_SOURCE_DIR}/tests/portability/test_static_assert_negative.c
            --include
            ${PROJECT_SOURCE_DIR}/include
    )

    add_test(
            NAME hps_static_assert_negative_cpp
            COMMAND
            ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/tools/test_static_assert_negative.py
            --compiler ${CMAKE_CXX_COMPILER}
            --language cpp
            --source
            ${PROJECT_SOURCE_DIR}/tests/portability/test_static_assert_negative.c
            --include
            ${PROJECT_SOURCE_DIR}/include
    )

    














    # ------------------------------------------------------------
    # Static analyzer self-verification
    #
    # The source intentionally contains a double-free defect.
    #
    # The analyzer must detect the defect without executing it.
    # ------------------------------------------------------------

    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")

        add_test(
                NAME hps_gcc_analyzer_selfcheck
                COMMAND
                ${Python3_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/tools/test_static_analyzer.py
                --compiler ${CMAKE_C_COMPILER}
                --analyzer gcc
                --source
                ${PROJECT_SOURCE_DIR}/tests/static_analysis/test_double_free.c
        )

    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")

        add_test(
                NAME hps_clang_analyzer_selfcheck
                COMMAND
                ${Python3_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/tools/test_static_analyzer.py
                --compiler ${CMAKE_C_COMPILER}
                --analyzer clang
                --source
                ${PROJECT_SOURCE_DIR}/tests/static_analysis/test_double_free.c
        )

    endif()








    # ------------------------------------------------------------
    # Sanitizer self-tests
    #
    # These programs intentionally contain undefined behavior.
    # They must never be built in STRICT or RELEASE profiles.
    # ------------------------------------------------------------

    if(HPS_BUILD_PROFILE STREQUAL "SANITIZED")

        # --------------------------------------------------------
        # AddressSanitizer: heap-use-after-free
        # --------------------------------------------------------

        add_executable(hps_test_asan_selfcheck
                tests/sanitizers/test_asan_use_after_free.c
        )

        hps_enable_strict_warnings(hps_test_asan_selfcheck)
        hps_apply_build_profile(hps_test_asan_selfcheck)

        add_test(
                NAME hps_asan_selfcheck
                COMMAND
                ${Python3_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/tools/test_sanitizer_diagnostic.py
                --sanitizer asan
                --executable $<TARGET_FILE:hps_test_asan_selfcheck>
        )


        # --------------------------------------------------------
        # UndefinedBehaviorSanitizer: signed overflow
        # --------------------------------------------------------

        add_executable(hps_test_ubsan_selfcheck
                tests/sanitizers/test_ubsan_signed_overflow.c
        )

        hps_enable_strict_warnings(hps_test_ubsan_selfcheck)
        hps_apply_build_profile(hps_test_ubsan_selfcheck)

        add_test(
                NAME hps_ubsan_selfcheck
                COMMAND
                ${Python3_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/tools/test_sanitizer_diagnostic.py
                --sanitizer ubsan
                --executable $<TARGET_FILE:hps_test_ubsan_selfcheck>
        )

    endif()



    # --------------------------------------------------------
    # 10.5 Negative language verification test
    #
    # The Python script intentionally compiles C99 source
    # while expecting C90.
    #
    # The test passes only when the compiler rejects this
    # mismatch for the expected reason.
    # --------------------------------------------------------

    add_test(
            NAME hps_language_negative
            COMMAND
            ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/tools/test_language_negative.py
            ${CMAKE_C_COMPILER}
            ${PROJECT_SOURCE_DIR}/tests/portability/test_language_standard.c
    )

endif()




# ------------------------------------------------------------
# HPS Static Analysis Targets
#
# Analysis is optional and does not run during normal builds.
# Intentional defect samples are excluded.
# ------------------------------------------------------------

if(BUILD_TESTING)

    # Initial list of clean sources to analyze.
    # Later, production sources will be managed separately.
    set(HPS_ANALYSIS_SOURCES
            ${PROJECT_SOURCE_DIR}/tests/portability/test_c89.c
            ${PROJECT_SOURCE_DIR}/tests/portability/test_language_standard.c
    )

    # Clang-tidy must use a Clang compilation database.
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang"
            AND HPS_C_STANDARD STREQUAL "90")

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


    # Cppcheck analysis of the initial clean C90 sources.
    find_program(HPS_CPPCHECK cppcheck)

    if(HPS_CPPCHECK AND HPS_C_STANDARD STREQUAL "90")

        add_custom_target(hps_analyze_cppcheck

                COMMAND ${HPS_CPPCHECK}
                --std=c89
                --enable=warning,style,performance,portability
                --error-exitcode=1
                ${HPS_ANALYSIS_SOURCES}

                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                COMMENT "HPS: Running cppcheck analysis"
                VERBATIM
        )

    endif()







    # --------------------------------------------------------
    # C/C++ interoperability verification.
    #
    # The implementation is compiled as C.
    # The consumer is compiled as C++98.
    #
    # The test verifies that C linkage is correctly exposed
    # through the public HPS language abstraction.
    # --------------------------------------------------------

    add_library(hps_test_c_api STATIC
            tests/interop/c_api.c
    )

    # Make the public HPS headers available to the C API.
    target_link_libraries(hps_test_c_api PUBLIC
            HPS::base
    )

    set_target_properties(hps_test_c_api PROPERTIES
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )

    # Apply the C compiler warning policy.
    hps_enable_strict_warnings(hps_test_c_api)

    # Apply the selected build profile.
    hps_apply_build_profile(hps_test_c_api)


    # --------------------------------------------------------
    # C++ consumer of the C API.
    # --------------------------------------------------------

    add_executable(hps_test_cpp_linkage
            tests/interop/test_cpp_linkage.cpp
    )


    set_target_properties(hps_test_cpp_linkage PROPERTIES
            CXX_STANDARD ${HPS_CXX_STANDARD}
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )

    target_compile_definitions(hps_test_cpp_linkage PRIVATE
            HPS_EXPECTED_CXX_STANDARD=${HPS_CXX_STANDARD}
    )


    target_link_libraries(hps_test_cpp_linkage PRIVATE
            hps_test_c_api
    )

    # Basic strict C++ diagnostics.
    # A dedicated C++ warning policy will be added later.
    target_compile_options(hps_test_cpp_linkage PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -pedantic-errors
            -Werror
    )

    hps_apply_build_profile(hps_test_cpp_linkage)

    add_test(
            NAME hps_cpp_linkage
            COMMAND hps_test_cpp_linkage
    )









    # --------------------------------------------------------
    # HPS Base: Compile-Time Assertion Test
    #
    # Verifies portable assertions under the selected C
    # language standard and build profile.
    # --------------------------------------------------------

    add_executable(hps_test_static_assert
            tests/unit/base/test_static_assert.c
    )

    target_link_libraries(hps_test_static_assert PRIVATE
            HPS::base
    )

    set_target_properties(hps_test_static_assert PROPERTIES
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )

    hps_enable_strict_warnings(hps_test_static_assert)
    hps_apply_build_profile(hps_test_static_assert)

    add_test(
            NAME hps_static_assert
            COMMAND hps_test_static_assert
    )






    # ------------------------------------------------------------
    # C++ language standard verification
    #
    # Confirms that the requested C++ standard matches the
    # language mode reported by the compiler.
    # ------------------------------------------------------------

    add_executable(hps_test_cpp_standard
            tests/interop/test_cpp_standard.cpp
    )

    set_target_properties(hps_test_cpp_standard PROPERTIES
            CXX_STANDARD ${HPS_CXX_STANDARD}
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )

    target_link_libraries(hps_test_cpp_standard PRIVATE
            HPS::base
    )

    target_compile_definitions(hps_test_cpp_standard PRIVATE
            HPS_EXPECTED_CXX_STANDARD=${HPS_CXX_STANDARD}
    )

    target_compile_options(hps_test_cpp_standard PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -pedantic-errors
            -Werror
    )

    hps_apply_build_profile(hps_test_cpp_standard)

    add_test(
            NAME hps_cpp_standard
            COMMAND hps_test_cpp_standard
    )








endif()


