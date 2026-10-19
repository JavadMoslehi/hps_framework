#ifndef HPS_PLATFORM_LIBC_H
#define HPS_PLATFORM_LIBC_H

/**
 * @file hps/platform/libc.h
 * @brief Visible C-library family evidence and an optional explicit family hint.
 */

/*
 * Include the consumer's selected C-library header before the first inclusion
 * of this header when automatic library identification is needed. No libc or
 * OS header is imported here. Detection observes macros visible at that point;
 * an unknown Linux library is not presumed musl, and compiler/OS identity alone
 * does not establish a C-library implementation or any callable capability.
 *
 * HPS_CONFIG_LIBC is an optional integer preprocessing expression selecting one
 * known HPS_LIBC_* family below, set before first inclusion. This supports musl,
 * which has no official identification macro, and libraries without a visible
 * marker. The hint must agree with any observed family. UNKNOWN is a detection
 * result rather than an override. Consumers must not change it mid-translation
 * unit. These selectors affect reported metadata only, never declarations/ABI.
 */

#define HPS_LIBC_UNKNOWN 0
#define HPS_LIBC_GLIBC 1
#define HPS_LIBC_MUSL 2
#define HPS_LIBC_UCLIBC 3
#define HPS_LIBC_NEWLIB 4
#define HPS_LIBC_BIONIC 5
#define HPS_LIBC_MSVCRT 6
#define HPS_LIBC_APPLE 7
#define HPS_LIBC_UCRT 8

/* Test implementation families before their compatibility macros. */
#if defined(__BIONIC__)
# define HPS_LIBC_DETECTED HPS_LIBC_BIONIC
#elif defined(__UCLIBC__)
# define HPS_LIBC_DETECTED HPS_LIBC_UCLIBC
#elif defined(__NEWLIB__)
# define HPS_LIBC_DETECTED HPS_LIBC_NEWLIB
#elif defined(__GLIBC__)
# define HPS_LIBC_DETECTED HPS_LIBC_GLIBC
#elif defined(__DARWIN_C_LEVEL)
# define HPS_LIBC_DETECTED HPS_LIBC_APPLE
#elif defined(_UCRT)
# define HPS_LIBC_DETECTED HPS_LIBC_UCRT
#elif defined(__MSVCRT__) && !defined(__CRTDLL__)
# define HPS_LIBC_DETECTED HPS_LIBC_MSVCRT
#else
# define HPS_LIBC_DETECTED HPS_LIBC_UNKNOWN
#endif

#ifdef HPS_CONFIG_LIBC
# if (HPS_CONFIG_LIBC != HPS_LIBC_GLIBC) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_MUSL) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_UCLIBC) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_NEWLIB) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_BIONIC) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_MSVCRT) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_APPLE) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_UCRT)
#  error "HPS_CONFIG_LIBC must select a known HPS_LIBC_* family"
# endif
# if (HPS_LIBC_DETECTED != HPS_LIBC_UNKNOWN) && \
     (HPS_CONFIG_LIBC != HPS_LIBC_DETECTED)
#  error "HPS_CONFIG_LIBC disagrees with visible C-library evidence"
# endif
# define HPS_LIBC HPS_CONFIG_LIBC
#else
# define HPS_LIBC HPS_LIBC_DETECTED
#endif

#define HPS_LIBC_IS_UNKNOWN (HPS_LIBC == HPS_LIBC_UNKNOWN)
#define HPS_LIBC_IS_GLIBC (HPS_LIBC == HPS_LIBC_GLIBC)
#define HPS_LIBC_IS_MUSL (HPS_LIBC == HPS_LIBC_MUSL)
#define HPS_LIBC_IS_UCLIBC (HPS_LIBC == HPS_LIBC_UCLIBC)
#define HPS_LIBC_IS_NEWLIB (HPS_LIBC == HPS_LIBC_NEWLIB)
#define HPS_LIBC_IS_BIONIC (HPS_LIBC == HPS_LIBC_BIONIC)
#define HPS_LIBC_IS_MSVCRT (HPS_LIBC == HPS_LIBC_MSVCRT)
#define HPS_LIBC_IS_APPLE (HPS_LIBC == HPS_LIBC_APPLE)
#define HPS_LIBC_IS_UCRT (HPS_LIBC == HPS_LIBC_UCRT)

#endif /* HPS_PLATFORM_LIBC_H */
