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

    hps_enable_strict_warnings(
            hps_test_language_standard
    )

    add_test(
            NAME hps_language_standard
            COMMAND hps_test_language_standard
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

endif()
