#ifndef HPS_BASE_FEATURES_LANGUAGE_H
#define HPS_BASE_FEATURES_LANGUAGE_H

#include "hps/base/standard.h"
#include "hps/base/compiler.h"


/* --------------------------------------------------------------------------
 * C99
 * -------------------------------------------------------------------------- */

#if HPS_C_IS_C99

# define HPS_HAS_C99_FEATURES 1

#else

# define HPS_HAS_C99_FEATURES 0

#endif


/* --------------------------------------------------------------------------
 * C11
 * -------------------------------------------------------------------------- */

#if HPS_C_IS_C11

# define HPS_HAS_C11_FEATURES 1

#else

# define HPS_HAS_C11_FEATURES 0

#endif


/* --------------------------------------------------------------------------
 * C17
 * -------------------------------------------------------------------------- */

#if HPS_C_IS_C17

# define HPS_HAS_C17_FEATURES 1

#else

# define HPS_HAS_C17_FEATURES 0

#endif


/* --------------------------------------------------------------------------
 * C23
 * -------------------------------------------------------------------------- */

#if HPS_C_IS_C23

# define HPS_HAS_C23_FEATURES 1

#else

# define HPS_HAS_C23_FEATURES 0

#endif


/* --------------------------------------------------------------------------
 * C11 language features
 * -------------------------------------------------------------------------- */

#if HPS_HAS_C11_FEATURES

# define HPS_HAS_STATIC_ASSERT 1
# define HPS_HAS_GENERIC_SELECTION 1
# define HPS_HAS_ALIGNOF 1
# define HPS_HAS_ALIGNAS 1
# if defined(_MSC_VER)
#  define HPS_HAS_THREAD_LOCAL 0
# else
#  define HPS_HAS_THREAD_LOCAL 1
# endif

#else

# define HPS_HAS_STATIC_ASSERT 0
# define HPS_HAS_GENERIC_SELECTION 0
# define HPS_HAS_ALIGNOF 0
# define HPS_HAS_ALIGNAS 0
# define HPS_HAS_THREAD_LOCAL 0

#endif


/* --------------------------------------------------------------------------
 * C99 language features
 * -------------------------------------------------------------------------- */

#if HPS_HAS_C99_FEATURES

# if HPS_COMPILER_IS_MSVC || defined(_MSC_VER)
#  define HPS_HAS_VLA 0
#  define HPS_HAS_COMPLEX 0
# else
#  define HPS_HAS_VLA 1
#  define HPS_HAS_COMPLEX 1
# endif
# define HPS_HAS_FLEXIBLE_ARRAY 1

#else

# define HPS_HAS_VLA 0
# define HPS_HAS_FLEXIBLE_ARRAY 0
# define HPS_HAS_COMPLEX 0

#endif


/* --------------------------------------------------------------------------
 * Optional C11 features
 * -------------------------------------------------------------------------- */

#if HPS_HAS_C99_FEATURES

# if defined(__STDC_NO_VLA__) && (__STDC_NO_VLA__ == 1)
#  undef HPS_HAS_VLA
#  define HPS_HAS_VLA 0
# endif

# if defined(__STDC_NO_COMPLEX__) && (__STDC_NO_COMPLEX__ == 1)
#  undef HPS_HAS_COMPLEX
#  define HPS_HAS_COMPLEX 0
# endif

#endif


/* Atomics are optional, including on hosted implementations. */
#if HPS_HAS_C11_FEATURES && !defined(_MSC_VER)
# if defined(__STDC_NO_ATOMICS__) && (__STDC_NO_ATOMICS__ == 1)
#  define HPS_HAS_ATOMICS 0
# else
#  define HPS_HAS_ATOMICS 1
# endif
#else
# define HPS_HAS_ATOMICS 0
#endif

#endif /* HPS_BASE_FEATURES_LANGUAGE_H */
