#ifndef HPS_BASE_ATTRIBUTES_H
#define HPS_BASE_ATTRIBUTES_H

#include "hps/base/features.h"

#define HPS_DETAIL_JOIN_RAW(a, b) a##b
#define HPS_DETAIL_JOIN(a, b) HPS_DETAIL_JOIN_RAW(a, b)
#define HPS_UNUSED(value) ((void)(value))

#if HPS_C_IS_C23
# define HPS_STATIC_ASSERT(condition, message) static_assert(condition, message)
#elif HPS_HAS_STATIC_ASSERT
# define HPS_STATIC_ASSERT(condition, message) _Static_assert(condition, message)
#else
/* The condition must be an integer constant expression. */
# define HPS_STATIC_ASSERT(condition, message) \
    typedef char HPS_DETAIL_JOIN(hps_static_assert_line_, __LINE__)[(condition) ? 1 : -1]
#endif

#if HPS_C_IS_C99
# define HPS_INLINE inline
# define HPS_RESTRICT restrict
#else
# define HPS_INLINE
# define HPS_RESTRICT
#endif

#if HPS_HAS_ATTRIBUTE(noreturn)
# define HPS_NORETURN __attribute__((__noreturn__))
# define HPS_HAS_NORETURN 1
#elif HPS_C_IS_C23
# define HPS_NORETURN [[noreturn]]
# define HPS_HAS_NORETURN 1
#elif HPS_HAS_C11_FEATURES
# define HPS_NORETURN _Noreturn
# define HPS_HAS_NORETURN 1
#elif HPS_COMPILER_IS_MSVC
# define HPS_NORETURN __declspec(noreturn)
# define HPS_HAS_NORETURN 1
#else
# define HPS_NORETURN
# define HPS_HAS_NORETURN 0
#endif

#if HPS_HAS_ATTRIBUTE(warn_unused_result)
# define HPS_NODISCARD __attribute__((__warn_unused_result__))
# define HPS_HAS_NODISCARD 1
#elif HPS_C_IS_C23
# define HPS_NODISCARD [[nodiscard]]
# define HPS_HAS_NODISCARD 1
#else
# define HPS_NODISCARD
# define HPS_HAS_NODISCARD 0
#endif

#if HPS_HAS_ATTRIBUTE(deprecated)
# define HPS_DEPRECATED(message) __attribute__((__deprecated__(message)))
# define HPS_HAS_DEPRECATED 1
#elif HPS_C_IS_C23
# define HPS_DEPRECATED(message) [[deprecated(message)]]
# define HPS_HAS_DEPRECATED 1
#elif HPS_COMPILER_IS_MSVC
# define HPS_DEPRECATED(message) __declspec(deprecated(message))
# define HPS_HAS_DEPRECATED 1
#else
# define HPS_DEPRECATED(message)
# define HPS_HAS_DEPRECATED 0
#endif

/* No fake fallback for alignment or thread-local storage. Check the flags. */
#if HPS_C_IS_C23
# define HPS_ALIGNAS(alignment) alignas(alignment)
# define HPS_ALIGNOF(type) alignof(type)
#elif HPS_HAS_ALIGNAS && HPS_HAS_ALIGNOF
# define HPS_ALIGNAS(alignment) _Alignas(alignment)
# define HPS_ALIGNOF(type) _Alignof(type)
#endif
#if HPS_HAS_THREAD_LOCAL
# if HPS_C_IS_C23
#  define HPS_THREAD_LOCAL thread_local
# else
#  define HPS_THREAD_LOCAL _Thread_local
# endif
#endif

#endif /* HPS_BASE_ATTRIBUTES_H */
