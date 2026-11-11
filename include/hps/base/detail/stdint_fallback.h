
/*
 * HPS Base Internal Header
 *
 * Direct inclusion by applications is prohibited.
 *
 * This header may only be included:
 *   1. Through an authorized public HPS Base header.
 *   2. During compilation of the HPS Base module.
 */

#if !defined(HPS_BASE_INSIDE) && !defined(HPS_BASE_COMPILATION)

#error "This HPS internal header cannot be included directly."

#endif

#ifndef HPS_BASE_DETAIL_STDINT_FALLBACK_H
#define HPS_BASE_DETAIL_STDINT_FALLBACK_H


/*
 * HPS Legacy Fixed-Width Integer Fallback
 *
 * This is a restricted fallback for older toolchains.
 *
 * Supported types:
 *   int8_t,  uint8_t
 *   int16_t, uint16_t
 *   int32_t, uint32_t
 *
 * No 64-bit integer fallback is provided yet.
 *
 * Requires eight-bit bytes and exact type widths.
 */

#include <limits.h>

/* ----------------------------------------------------------
 * 1. Eight-bit types
 * ---------------------------------------------------------- */

#if CHAR_BIT != 8 || UCHAR_MAX != 255U || \
    SCHAR_MIN != (-127 - 1) || SCHAR_MAX != 127

#error "HPS: Exact 8-bit integer types unavailable"

#else

typedef signed char int8_t;
typedef unsigned char uint8_t;

#endif


/* ----------------------------------------------------------
 * 2. Sixteen-bit types
 * ---------------------------------------------------------- */

#if USHRT_MAX == 65535U && \
    SHRT_MIN == (-32767 - 1) && SHRT_MAX == 32767

typedef signed short int16_t;
typedef unsigned short uint16_t;

#elif UINT_MAX == 65535U && \
      INT_MIN == (-32767 - 1) && INT_MAX == 32767

typedef signed int int16_t;
typedef unsigned int uint16_t;

#else

#error "HPS: Exact 16-bit integer types unavailable"

#endif


/* ----------------------------------------------------------
 * 3. Thirty-two-bit types
 * ---------------------------------------------------------- */

#if UINT_MAX == 4294967295UL && \
    INT_MIN == (-2147483647L - 1L) && \
    INT_MAX == 2147483647L

typedef signed int int32_t;
typedef unsigned int uint32_t;

#elif ULONG_MAX == 4294967295UL && \
      LONG_MIN == (-2147483647L - 1L) && \
      LONG_MAX == 2147483647L

typedef signed long int32_t;
typedef unsigned long uint32_t;

#else

#error "HPS: Exact 32-bit integer types unavailable"

#endif

#endif /* HPS_BASE_DETAIL_STDINT_FALLBACK_H */
