#ifndef HPS_PLATFORM_POSIX_H
#define HPS_PLATFORM_POSIX_H

/**
 * @file hps/platform/posix.h
 * @brief POSIX and X/Open levels advertised by already-visible version macros.
 */

/*
 * Include the selected platform's unistd.h before first inclusion when version
 * evidence is needed. No system header or feature-test macro is supplied here.
 * HPS_POSIX_AVAILABLE reports visible _POSIX_VERSION evidence, not an OS-family
 * guess or a promise that an arbitrary POSIX interface is provided. A zero result
 * means evidence was absent at inclusion; it does not prove unsupported APIs.
 * The independent family hint is in hps/platform/os.h.
 */

#if defined(_POSIX_VERSION)
# define HPS_POSIX_AVAILABLE 1
# define HPS_POSIX_VERSION _POSIX_VERSION
#else
# define HPS_POSIX_AVAILABLE 0
# define HPS_POSIX_VERSION 0L
#endif

#if defined(_XOPEN_VERSION)
# define HPS_XOPEN_VERSION _XOPEN_VERSION
#else
# define HPS_XOPEN_VERSION 0L
#endif

#endif /* HPS_PLATFORM_POSIX_H */
