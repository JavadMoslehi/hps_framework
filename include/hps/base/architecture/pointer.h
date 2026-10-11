#ifndef HPS_BASE_ARCHITECTURE_POINTER_H
#define HPS_BASE_ARCHITECTURE_POINTER_H

/**
 * @file hps/base/architecture/pointer.h
 * @brief Target pointer bit count and width classification.
 */

/*
 * HPS - General Purpose C Framework
 *
 * Target pointer width detection.
 */


/* --------------------------------------------------------------------------
 * Pointer width identifiers
 * -------------------------------------------------------------------------- */

#include <limits.h>

#define HPS_POINTER_WIDTH_UNKNOWN  0
#define HPS_POINTER_WIDTH_32       1
#define HPS_POINTER_WIDTH_64       2


/* --------------------------------------------------------------------------
 * Pointer bit width
 * -------------------------------------------------------------------------- */

#if defined(__SIZEOF_POINTER__)

# define HPS_POINTER_BITS (__SIZEOF_POINTER__ * CHAR_BIT)

#elif defined(_WIN64)

# define HPS_POINTER_BITS 64

#elif defined(_WIN32)

# define HPS_POINTER_BITS 32

#else

# define HPS_POINTER_BITS 0

#endif


/* --------------------------------------------------------------------------
 * Pointer width classification
 * -------------------------------------------------------------------------- */

#if HPS_POINTER_BITS == 32

# define HPS_POINTER_WIDTH HPS_POINTER_WIDTH_32

#elif HPS_POINTER_BITS == 64

# define HPS_POINTER_WIDTH HPS_POINTER_WIDTH_64

#else

# define HPS_POINTER_WIDTH HPS_POINTER_WIDTH_UNKNOWN

#endif


#endif /* HPS_BASE_ARCHITECTURE_POINTER_H */
