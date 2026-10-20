#ifndef HPS_BASE_HPS_STDINT_H
#define HPS_BASE_HPS_STDINT_H

/**
 * @file hps/base/hps_stdint.h
 * @brief Standard integer names with a native header or checked compiler fallback.
 */

/* Prefer the native library even in legacy language modes when header probing
 * confirms it exists. Otherwise C99+, C++11+ and MSVC 2010+ use <stdint.h>.
 * HPS_STDINT_USE_FALLBACK=0 explicitly selects the native header; =1 selects the
 * replacement provider for a target without a usable native header.
 *
 * Replacement mode requires compiler standard-type, limit and constant metadata.
 * It must not be mixed with native <stdint.h>, <inttypes.h> or another provider,
 * including indirectly: compiler least/fast choices can differ from libc's ABI.
 * Select a single provider consistently across the complete program. Availability
 * does not qualify a compiler, library or target, and does not permit framework
 * declarations to use the least/fast/max/pointer families against project policy.
 */
#ifndef HPS_STDINT_USE_FALLBACK
# if defined(__has_include)
#  if __has_include(<stdint.h>)
#   define HPS_STDINT_USE_FALLBACK 0
#  else
#   define HPS_STDINT_USE_FALLBACK 1
#  endif
# elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || \
       (defined(__cplusplus) && (__cplusplus >= 201103L)) || \
       (defined(_MSC_VER) && (_MSC_VER >= 1600))
#  define HPS_STDINT_USE_FALLBACK 0
# else
#  define HPS_STDINT_USE_FALLBACK 1
# endif
#endif
#if (HPS_STDINT_USE_FALLBACK != 0) && (HPS_STDINT_USE_FALLBACK != 1)
# error "HPS_STDINT_USE_FALLBACK must be 0 or 1"
#endif

#include <limits.h>

#if HPS_STDINT_USE_FALLBACK == 0

# include <stdint.h>

#else

# if defined(INT8_MAX) || defined(UINT8_MAX) || defined(INTMAX_MAX)
#  error "HPS stdint fallback must not be mixed with another stdint provider"
# endif
# if CHAR_BIT != 8
#  error "HPS stdint fallback requires CHAR_BIT == 8"
# endif

/* Compiler metadata supplies the actual ABI types and promoted constant types;
 * no CPU-name, pointer-size or native-width guess selects an integer type.
 */
# if !defined(__INT8_TYPE__) || !defined(__UINT8_TYPE__) || \
     !defined(__INT8_MAX__) || !defined(__UINT8_MAX__)
#  error "HPS stdint fallback requires compiler INT8/UINT8 type and limit metadata"
# endif
# if !defined(__INT16_TYPE__) || !defined(__UINT16_TYPE__) || \
     !defined(__INT16_MAX__) || !defined(__UINT16_MAX__)
#  error "HPS stdint fallback requires compiler INT16/UINT16 type and limit metadata"
# endif
# if !defined(__INT32_TYPE__) || !defined(__UINT32_TYPE__) || \
     !defined(__INT32_MAX__) || !defined(__UINT32_MAX__)
#  error "HPS stdint fallback requires compiler INT32/UINT32 type and limit metadata"
# endif
# if !defined(__INT64_TYPE__) || !defined(__UINT64_TYPE__) || \
     !defined(__INT64_MAX__) || !defined(__UINT64_MAX__)
#  error "HPS stdint fallback requires compiler INT64/UINT64 type and limit metadata"
# endif
# if !defined(__INT_LEAST8_TYPE__) || !defined(__UINT_LEAST8_TYPE__) || \
     !defined(__INT_LEAST8_MAX__) || !defined(__UINT_LEAST8_MAX__)
#  error "HPS stdint fallback requires compiler INT_LEAST8/UINT_LEAST8 type and limit metadata"
# endif
# if !defined(__INT_LEAST16_TYPE__) || !defined(__UINT_LEAST16_TYPE__) || \
     !defined(__INT_LEAST16_MAX__) || !defined(__UINT_LEAST16_MAX__)
