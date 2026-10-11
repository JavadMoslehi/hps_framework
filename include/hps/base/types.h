#ifndef HPS_BASE_TYPES_H
#define HPS_BASE_TYPES_H

#include "hps/base/standard.h"

#if !HPS_C_IS_C99
# error "HPS base types require C99 or newer"
#else

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

/* A byte is a C byte (CHAR_BIT bits), not an assumed eight-bit octet. */
typedef unsigned char hps_byte;
typedef _Bool hps_bool;
typedef size_t hps_size;
typedef ptrdiff_t hps_ssize;
typedef float hps_float;
typedef double hps_double;
/* Floating-point representation is implementation-defined. */

/* Exact-width integers are optional in C99, so expose availability flags. */
#if defined(INT8_MAX) && defined(UINT8_MAX)
# define HPS_HAS_INT8 1
typedef int8_t hps_i8;
typedef uint8_t hps_u8;
#else
# define HPS_HAS_INT8 0
#endif
#if defined(INT16_MAX) && defined(UINT16_MAX)
# define HPS_HAS_INT16 1
typedef int16_t hps_i16;
typedef uint16_t hps_u16;
#else
# define HPS_HAS_INT16 0
#endif
#if defined(INT32_MAX) && defined(UINT32_MAX)
# define HPS_HAS_INT32 1
typedef int32_t hps_i32;
typedef uint32_t hps_u32;
#else
# define HPS_HAS_INT32 0
#endif
#if defined(INT64_MAX) && defined(UINT64_MAX)
# define HPS_HAS_INT64 1
typedef int64_t hps_i64;
typedef uint64_t hps_u64;
#else
# define HPS_HAS_INT64 0
#endif

#if defined(INTPTR_MAX) && defined(UINTPTR_MAX)
# define HPS_HAS_INTPTR 1
typedef intptr_t hps_iptr;
typedef uintptr_t hps_uptr;
#else
# define HPS_HAS_INTPTR 0
#endif

typedef int_least8_t hps_i_least8;
typedef uint_least8_t hps_u_least8;
typedef int_least16_t hps_i_least16;
typedef uint_least16_t hps_u_least16;
typedef int_least32_t hps_i_least32;
typedef uint_least32_t hps_u_least32;
typedef int_least64_t hps_i_least64;
typedef uint_least64_t hps_u_least64;
typedef intmax_t hps_imax;
typedef uintmax_t hps_umax;

#define HPS_TRUE ((_Bool)1)
#define HPS_FALSE ((_Bool)0)
#define HPS_NULL NULL
#define HPS_BITS_PER_BYTE CHAR_BIT

#endif /* C99 */
#endif /* HPS_BASE_TYPES_H */
