#ifndef HPS_BASE_FEATURES_COMPILER_H
#define HPS_BASE_FEATURES_COMPILER_H

/**
 * @file hps/base/features/compiler.h
 * @brief Compiler capability and extension feature flags.
 */

#include "hps/base/compiler.h"
#include "hps/base/language.h"


/* This existing compatibility flag is informational even in strict ISO modes.
 * Individual syntax flags below are deliberately more conservative. */
#if defined(__GNUC__) && !defined(_MSC_VER)

# define HPS_HAS_GNU_EXTENSIONS 1

#else

# define HPS_HAS_GNU_EXTENSIONS 0

#endif

/* These GNU syntax flags require a recognized frontend and GNU language mode.
 * They grant no project-policy approval and enable no declaration macros. */
#if HPS_HAS_GNU_EXTENSIONS && !defined(__STRICT_ANSI__) && \
    (HPS_COMPILER_IS_GCC || HPS_COMPILER_IS_CLANG || \
     HPS_COMPILER_IS_ARMCLANG || HPS_COMPILER_IS_INTEL_LLVM)
# define HPS_HAS_EXTENSION_STATEMENT_EXPRESSIONS 1
# define HPS_HAS_EXTENSION_TYPEOF 1
# define HPS_HAS_EXTENSION_THREAD_LOCAL 1
# if HPS_LANGUAGE_IS_C
#  define HPS_HAS_EXTENSION_COMPOUND_LITERALS 1
#  define HPS_HAS_EXTENSION_DESIGNATED_INITIALIZERS 1
# else
#  define HPS_HAS_EXTENSION_COMPOUND_LITERALS 0
#  define HPS_HAS_EXTENSION_DESIGNATED_INITIALIZERS 0
# endif
#else
# define HPS_HAS_EXTENSION_STATEMENT_EXPRESSIONS 0
# define HPS_HAS_EXTENSION_TYPEOF 0
# define HPS_HAS_EXTENSION_COMPOUND_LITERALS 0
# define HPS_HAS_EXTENSION_DESIGNATED_INITIALIZERS 0
# if defined(_MSC_VER)
/* __declspec(thread) is distinct from the standard _Thread_local contract. */
#  define HPS_HAS_EXTENSION_THREAD_LOCAL 1
# else
#  define HPS_HAS_EXTENSION_THREAD_LOCAL 0
# endif
#endif

/* Compiler probes describe facilities, including in strict ISO modes. GCC is
 * the fallback only for the actual compiler identity, not GNU compatibility. */
#if HPS_HAS_BUILTIN(__builtin_expect) || HPS_COMPILER_IS_GCC
# define HPS_HAS_BUILTIN_EXPECT 1
#else
# define HPS_HAS_BUILTIN_EXPECT 0
#endif

/* These builtins use C type compatibility and expression rules. GCC does not
 * implement them in C++, even though its GNU compatibility macros are present. */
#if HPS_LANGUAGE_IS_C && \
    (HPS_HAS_BUILTIN(__builtin_types_compatible_p) || HPS_COMPILER_IS_GCC)
# define HPS_HAS_BUILTIN_TYPES_COMPATIBLE_P 1
#else
# define HPS_HAS_BUILTIN_TYPES_COMPATIBLE_P 0
#endif
#if HPS_LANGUAGE_IS_C && \
    (HPS_HAS_BUILTIN(__builtin_choose_expr) || HPS_COMPILER_IS_GCC)
# define HPS_HAS_BUILTIN_CHOOSE_EXPR 1
#else
# define HPS_HAS_BUILTIN_CHOOSE_EXPR 0
#endif

/* Preserve the legacy prerequisite combination as an informational flag only.
 * It does not establish lambda/nested-function support or provide a lambda API. */
#define HPS_HAS_LAMBDA_MACRO_SUPPORT \
    (HPS_LANGUAGE_IS_C && HPS_HAS_EXTENSION_STATEMENT_EXPRESSIONS && \
     HPS_HAS_EXTENSION_TYPEOF)

#endif /* HPS_BASE_FEATURES_COMPILER_H */
