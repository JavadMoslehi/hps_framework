#ifndef HPS_BASE_ARCHITECTURE_ENDIAN_H
#define HPS_BASE_ARCHITECTURE_ENDIAN_H

/**
 * @file hps/base/architecture/endian.h
 * @brief Target byte-order identifiers and detection.
 */

/*
 * HPS - General Purpose C Framework
 *
 * Target byte-order detection.
 */


/* --------------------------------------------------------------------------
 * Endianness identifiers
 * -------------------------------------------------------------------------- */

#define HPS_ENDIAN_UNKNOWN  0
#define HPS_ENDIAN_LITTLE  1
#define HPS_ENDIAN_BIG     2


/* --------------------------------------------------------------------------
 * Endianness detection
 * -------------------------------------------------------------------------- */

#if defined(__BYTE_ORDER__) && \
    defined(__ORDER_LITTLE_ENDIAN__) && \
    (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

# define HPS_ENDIANNESS HPS_ENDIAN_LITTLE

#elif defined(__BYTE_ORDER__) && \
      defined(__ORDER_BIG_ENDIAN__) && \
      (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

# define HPS_ENDIANNESS HPS_ENDIAN_BIG

#elif defined(__BYTE_ORDER__)

/* Preserve unsupported/mixed byte orders instead of overriding direct evidence. */
# define HPS_ENDIANNESS HPS_ENDIAN_UNKNOWN

#elif defined(_WIN32) || defined(__i386__) || defined(_M_IX86) || \
      defined(__x86_64__) || defined(_M_X64)

# define HPS_ENDIANNESS HPS_ENDIAN_LITTLE

#else

# define HPS_ENDIANNESS HPS_ENDIAN_UNKNOWN

#endif

/* ARM, RISC-V, MIPS and PowerPC identity alone does not select byte order. */


#endif /* HPS_BASE_ARCHITECTURE_ENDIAN_H */
