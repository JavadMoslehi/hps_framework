
# ============================================================
# HPS Strict Compiler Warning Policy
#
# Common warnings are shared between GCC and Clang.
# Compiler-specific warnings are selected separately.
#
# All mandatory flags are validated before being applied.
# ============================================================

include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/HPSCompilerFeatures.cmake")


function(hps_enable_strict_warnings target)

    # --------------------------------------------------------
    # 1. Validate the target.
    # --------------------------------------------------------

    if(NOT TARGET ${target})
        message(FATAL_ERROR
                "HPS: Target does not exist: ${target}"
        )
    endif()


    # --------------------------------------------------------
    # 2. Common C compiler diagnostics.
    # --------------------------------------------------------

    set(hps_common_warnings
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


    # --------------------------------------------------------
    # 3. Compiler-specific diagnostics.
    # --------------------------------------------------------

    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")

        set(hps_compiler_warnings
                -Wcast-align=strict
        )

    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")

        set(hps_compiler_warnings
                -Wcast-align
                -Werror=unknown-warning-option
        )

    else()

        message(FATAL_ERROR
                "HPS: Unsupported compiler: ${CMAKE_C_COMPILER_ID}"
        )

    endif()


    # --------------------------------------------------------
    # 4. Combine warning policies.
    # --------------------------------------------------------

    set(hps_required_warnings
            ${hps_common_warnings}
            ${hps_compiler_warnings}
    )


    # --------------------------------------------------------
    # 5. Validate every mandatory flag.
    # --------------------------------------------------------

    foreach(flag IN LISTS hps_required_warnings)

        hps_require_c_compiler_flag("${flag}")

    endforeach()


    # --------------------------------------------------------
    # 6. Apply verified warnings to the target.
    #
    # PRIVATE prevents internal HPS warnings from being
    # propagated to downstream consumers.
    # --------------------------------------------------------

    target_compile_options(${target} PRIVATE
            ${hps_required_warnings}
    )

endfunction()
