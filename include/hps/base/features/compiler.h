#ifndef HPS_BASE_FEATURES_COMPILER_H
#define HPS_BASE_FEATURES_COMPILER_H

#include "hps/base/compiler.h"


/*
 * Compiler extensions are intentionally not treated as standard
 * language features.
 *
 * This header is reserved for compiler capability detection.
 */


/*
 * Built-in compiler support.
 */
#if defined(__GNUC__) && !defined(_MSC_VER)

# define HPS_HAS_GNU_EXTENSIONS 1

#else

# define HPS_HAS_GNU_EXTENSIONS 0

#endif


#endif /* HPS_BASE_FEATURES_COMPILER_H */