#  error "HPS stdint fallback requires compiler INT_LEAST16/UINT_LEAST16 type and limit metadata"
# endif
# if !defined(__INT_LEAST32_TYPE__) || !defined(__UINT_LEAST32_TYPE__) || \
     !defined(__INT_LEAST32_MAX__) || !defined(__UINT_LEAST32_MAX__)
#  error "HPS stdint fallback requires compiler INT_LEAST32/UINT_LEAST32 type and limit metadata"
# endif
# if !defined(__INT_LEAST64_TYPE__) || !defined(__UINT_LEAST64_TYPE__) || \
     !defined(__INT_LEAST64_MAX__) || !defined(__UINT_LEAST64_MAX__)
#  error "HPS stdint fallback requires compiler INT_LEAST64/UINT_LEAST64 type and limit metadata"
# endif
# if !defined(__INT_FAST8_TYPE__) || !defined(__UINT_FAST8_TYPE__) || \
     !defined(__INT_FAST8_MAX__) || !defined(__UINT_FAST8_MAX__)
#  error "HPS stdint fallback requires compiler INT_FAST8/UINT_FAST8 type and limit metadata"
# endif
# if !defined(__INT_FAST16_TYPE__) || !defined(__UINT_FAST16_TYPE__) || \
     !defined(__INT_FAST16_MAX__) || !defined(__UINT_FAST16_MAX__)
#  error "HPS stdint fallback requires compiler INT_FAST16/UINT_FAST16 type and limit metadata"
# endif
# if !defined(__INT_FAST32_TYPE__) || !defined(__UINT_FAST32_TYPE__) || \
     !defined(__INT_FAST32_MAX__) || !defined(__UINT_FAST32_MAX__)
#  error "HPS stdint fallback requires compiler INT_FAST32/UINT_FAST32 type and limit metadata"
# endif
# if !defined(__INT_FAST64_TYPE__) || !defined(__UINT_FAST64_TYPE__) || \
     !defined(__INT_FAST64_MAX__) || !defined(__UINT_FAST64_MAX__)
#  error "HPS stdint fallback requires compiler INT_FAST64/UINT_FAST64 type and limit metadata"
# endif
# if !defined(__INTMAX_TYPE__) || !defined(__UINTMAX_TYPE__) || \
     !defined(__INTMAX_MAX__) || !defined(__UINTMAX_MAX__)
#  error "HPS stdint fallback requires compiler INTMAX/UINTMAX type and limit metadata"
# endif

typedef __INT8_TYPE__ int8_t;
typedef __UINT8_TYPE__ uint8_t;
# define INT8_MIN (-__INT8_MAX__ - 1)
# define INT8_MAX __INT8_MAX__
# define UINT8_MAX __UINT8_MAX__

typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;
# define INT16_MIN (-__INT16_MAX__ - 1)
# define INT16_MAX __INT16_MAX__
# define UINT16_MAX __UINT16_MAX__

typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
# define INT32_MIN (-__INT32_MAX__ - 1)
# define INT32_MAX __INT32_MAX__
# define UINT32_MAX __UINT32_MAX__

typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;
# define INT64_MIN (-__INT64_MAX__ - 1)
# define INT64_MAX __INT64_MAX__
# define UINT64_MAX __UINT64_MAX__

typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __UINT_LEAST8_TYPE__ uint_least8_t;
# define INT_LEAST8_MIN (-__INT_LEAST8_MAX__ - 1)
# define INT_LEAST8_MAX __INT_LEAST8_MAX__
# define UINT_LEAST8_MAX __UINT_LEAST8_MAX__

typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
# define INT_LEAST16_MIN (-__INT_LEAST16_MAX__ - 1)
# define INT_LEAST16_MAX __INT_LEAST16_MAX__
# define UINT_LEAST16_MAX __UINT_LEAST16_MAX__

typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
# define INT_LEAST32_MIN (-__INT_LEAST32_MAX__ - 1)
# define INT_LEAST32_MAX __INT_LEAST32_MAX__
# define UINT_LEAST32_MAX __UINT_LEAST32_MAX__

