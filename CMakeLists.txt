
cmake_minimum_required(VERSION 3.21)

project(
        HPS
        VERSION 0.1.0
        DESCRIPTION "High-Performance and Safe C Framework"
        LANGUAGES C
)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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
            C_STANDARD 90
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
    )

    hps_enable_strict_warnings(hps_test_c89)

    add_test(
            NAME hps_portability_c89
            COMMAND hps_test_c89
    )
endif()
