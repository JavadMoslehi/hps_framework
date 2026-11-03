include_guard(GLOBAL)

function(hps_enable_strict_warnings target)

    if(NOT TARGET ${target})
        message(FATAL_ERROR
                "HPS: Target '${target}' does not exist"
        )
    endif()

    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$")
        message(FATAL_ERROR
                "HPS: Unsupported compiler: ${CMAKE_C_COMPILER_ID}"
        )
    endif()

    # Common strict warnings supported by GCC and Clang.
    target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -pedantic-errors
            -Werror
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wstrict-prototypes
            -Wmissing-prototypes
            -Wpointer-arith
            -Wundef
            -Wswitch-enum
            -Wformat=2
    )

    # Compiler-specific alignment diagnostics.
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")

        target_compile_options(${target} PRIVATE
                -Wcast-align=strict
        )

    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")

        target_compile_options(${target} PRIVATE
                -Wcast-align
        )

    endif()

endfunction()

