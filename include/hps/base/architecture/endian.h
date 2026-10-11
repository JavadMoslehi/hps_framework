#ifndef HPS_BASE_ARCHITECTURE_ENDIAN_H
#define HPS_BASE_ARCHITECTURE_ENDIAN_H

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

#elif defined(_WIN32)

# define HPS_ENDIANNESS HPS_ENDIAN_LITTLE

#else

# define HPS_ENDIANNESS HPS_ENDIAN_UNKNOWN

#endif


#endif /* HPS_BASE_ARCHITECTURE_ENDIAN_H */
