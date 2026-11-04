
# ============================================================
# HPS Compiler Features
#
# Responsibilities:
#   - Validate compiler flags.
#   - Reject unsupported mandatory options.
#   - Prevent unknown Clang warnings from being ignored.
# ============================================================

include_guard(GLOBAL)

include(CheckCCompilerFlag)
include(CMakePushCheckState)


function(hps_require_c_compiler_flag flag)

    # HPS strict verification currently supports GCC and Clang.
    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$")
        message(FATAL_ERROR
                "HPS: Unsupported compiler: ${CMAKE_C_COMPILER_ID}"
        )
    endif()

    # Create a unique cache variable for this compiler,
    # compiler version, language standard and flag.
    string(
            SHA256 flag_hash
            "${CMAKE_C_COMPILER_ID}|${CMAKE_C_COMPILER_VERSION}|${HPS_C_STANDARD}|${flag}"
    )

    set(result_var "HPS_C_FLAG_${flag_hash}")

    # Isolate the temporary check settings.
    cmake_push_check_state(RESET)

    # Treat unknown or unsupported warning options as errors.
    set(CMAKE_REQUIRED_FLAGS "-Werror")

    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_REQUIRED_FLAGS
                " -Werror=unknown-warning-option"
        )
    endif()

#   # Ask CMake to compile a small test program with the flag.
#    check_c_compiler_flag("${flag}" ${result_var})


    # Sanitizers require instrumentation during compilation
    # and the corresponding runtime during linking.
    #
    # CheckCCompilerFlag normally applies the requested flag
    # to compilation, but not automatically to linking.
    if(flag MATCHES "^-fsanitize=")

        set(CMAKE_REQUIRED_LINK_OPTIONS
                "${flag}"
        )

    endif()

    # Compile and link the test program.
    check_c_compiler_flag(
            "${flag}"
            ${result_var}
    )




    cmake_pop_check_state()

    # A mandatory flag must never be silently skipped.
    if(NOT ${result_var})
        message(FATAL_ERROR
                "HPS: Required compiler flag is unsupported: ${flag}"
        )
    endif()

endfunction()