typedef __INT_LEAST64_TYPE__ int_least64_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;
# define INT_LEAST64_MIN (-__INT_LEAST64_MAX__ - 1)
# define INT_LEAST64_MAX __INT_LEAST64_MAX__
# define UINT_LEAST64_MAX __UINT_LEAST64_MAX__

typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __UINT_FAST8_TYPE__ uint_fast8_t;
# define INT_FAST8_MIN (-__INT_FAST8_MAX__ - 1)
# define INT_FAST8_MAX __INT_FAST8_MAX__
# define UINT_FAST8_MAX __UINT_FAST8_MAX__

typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
# define INT_FAST16_MIN (-__INT_FAST16_MAX__ - 1)
# define INT_FAST16_MAX __INT_FAST16_MAX__
# define UINT_FAST16_MAX __UINT_FAST16_MAX__

typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
# define INT_FAST32_MIN (-__INT_FAST32_MAX__ - 1)
# define INT_FAST32_MAX __INT_FAST32_MAX__
# define UINT_FAST32_MAX __UINT_FAST32_MAX__

typedef __INT_FAST64_TYPE__ int_fast64_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;
# define INT_FAST64_MIN (-__INT_FAST64_MAX__ - 1)
# define INT_FAST64_MAX __INT_FAST64_MAX__
# define UINT_FAST64_MAX __UINT_FAST64_MAX__

typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;
# define INTMAX_MIN (-__INTMAX_MAX__ - 1)
# define INTMAX_MAX __INTMAX_MAX__
# define UINTMAX_MAX __UINTMAX_MAX__

/* Pointer-sized integers are optional in the standard. Publish a pair only
 * when the compiler supplies both underlying types and both limits.
 */
# if defined(__INTPTR_TYPE__) && defined(__UINTPTR_TYPE__) && \
     defined(__INTPTR_MAX__) && defined(__UINTPTR_MAX__)
typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
#  define INTPTR_MIN (-__INTPTR_MAX__ - 1)
#  define INTPTR_MAX __INTPTR_MAX__
#  define UINTPTR_MAX __UINTPTR_MAX__
# endif

/* GCC supplies constant builders; Clang supplies suffix metadata. Keep the
 * promoted types specified by the compiler, including unsigned 8/16 constants
 * that promote to signed int on ordinary 32-bit-int targets. All results remain
 * usable in preprocessor expressions; no cast replaces a standard _C macro.
 * Constants whose ranges fit int avoid Clang's empty suffix as a macro argument,
 * unavailable in C89. Signed constants add compiler-typed zero to preserve the
 * promoted type even when int and long have equal ranges. Unsigned types whose
 * entire range fits int necessarily promote to int and need only the literal.
 */
# define HPS_DETAIL_STDINT_JOIN_RAW(value_, suffix_) value_##suffix_
# define HPS_DETAIL_STDINT_JOIN(value_, suffix_) \
    HPS_DETAIL_STDINT_JOIN_RAW(value_, suffix_)
