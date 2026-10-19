#ifndef HPS_BASE_FEATURES_ARCHITECTURE_H
#define HPS_BASE_FEATURES_ARCHITECTURE_H

/**
 * @file hps/base/features/architecture.h
 * @brief Target identity, data-model and enabled x86 instruction feature flags.
 */

#include "hps/base/architecture.h"

/* Architecture identity alone does not promise SIMD or unaligned access. */
#define HPS_ARCHITECTURE_IS_KNOWN (HPS_ARCHITECTURE != HPS_ARCH_UNKNOWN)
#define HPS_POINTER_WIDTH_IS_KNOWN (HPS_POINTER_BITS != 0)
#define HPS_ENDIANNESS_IS_KNOWN (HPS_ENDIANNESS != HPS_ENDIAN_UNKNOWN)
#define HPS_IS_LITTLE_ENDIAN (HPS_ENDIANNESS == HPS_ENDIAN_LITTLE)
#define HPS_IS_BIG_ENDIAN (HPS_ENDIANNESS == HPS_ENDIAN_BIG)

/* Compile-target evidence only: these 0/1 flags report predefined instruction
 * macros for this translation unit and its options. They do not query hardware,
 * CPUID, OS register-state support or permission to execute these instructions.
 * Zero means no positive target evidence; it does not prove hardware absence.
 * No instruction, runtime dispatch or cryptographic implementation is provided.
 * x86 identity and each individual macro are required independently; AVX2 does
 * not imply AVX here unless the compiler also defines __AVX__. Other families
 * remain zero even when unrelated/stray instruction macros are visible. */
#if ((HPS_ARCHITECTURE == HPS_ARCH_X86) || \
     (HPS_ARCHITECTURE == HPS_ARCH_X86_64)) && defined(__AES__)
# define HPS_TARGET_HAS_X86_AESNI 1
#else
# define HPS_TARGET_HAS_X86_AESNI 0
#endif

#if ((HPS_ARCHITECTURE == HPS_ARCH_X86) || \
     (HPS_ARCHITECTURE == HPS_ARCH_X86_64)) && defined(__PCLMUL__)
# define HPS_TARGET_HAS_X86_PCLMUL 1
#else
# define HPS_TARGET_HAS_X86_PCLMUL 0
#endif

#if ((HPS_ARCHITECTURE == HPS_ARCH_X86) || \
     (HPS_ARCHITECTURE == HPS_ARCH_X86_64)) && defined(__AVX__)
# define HPS_TARGET_HAS_X86_AVX 1
#else
# define HPS_TARGET_HAS_X86_AVX 0
#endif

#if ((HPS_ARCHITECTURE == HPS_ARCH_X86) || \
     (HPS_ARCHITECTURE == HPS_ARCH_X86_64)) && defined(__AVX2__)
# define HPS_TARGET_HAS_X86_AVX2 1
#else
# define HPS_TARGET_HAS_X86_AVX2 0
#endif

#if ((HPS_ARCHITECTURE == HPS_ARCH_X86) || \
     (HPS_ARCHITECTURE == HPS_ARCH_X86_64)) && defined(__SHA__)
# define HPS_TARGET_HAS_X86_SHA 1
#else
# define HPS_TARGET_HAS_X86_SHA 0
#endif

#endif /* HPS_BASE_FEATURES_ARCHITECTURE_H */
