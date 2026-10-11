#ifndef HPS_BASE_LANGUAGE_H
#define HPS_BASE_LANGUAGE_H

/**
 * @file hps/base/language.h
 * @brief C and C++ language and execution-environment detection.
 */

/* C++ is not a C dialect; do not advertise C features in C++ mode. */
#define HPS_C_STANDARD_C89 198900L
#define HPS_C_STANDARD_C95 199409L
#define HPS_C_STANDARD_C99 199901L
#define HPS_C_STANDARD_C11 201112L
#define HPS_C_STANDARD_C17 201710L
#define HPS_C_STANDARD_C23 202311L

#ifdef __cplusplus
# define HPS_LANGUAGE_IS_C 0
# define HPS_LANGUAGE_IS_CPP 1
# define HPS_C_STANDARD_VERSION 0L
# define HPS_CPP_STANDARD_VERSION __cplusplus
#else
# define HPS_LANGUAGE_IS_C 1
# define HPS_LANGUAGE_IS_CPP 0
# define HPS_CPP_STANDARD_VERSION 0L
# ifdef __STDC_VERSION__
#  define HPS_C_STANDARD_VERSION __STDC_VERSION__
# else
#  define HPS_C_STANDARD_VERSION HPS_C_STANDARD_C89
# endif
#endif

/* An absent hosted macro means unknown, not necessarily freestanding. */
#ifdef __STDC_HOSTED__
# define HPS_HOSTED_IS_KNOWN 1
# define HPS_IS_HOSTED (__STDC_HOSTED__ == 1)
# define HPS_IS_FREESTANDING (__STDC_HOSTED__ == 0)
#else
# define HPS_HOSTED_IS_KNOWN 0
# define HPS_IS_HOSTED 0
# define HPS_IS_FREESTANDING 0
#endif

#endif /* HPS_BASE_LANGUAGE_H */