# if defined(__INT8_C)
#  define INT8_C(value_) __INT8_C(value_)
# elif defined(__INT8_C_SUFFIX__)
#  if __INT8_MAX__ <= INT_MAX
#   define INT8_C(value_) ((value_) + (__INT8_MAX__ - __INT8_MAX__))
#  else
#   define INT8_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __INT8_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler INT8_C constant metadata"
# endif
# if defined(__UINT8_C)
#  define UINT8_C(value_) __UINT8_C(value_)
# elif defined(__UINT8_C_SUFFIX__)
#  if __UINT8_MAX__ <= INT_MAX
#   define UINT8_C(value_) value_
#  else
#   define UINT8_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __UINT8_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler UINT8_C constant metadata"
# endif
# if defined(__INT16_C)
#  define INT16_C(value_) __INT16_C(value_)
# elif defined(__INT16_C_SUFFIX__)
#  if __INT16_MAX__ <= INT_MAX
#   define INT16_C(value_) ((value_) + (__INT16_MAX__ - __INT16_MAX__))
#  else
#   define INT16_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __INT16_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler INT16_C constant metadata"
# endif
# if defined(__UINT16_C)
#  define UINT16_C(value_) __UINT16_C(value_)
# elif defined(__UINT16_C_SUFFIX__)
#  if __UINT16_MAX__ <= INT_MAX
#   define UINT16_C(value_) value_
#  else
#   define UINT16_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __UINT16_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler UINT16_C constant metadata"
# endif
# if defined(__INT32_C)
#  define INT32_C(value_) __INT32_C(value_)
# elif defined(__INT32_C_SUFFIX__)
#  if __INT32_MAX__ <= INT_MAX
#   define INT32_C(value_) ((value_) + (__INT32_MAX__ - __INT32_MAX__))
#  else
#   define INT32_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __INT32_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler INT32_C constant metadata"
# endif
# if defined(__UINT32_C)
#  define UINT32_C(value_) __UINT32_C(value_)
# elif defined(__UINT32_C_SUFFIX__)
#  if __UINT32_MAX__ <= INT_MAX
#   define UINT32_C(value_) value_
#  else
#   define UINT32_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __UINT32_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler UINT32_C constant metadata"
# endif
# if defined(__INT64_C)
#  define INT64_C(value_) __INT64_C(value_)
# elif defined(__INT64_C_SUFFIX__)
#  if __INT64_MAX__ <= INT_MAX
#   define INT64_C(value_) ((value_) + (__INT64_MAX__ - __INT64_MAX__))
#  else
#   define INT64_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __INT64_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler INT64_C constant metadata"
# endif
# if defined(__UINT64_C)
#  define UINT64_C(value_) __UINT64_C(value_)
# elif defined(__UINT64_C_SUFFIX__)
#  if __UINT64_MAX__ <= INT_MAX
#   define UINT64_C(value_) value_
#  else
#   define UINT64_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __UINT64_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler UINT64_C constant metadata"
# endif
# if defined(__INTMAX_C)
#  define INTMAX_C(value_) __INTMAX_C(value_)
# elif defined(__INTMAX_C_SUFFIX__)
#  if __INTMAX_MAX__ <= INT_MAX
#   define INTMAX_C(value_) ((value_) + (__INTMAX_MAX__ - __INTMAX_MAX__))
#  else
#   define INTMAX_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __INTMAX_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler INTMAX_C constant metadata"
# endif
# if defined(__UINTMAX_C)
#  define UINTMAX_C(value_) __UINTMAX_C(value_)
# elif defined(__UINTMAX_C_SUFFIX__)
#  if __UINTMAX_MAX__ <= INT_MAX
#   define UINTMAX_C(value_) value_
#  else
#   define UINTMAX_C(value_) HPS_DETAIL_STDINT_JOIN(value_, __UINTMAX_C_SUFFIX__)
#  endif
# else
#  error "HPS stdint fallback requires compiler UINTMAX_C constant metadata"
# endif

/* Other standard limit macros are published only when compiler metadata
 * identifies their bounds. They introduce no additional typedefs.
 */
# if defined(__SIZE_MAX__) && !defined(SIZE_MAX)
#  define SIZE_MAX __SIZE_MAX__
# endif
# if defined(__PTRDIFF_MAX__) && !defined(PTRDIFF_MAX)
#  define PTRDIFF_MAX __PTRDIFF_MAX__
#  define PTRDIFF_MIN (-__PTRDIFF_MAX__ - 1)
# endif
# if defined(__SIG_ATOMIC_MAX__) && !defined(SIG_ATOMIC_MAX)
#  if defined(__SIG_ATOMIC_MIN__)
#   define SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#   define SIG_ATOMIC_MIN __SIG_ATOMIC_MIN__
#  elif defined(__SIG_ATOMIC_UNSIGNED__)
#   define SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#   define SIG_ATOMIC_MIN (__SIG_ATOMIC_MAX__ - __SIG_ATOMIC_MAX__)
#  endif
# endif
# if defined(__WCHAR_MAX__) && !defined(WCHAR_MAX)
#  define WCHAR_MAX __WCHAR_MAX__
#  if defined(__WCHAR_MIN__)
#   define WCHAR_MIN __WCHAR_MIN__
#  elif defined(__WCHAR_UNSIGNED__)
#   define WCHAR_MIN (__WCHAR_MAX__ - __WCHAR_MAX__)
#  else
#   define WCHAR_MIN (-__WCHAR_MAX__ - 1)
#  endif
# endif
# if defined(__WINT_MAX__) && !defined(WINT_MAX)
#  define WINT_MAX __WINT_MAX__
#  if defined(__WINT_MIN__)
#   define WINT_MIN __WINT_MIN__
#  elif defined(__WINT_UNSIGNED__)
#   define WINT_MIN (__WINT_MAX__ - __WINT_MAX__)
#  else
#   define WINT_MIN (-__WINT_MAX__ - 1)
#  endif
# endif

