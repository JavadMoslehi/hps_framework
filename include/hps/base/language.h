#ifndef HPS_BASE_LANGUAGE_H
#define HPS_BASE_LANGUAGE_H

/*
 * HPS Language Abstraction
 *
 * This header provides:
 *   - C and C++ language detection.
 *   - C language standard classification.
 *   - C23 draft identification.
 *   - C++ linkage helpers for public C APIs.
 *
 * Requirements:
 *   - ISO C90 compatible syntax.
 *   - No runtime dependencies.
 *   - No compiler-specific extensions.
 */


/* ----------------------------------------------------------
 * 1. Language identification
 * ---------------------------------------------------------- */

#if defined(__cplusplus)

#define HPS_LANG_C       0
#define HPS_LANG_CPP     1

#else

#define HPS_LANG_C       1
#define HPS_LANG_CPP     0

#endif


/* ----------------------------------------------------------
 * 2. C language standard identification
 *
 * HPS_C_STANDARD:
 *    0  -> Compiling as C++
 *   90  -> C90
 *   99  -> C99
 *   11  -> C11
 *   17  -> C17
 *   23  -> C23 or C2x draft
 *
 * HPS_C23_DRAFT distinguishes draft C2x from final C23.
 *
 * These macros report the selected language mode, not
 * complete conformance to the ISO specification.
 * ---------------------------------------------------------- */

#if HPS_LANG_CPP

#define HPS_C_STANDARD   0
#define HPS_C23_DRAFT    0

#elif !defined(__STDC_VERSION__)

#define HPS_C_STANDARD   90
#define HPS_C23_DRAFT    0

#elif __STDC_VERSION__ >= 202311L

#define HPS_C_STANDARD   23
#define HPS_C23_DRAFT    0

#elif __STDC_VERSION__ >= 202000L

#define HPS_C_STANDARD   23
#define HPS_C23_DRAFT    1

#elif __STDC_VERSION__ >= 201710L

#define HPS_C_STANDARD   17
#define HPS_C23_DRAFT    0

#elif __STDC_VERSION__ >= 201112L

#define HPS_C_STANDARD   11
#define HPS_C23_DRAFT    0

#elif __STDC_VERSION__ >= 199901L

#define HPS_C_STANDARD   99
#define HPS_C23_DRAFT    0

#else

/*
 * Includes C90 and the C95 amendment.
 */
#define HPS_C_STANDARD   90
#define HPS_C23_DRAFT    0

#endif


/* ----------------------------------------------------------
 * 3. C++ linkage helpers
 *
 * Use these around public C function declarations that
 * must be callable from C++.
 *
 * These macros do not provide C++ ABI compatibility for
 * arbitrary C structures or compiler-specific types.
 * ---------------------------------------------------------- */

#if HPS_LANG_CPP

#define HPS_BEGIN_DECLS  extern "C" {
#define HPS_END_DECLS    }

#else

#define HPS_BEGIN_DECLS
#define HPS_END_DECLS

#endif

#endif /* HPS_BASE_LANGUAGE_H */

