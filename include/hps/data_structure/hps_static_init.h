#ifndef HPS_DATA_STRUCTURE_HPS_STATIC_INIT_H
#define HPS_DATA_STRUCTURE_HPS_STATIC_INIT_H

/**
 * @file hps/data_structure/hps_static_init.h
 * @brief Typed empty initialization shared by in-place containers and optional values.
 */

/* In-place containers own their embedded array. capacity_ must be a positive
 * integer constant expression. Empty construction initializes every element to
 * its C zero-initialized value (including proper null pointers), then zeroes all
 * counters. It performs bounded O(capacity_) initialization/copying, not O(1).
 * Returning/copying an object copies its embedded array and invalidates no source
 * object, but pointers into one copy do not retarget themselves to another copy.
 */
#ifdef __cplusplus
# define HPS_DATA_STRUCTURE_EMPTY_INIT {}
#else
# define HPS_DATA_STRUCTURE_EMPTY_INIT {0}
#endif

#endif /* HPS_DATA_STRUCTURE_HPS_STATIC_INIT_H */