#endif /* HPS_STDINT_USE_FALLBACK */

/* HPS requires these exact widths. Reject missing types or inconsistent limits
 * instead of silently substituting larger integers. Compiler/library-provided
 * exact signed integer types have the standard two's-complement representation.
 */
#if !defined(INT8_MIN) || !defined(INT8_MAX) || !defined(UINT8_MAX) || \
    !defined(INT16_MIN) || !defined(INT16_MAX) || !defined(UINT16_MAX) || \
    !defined(INT32_MIN) || !defined(INT32_MAX) || !defined(UINT32_MAX) || \
    !defined(INT64_MIN) || !defined(INT64_MAX) || !defined(UINT64_MAX)
# error "HPS requires standard exact-width 8, 16, 32 and 64-bit integers"
#endif
#if CHAR_BIT != 8
# error "HPS requires CHAR_BIT == 8"
#endif
#if (INT8_MAX != INT8_C(127)) || \
    (INT8_MIN != (-INT8_C(127) - 1)) || \
    (UINT8_MAX != UINT8_C(255))
# error "HPS requires exact 8-bit integer limits"
#endif
#if (INT16_MAX != INT16_C(32767)) || \
    (INT16_MIN != (-INT16_C(32767) - 1)) || \
    (UINT16_MAX != UINT16_C(65535))
# error "HPS requires exact 16-bit integer limits"
#endif
#if (INT32_MAX != INT32_C(2147483647)) || \
    (INT32_MIN != (-INT32_C(2147483647) - 1)) || \
    (UINT32_MAX != UINT32_C(4294967295))
# error "HPS requires exact 32-bit integer limits"
#endif
#if (INT64_MAX != INT64_C(9223372036854775807)) || \
    (INT64_MIN != (-INT64_C(9223372036854775807) - 1)) || \
    (UINT64_MAX != UINT64_C(18446744073709551615))
# error "HPS requires exact 64-bit integer limits"
#endif

/* C89-compatible compile-time checks also reject padding or signedness errors.
 * These declarations create no objects, runtime state or storage budget.
 */
typedef uint8_t hps_detail_stdint_exact8_check[
    ((sizeof(int8_t) * CHAR_BIT == 8) &&
     (sizeof(uint8_t) * CHAR_BIT == 8) &&
     ((int8_t)-1 < (int8_t)0) &&
     ((uint8_t)-1 > (uint8_t)0)) ? 1 : -1];
typedef uint8_t hps_detail_stdint_exact16_check[
    ((sizeof(int16_t) * CHAR_BIT == 16) &&
     (sizeof(uint16_t) * CHAR_BIT == 16) &&
     ((int16_t)-1 < (int16_t)0) &&
     ((uint16_t)-1 > (uint16_t)0)) ? 1 : -1];
typedef uint8_t hps_detail_stdint_exact32_check[
    ((sizeof(int32_t) * CHAR_BIT == 32) &&
     (sizeof(uint32_t) * CHAR_BIT == 32) &&
     ((int32_t)-1 < (int32_t)0) &&
     ((uint32_t)-1 > (uint32_t)0)) ? 1 : -1];
typedef uint8_t hps_detail_stdint_exact64_check[
    ((sizeof(int64_t) * CHAR_BIT == 64) &&
     (sizeof(uint64_t) * CHAR_BIT == 64) &&
     ((int64_t)-1 < (int64_t)0) &&
     ((uint64_t)-1 > (uint64_t)0)) ? 1 : -1];

#endif /* HPS_BASE_HPS_STDINT_H */
