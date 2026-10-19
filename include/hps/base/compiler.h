#ifndef HPS_BASE_COMPILER_H
#define HPS_BASE_COMPILER_H

/**
 * @file hps/base/compiler.h
 * @brief Compiler identity, version and capability probes.
 */

#define HPS_COMPILER_UNKNOWN 0
#define HPS_COMPILER_GCC 1
#define HPS_COMPILER_CLANG 2
#define HPS_COMPILER_MSVC 3
#define HPS_COMPILER_ARMCLANG 4
#define HPS_COMPILER_INTEL_CLASSIC 5
#define HPS_COMPILER_INTEL_LLVM 6
#define HPS_COMPILER_IAR 7
#define HPS_COMPILER_ARMCC 8
#define HPS_COMPILER_TINYCC 9

/* Test derivatives before the compatibility macros they also define. */
#if defined(__ARMCOMPILER_VERSION)
# define HPS_COMPILER HPS_COMPILER_ARMCLANG
# define HPS_COMPILER_NAME "Arm Compiler for Embedded (Clang)"
#elif defined(__INTEL_LLVM_COMPILER)
# define HPS_COMPILER HPS_COMPILER_INTEL_LLVM
# define HPS_COMPILER_NAME "Intel LLVM"
#elif defined(__ICC) || defined(__ICL) || defined(__INTEL_COMPILER)
# define HPS_COMPILER HPS_COMPILER_INTEL_CLASSIC
# define HPS_COMPILER_NAME "Intel Classic"
#elif defined(__IAR_SYSTEMS_ICC__) || defined(__ICCARM__) || \
      defined(__ICCRX__) || defined(__ICCAVR__) || defined(__ICAVR__)
# define HPS_COMPILER HPS_COMPILER_IAR
# define HPS_COMPILER_NAME "IAR"
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
# define HPS_COMPILER HPS_COMPILER_ARMCC
# define HPS_COMPILER_NAME "Arm Compiler (armcc)"
#elif defined(__TINYC__)
# define HPS_COMPILER HPS_COMPILER_TINYCC
# define HPS_COMPILER_NAME "TinyCC"
#elif defined(__clang__)
# define HPS_COMPILER HPS_COMPILER_CLANG
# define HPS_COMPILER_NAME "Clang"
#elif defined(_MSC_VER)
# define HPS_COMPILER HPS_COMPILER_MSVC
# define HPS_COMPILER_NAME "MSVC"
#elif defined(__GNUC__) && !defined(__INTEL_COMPILER) && \
      !defined(__INTEL_LLVM_COMPILER) && !defined(__NVCOMPILER) && \
      !defined(__CUDACC__) && !defined(__ibmxl__) && !defined(__ARMCC_VERSION)
# define HPS_COMPILER HPS_COMPILER_GCC
# define HPS_COMPILER_NAME "GCC"
#else
# define HPS_COMPILER HPS_COMPILER_UNKNOWN
# define HPS_COMPILER_NAME "unknown"
#endif

#define HPS_COMPILER_IS_GCC (HPS_COMPILER == HPS_COMPILER_GCC)
#define HPS_COMPILER_IS_CLANG (HPS_COMPILER == HPS_COMPILER_CLANG)
#define HPS_COMPILER_IS_MSVC (HPS_COMPILER == HPS_COMPILER_MSVC)
#define HPS_COMPILER_IS_ARMCLANG (HPS_COMPILER == HPS_COMPILER_ARMCLANG)
#define HPS_COMPILER_IS_INTEL_CLASSIC \
    (HPS_COMPILER == HPS_COMPILER_INTEL_CLASSIC)
#define HPS_COMPILER_IS_INTEL_LLVM (HPS_COMPILER == HPS_COMPILER_INTEL_LLVM)
#define HPS_COMPILER_IS_IAR (HPS_COMPILER == HPS_COMPILER_IAR)
#define HPS_COMPILER_IS_ARMCC (HPS_COMPILER == HPS_COMPILER_ARMCC)
#define HPS_COMPILER_IS_TINYCC (HPS_COMPILER == HPS_COMPILER_TINYCC)
#define HPS_COMPILER_IS_UNKNOWN (HPS_COMPILER == HPS_COMPILER_UNKNOWN)

/* Keep frontend versions for Clang derivatives, including Armclang and Intel
 * LLVM; their vendor release number is a different version contract. */
