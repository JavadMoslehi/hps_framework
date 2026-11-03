
cmake_minimum_required(VERSION 3.21)

project(
        HPS
        VERSION 0.1.0
        DESCRIPTION "High-Performance and Safe C Framework"
        LANGUAGES C
)

#set(CMAKE_C_STANDARD 90)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)



# HPS language standard selection.
set(HPS_C_STANDARD "90" CACHE STRING
        "ISO C standard used for HPS verification"
)

set_property(CACHE HPS_C_STANDARD PROPERTY
        STRINGS 90 99 11 17 23
)

set(HPS_SUPPORTED_C_STANDARDS 90 99 11 17 23)

if(NOT HPS_C_STANDARD IN_LIST HPS_SUPPORTED_C_STANDARDS)
    message(FATAL_ERROR
            "HPS: Unsupported C standard: ${HPS_C_STANDARD}"
    )
endif()

set(CMAKE_C_STANDARD ${HPS_C_STANDARD})
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

message(STATUS "HPS: C standard = C${HPS_C_STANDARD}")
message(STATUS "HPS: Compiler = ${CMAKE_C_COMPILER_ID}")




set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

include(cmake/modules/HPSCompilerWarnings.cmake)

add_library(hps_base INTERFACE
        tests/portability/test_c89.c)
add_library(HPS::base ALIAS hps_base)

target_include_directories(hps_base INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

if(BUILD_TESTING)
    add_executable(hps_test_c89
            tests/portability/test_c89.c
    )

    set_target_properties(hps_test_c89 PROPERTIES
#            C_STANDARD 90
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )
    hps_enable_strict_warnings(hps_test_c89)

    add_test(
            NAME hps_portability_c89
            COMMAND hps_test_c89
    )




    #######################################################
    # Verify the compiler's selected C language standard.
    add_executable(hps_test_language_standard
            tests/portability/test_language_standard.c
    )

    set_target_properties(hps_test_language_standard PROPERTIES
            C_STANDARD ${HPS_C_STANDARD}
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )

    target_compile_definitions(
            hps_test_language_standard
            PRIVATE
            HPS_EXPECTED_C_STANDARD=${HPS_C_STANDARD}
    )

    hps_enable_strict_warnings(hps_test_language_standard)

    add_test(
            NAME hps_language_standard
            COMMAND hps_test_language_standard
    )
    #######################################################


endif()
