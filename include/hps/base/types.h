
#ifndef HPS_BASE_TYPES_H
#define HPS_BASE_TYPES_H

/*
 * HPS Standard Type Compatibility
 *
 * Goals:
 *   - Prefer standard integer types.
 *   - Support legacy C90 and C++98 toolchains.
 *   - Avoid unnecessary aliases.
 *
 * Legacy integer fallback is explicitly enabled with:
 *
 *   HPS_LEGACY_NO_STDINT
 *
 * Never enable the fallback when stdint.h has already
 * provided the corresponding type definitions.
 */

#include <hps/base/language.h>
#include <stddef.h>
#include <limits.h>

/* ----------------------------------------------------------
 * 1. Fixed-width integer types
 * ---------------------------------------------------------- */
#if defined(HPS_LEGACY_NO_STDINT)


    /*
     * Authorize inclusion of an internal Base header.
     */
    #define HPS_BASE_INSIDE 1
        #include <hps/base/detail/stdint_fallback.h>
    /*
     * Remove temporary internal authorization.
     */
    #undef HPS_BASE_INSIDE



#else

    
    /*
     * C99+ and C++11+: standard library.
     *
     * C90 and C++98: use the toolchain-provided header.
     * Its presence must be verified for those legacy modes.
     */
    
    #include <stdint.h>

#endif





/* ----------------------------------------------------------
 * Boolean compatibility
 *
 * C++98 and later:
 *     Native bool, true and false.
 *
 * C23:
 *     Native bool, true and false.
 *
 * C99 to C17:
 *     Standard <stdbool.h>.
 *
 * C90:
 *     Restricted compatibility fallback.
 * ---------------------------------------------------------- */

#if defined(__cplusplus)

/* Native C++ boolean support. */

#elif defined(__STDC_VERSION__) && \
      (__STDC_VERSION__ >= 202311L)

/* Native C23 boolean support. */

#elif defined(__STDC_VERSION__) && \
      (__STDC_VERSION__ >= 199901L)

#include <stdbool.h>

#else

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define HPS_BOOL(value) ((value) != 0)

#endif





#endif /* HPS_BASE_TYPES_H */
