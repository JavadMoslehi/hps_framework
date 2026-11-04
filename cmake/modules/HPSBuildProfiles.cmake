
# ============================================================
# HPS Build Profiles
#
# Supported profiles:
#   STRICT    - Strict diagnostics and standard verification.
#   SANITIZED - Strict diagnostics with ASan and UBSan.
#   RELEASE   - Optimized build with strict diagnostics.
#
# Profile selection is independent of the C language standard.
# ============================================================

include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/HPSCompilerFeatures.cmake")


# ------------------------------------------------------------
# 1. Profile selection
# ------------------------------------------------------------

set(
        HPS_BUILD_PROFILE
        "STRICT"
        CACHE STRING
        "HPS build verification profile"
)

set_property(
        CACHE HPS_BUILD_PROFILE
        PROPERTY STRINGS
        STRICT
        SANITIZED
        RELEASE
)

set(HPS_SUPPORTED_BUILD_PROFILES
        STRICT
        SANITIZED
        RELEASE
)

if(NOT HPS_BUILD_PROFILE IN_LIST HPS_SUPPORTED_BUILD_PROFILES)
    message(FATAL_ERROR
            "HPS: Unsupported build profile: ${HPS_BUILD_PROFILE}"
    )
endif()

message(STATUS "HPS: Build profile = ${HPS_BUILD_PROFILE}")


# ------------------------------------------------------------
# 2. Apply the selected profile to a target
# ------------------------------------------------------------

function(hps_apply_build_profile target)

    if(NOT TARGET ${target})
        message(FATAL_ERROR
                "HPS: Unknown target: ${target}"
        )
    endif()

    # STRICT:
    # The mandatory warning policy is applied separately
    # through hps_enable_strict_warnings().
    if(HPS_BUILD_PROFILE STREQUAL "STRICT")
        return()
    endif()

    # RELEASE:
    # Optimization is controlled by CMAKE_BUILD_TYPE=Release.
    # Warning policy remains enabled.
    if(HPS_BUILD_PROFILE STREQUAL "RELEASE")

        if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
            message(FATAL_ERROR
                    "HPS: RELEASE profile requires Release build type"
            )
        endif()

        return()
    endif()

    # SANITIZED:
    # Limit this profile to supported host compilers/platforms.
    if(HPS_BUILD_PROFILE STREQUAL "SANITIZED")

        if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
            message(FATAL_ERROR
                    "HPS: Sanitized profile currently requires Linux"
            )
        endif()

        if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$")
            message(FATAL_ERROR
                    "HPS: Sanitizers require GCC or Clang"
            )
        endif()

        if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
            message(FATAL_ERROR
                    "HPS: SANITIZED profile requires Debug build type"
            )
        endif()

        # Check the requested sanitizer option.
        hps_require_c_compiler_flag(
                "-fsanitize=address,undefined"
        )

        # Instrument source code during compilation.
        target_compile_options(${target} PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
                -fno-sanitize-recover=all
        )

        # Link the executable with sanitizer runtimes.
        target_link_options(${target} PRIVATE
                -fsanitize=address,undefined
                -fno-sanitize-recover=all
        )

    endif()

endfunction()

