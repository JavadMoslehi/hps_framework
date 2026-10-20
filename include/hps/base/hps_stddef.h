#ifndef HPS_BASE_HPS_STDDEF_H
#define HPS_BASE_HPS_STDDEF_H

/**
 * @file hps/base/hps_stddef.h
 * @brief Standard object-size types and offsetof with an opt-in compiler fallback.
 */

/* The standard header is available in conforming C89+ and C++ implementations.
 * Set HPS_STDDEF_USE_FALLBACK=1 only for a target without a usable header.
 * Select one configuration consistently in every translation unit. Replacement
 * definitions must not be mixed with another stddef provider, including native
 * headers that define the same types indirectly. Fallback
 * definitions depend on compiler metadata, never architecture-name ABI guesses.
 * This is a C89/C++98 subset: newer additions such as max_align_t and nullptr_t
 * are supplied only by the native header when its selected language requires it.
 */
#ifndef HPS_STDDEF_USE_FALLBACK
# define HPS_STDDEF_USE_FALLBACK 0
#endif
#if (HPS_STDDEF_USE_FALLBACK != 0) && (HPS_STDDEF_USE_FALLBACK != 1)
# error "HPS_STDDEF_USE_FALLBACK must be 0 or 1"
#endif

#if HPS_STDDEF_USE_FALLBACK == 0

# include <stddef.h>

#else

# if !defined(__SIZE_TYPE__) || !defined(__PTRDIFF_TYPE__)
#  error "HPS stddef fallback requires compiler size and pointer-difference types"
# endif
# if !defined(__cplusplus) && !defined(__WCHAR_TYPE__)
#  error "HPS stddef fallback requires the compiler wide-character type"
# endif
# if !defined(__GNUC__) && !defined(__clang__)
#  error "HPS stddef fallback requires compiler __builtin_offsetof support"
# endif

/* These are the compiler's actual standard ABI types, not HPS aliases. */
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
# if !defined(__cplusplus)
typedef __WCHAR_TYPE__ wchar_t;
# endif

# ifndef NULL
#  if defined(__cplusplus)
#   define NULL 0
#  else
#   define NULL ((void *)0)
#  endif
# endif

/* Avoid manufacturing an object pointer or dereferencing a null address. The
 * same constraints as the native offsetof apply, including C++ layout rules.
 */
# ifndef offsetof
#  define offsetof(type_, member_) __builtin_offsetof(type_, member_)
# endif

#endif /* HPS_STDDEF_USE_FALLBACK */

#endif /* HPS_BASE_HPS_STDDEF_H */
