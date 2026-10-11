#ifndef HPS_BASE_COMPILER_H
#define HPS_BASE_COMPILER_H

#define HPS_COMPILER_UNKNOWN 0
#define HPS_COMPILER_GCC 1
#define HPS_COMPILER_CLANG 2
#define HPS_COMPILER_MSVC 3
#define HPS_COMPILER_ARMCLANG 4

/* Test derivatives before the compatibility macros they also define. */
#if defined(__clang__) && defined(__ARMCOMPILER_VERSION)
# define HPS_COMPILER HPS_COMPILER_ARMCLANG
# define HPS_COMPILER_NAME "Arm Compiler for Embedded (Clang)"
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

#if HPS_COMPILER_IS_CLANG || HPS_COMPILER_IS_ARMCLANG
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
#else
# define HPS_COMPILER_VERSION_MAJOR 0
# define HPS_COMPILER_VERSION_MINOR 0
# define HPS_COMPILER_VERSION_PATCH 0
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
