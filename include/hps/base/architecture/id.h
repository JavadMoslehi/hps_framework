#ifndef HPS_BASE_ARCHITECTURE_ID_H
#define HPS_BASE_ARCHITECTURE_ID_H

/**
 * @file hps/base/architecture/id.h
 * @brief Target processor architecture identifiers and detection.
 */

/*
 * HPS - General Purpose C Framework
 *
 * CPU architecture identifiers.
 */


/* --------------------------------------------------------------------------
 * Architecture identifiers
 * -------------------------------------------------------------------------- */

#define HPS_ARCH_UNKNOWN   0

#define HPS_ARCH_X86       1
#define HPS_ARCH_X86_64    2

#define HPS_ARCH_ARM32     3
#define HPS_ARCH_AARCH64   4

#define HPS_ARCH_RISCV32   5
#define HPS_ARCH_RISCV64   6


/* --------------------------------------------------------------------------
 * Architecture detection
 * -------------------------------------------------------------------------- */

#if defined(__x86_64__) || defined(_M_X64)

# define HPS_ARCHITECTURE HPS_ARCH_X86_64

#elif defined(__i386__) || defined(_M_IX86)

# define HPS_ARCHITECTURE HPS_ARCH_X86

#elif defined(__aarch64__) || defined(_M_ARM64)

# define HPS_ARCHITECTURE HPS_ARCH_AARCH64

#elif defined(__arm__) || defined(_M_ARM)

# define HPS_ARCHITECTURE HPS_ARCH_ARM32

#elif defined(__riscv)

# if defined(__riscv_xlen)

#  if (__riscv_xlen == 64)

#   define HPS_ARCHITECTURE HPS_ARCH_RISCV64

#  elif (__riscv_xlen == 32)

#   define HPS_ARCHITECTURE HPS_ARCH_RISCV32

#  else

#   define HPS_ARCHITECTURE HPS_ARCH_UNKNOWN

#  endif

# else

#  define HPS_ARCHITECTURE HPS_ARCH_UNKNOWN

# endif

#else

# define HPS_ARCHITECTURE HPS_ARCH_UNKNOWN

#endif


#endif /* HPS_BASE_ARCHITECTURE_ID_H */
