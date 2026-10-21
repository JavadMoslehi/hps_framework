#ifndef HPS_PLATFORM_HPS_OS_H
#define HPS_PLATFORM_HPS_OS_H

/**
 * @file hps/platform/hps_os.h
 * @brief Operating-system family hints from already-visible target macros.
 */

/*
 * These 0/1 preprocessing flags may overlap: Android is Linux, for example.
 * They establish neither API availability nor a qualified deployment profile.
 * Unknown targets remain unknown; an absent hosted OS is not embedded evidence.
 *
 * Apple consumers that need a subtype include their SDK's TargetConditionals.h
 * before the first inclusion of this header. No SDK or OS header is imported.
 */

#if defined(_WIN32) || defined(_WIN64)
# define HPS_PLATFORM_IS_WINDOWS 1
#else
# define HPS_PLATFORM_IS_WINDOWS 0
#endif

#if defined(__linux__)
# define HPS_PLATFORM_IS_LINUX 1
#else
# define HPS_PLATFORM_IS_LINUX 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
# define HPS_PLATFORM_IS_APPLE 1
#else
# define HPS_PLATFORM_IS_APPLE 0
#endif

#if HPS_PLATFORM_IS_APPLE && defined(TARGET_OS_IOS)
# if TARGET_OS_IOS
#  define HPS_PLATFORM_IS_IOS 1
# else
#  define HPS_PLATFORM_IS_IOS 0
# endif
#elif HPS_PLATFORM_IS_APPLE && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
/* Older SDKs lack TARGET_OS_IOS; exclude other known iPhone-family targets. */
# if (defined(TARGET_OS_TV) && TARGET_OS_TV) || \
     (defined(TARGET_OS_WATCH) && TARGET_OS_WATCH) || \
     (defined(TARGET_OS_VISION) && TARGET_OS_VISION) || \
     (defined(TARGET_OS_BRIDGE) && TARGET_OS_BRIDGE)
#  define HPS_PLATFORM_IS_IOS 0
# else
#  define HPS_PLATFORM_IS_IOS 1
# endif
#else
# define HPS_PLATFORM_IS_IOS 0
#endif

#if HPS_PLATFORM_IS_APPLE && defined(TARGET_OS_OSX) && TARGET_OS_OSX
# define HPS_PLATFORM_IS_MACOS 1
#else
# define HPS_PLATFORM_IS_MACOS 0
#endif

#if defined(__ANDROID__)
# define HPS_PLATFORM_IS_ANDROID 1
#else
# define HPS_PLATFORM_IS_ANDROID 0
#endif

#if defined(__unix__) || defined(__unix) || defined(unix)
# define HPS_PLATFORM_IS_UNIX 1
#else
# define HPS_PLATFORM_IS_UNIX 0
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(__DragonFly__)
# define HPS_PLATFORM_IS_BSD 1
#else
# define HPS_PLATFORM_IS_BSD 0
#endif

#define HPS_PLATFORM_IS_POSIX_FAMILY \
    (HPS_PLATFORM_IS_LINUX || HPS_PLATFORM_IS_APPLE || \
     HPS_PLATFORM_IS_ANDROID || HPS_PLATFORM_IS_BSD || HPS_PLATFORM_IS_UNIX)

#define HPS_PLATFORM_IS_UNKNOWN \
    (!(HPS_PLATFORM_IS_WINDOWS || HPS_PLATFORM_IS_LINUX || \
       HPS_PLATFORM_IS_APPLE || HPS_PLATFORM_IS_ANDROID || \
       HPS_PLATFORM_IS_BSD || HPS_PLATFORM_IS_UNIX))

#endif /* HPS_PLATFORM_HPS_OS_H */