#if defined(__clang__) && \
    (HPS_COMPILER_IS_CLANG || HPS_COMPILER_IS_ARMCLANG || \
     HPS_COMPILER_IS_INTEL_LLVM)
# define HPS_COMPILER_VERSION_MAJOR __clang_major__
# define HPS_COMPILER_VERSION_MINOR __clang_minor__
# define HPS_COMPILER_VERSION_PATCH __clang_patchlevel__
#elif HPS_COMPILER_IS_GCC
# define HPS_COMPILER_VERSION_MAJOR __GNUC__
# define HPS_COMPILER_VERSION_MINOR __GNUC_MINOR__
# ifdef __GNUC_PATCHLEVEL__
#  define HPS_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
# else
#  define HPS_COMPILER_VERSION_PATCH 0
# endif
#elif HPS_COMPILER_IS_MSVC
# define HPS_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
# define HPS_COMPILER_VERSION_MINOR (_MSC_VER % 100)
# define HPS_COMPILER_VERSION_PATCH 0
#elif HPS_COMPILER_IS_INTEL_CLASSIC && defined(__INTEL_COMPILER)
# define HPS_COMPILER_VERSION_MAJOR (__INTEL_COMPILER / 100)
# define HPS_COMPILER_VERSION_MINOR (__INTEL_COMPILER % 100)
# define HPS_COMPILER_VERSION_PATCH 0
#elif HPS_COMPILER_IS_ARMCC && defined(__ARMCC_VERSION)
/* armcc publishes PVVbbbb; the third field is a build number. */
# define HPS_COMPILER_VERSION_MAJOR (__ARMCC_VERSION / 1000000)
# define HPS_COMPILER_VERSION_MINOR ((__ARMCC_VERSION / 10000) % 100)
# define HPS_COMPILER_VERSION_PATCH (__ARMCC_VERSION % 10000)
#else
/* IAR encodings vary by family. Preserve the raw release below instead of
 * guessing a common encoding; TinyCC and unversioned identities also use 0. */
# define HPS_COMPILER_VERSION_MAJOR 0
# define HPS_COMPILER_VERSION_MINOR 0
# define HPS_COMPILER_VERSION_PATCH 0
#endif

/* Raw vendor release, when available; do not compare different vendors' values.
 * This does not change the existing frontend version fields above. */
#if HPS_COMPILER_IS_ARMCLANG
# define HPS_COMPILER_VENDOR_VERSION __ARMCOMPILER_VERSION
#elif HPS_COMPILER_IS_INTEL_LLVM
# define HPS_COMPILER_VENDOR_VERSION __INTEL_LLVM_COMPILER
#elif HPS_COMPILER_IS_INTEL_CLASSIC && defined(__INTEL_COMPILER)
# define HPS_COMPILER_VENDOR_VERSION __INTEL_COMPILER
#elif HPS_COMPILER_IS_IAR && defined(__VER__)
# define HPS_COMPILER_VENDOR_VERSION __VER__
#elif HPS_COMPILER_IS_ARMCC && defined(__ARMCC_VERSION)
# define HPS_COMPILER_VENDOR_VERSION __ARMCC_VERSION
#elif HPS_COMPILER_IS_TINYCC
# define HPS_COMPILER_VENDOR_VERSION __TINYC__
#elif HPS_COMPILER_IS_MSVC && defined(_MSC_FULL_VER)
# define HPS_COMPILER_VENDOR_VERSION _MSC_FULL_VER
#elif HPS_COMPILER_IS_MSVC
# define HPS_COMPILER_VENDOR_VERSION _MSC_VER
#else
# define HPS_COMPILER_VENDOR_VERSION 0
#endif

#ifdef __has_attribute
# define HPS_HAS_ATTRIBUTE(name) __has_attribute(name)
#else
# define HPS_HAS_ATTRIBUTE(name) 0
#endif
#ifdef __has_builtin
# define HPS_HAS_BUILTIN(name) __has_builtin(name)
#else
# define HPS_HAS_BUILTIN(name) 0
#endif
#ifdef __has_feature
# define HPS_HAS_FEATURE(name) __has_feature(name)
#else
# define HPS_HAS_FEATURE(name) 0
#endif

#endif /* HPS_BASE_COMPILER_H */
