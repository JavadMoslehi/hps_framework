#ifndef HPS_MEMORY_HPS_ARRAY_H
#define HPS_MEMORY_HPS_ARRAY_H

/**
 * @file hps/memory/hps_array.h
 * @brief Checked element-count wrappers for caller-owned byte operations.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"

#define HPS_MEMORY_ARRAY_OK ((int32_t)INT32_C(0))
#define HPS_MEMORY_ARRAY_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_MEMORY_ARRAY_SIZE_OVERFLOW ((int32_t)-INT32_C(2))

#ifdef __cplusplus
extern "C" {
#endif

/* Check count*element_size and write size only on success. size is non-NULL;
 * element_size must be positive when count is positive. Invalid arguments take
 * priority over overflow. O(1) work/space, no allocation or runtime callbacks. */
int32_t hps_memory_array_size(size_t count, size_t element_size, size_t *size);

/* These functions check count multiplication, not actual C array extents.
 * Callers prove live storage of count*element_size bytes and the unsigned-
 * character uint8_t target assumption in hps/memory/hps_operations.h. Zero count
 * permits NULL data pointers and zero element size. Nonzero count requires
 * non-NULL pointers and positive element size. Argument errors precede size
 * overflow; every error preserves data and output objects. No allocation,
 * ownership transfer, internal shared state or synchronization is performed.
 * Scalar bookkeeping is O(1); successful byte work is O(count*element_size),
 * with delegated libc resource bounds subject to target qualification.
 * Byte zeroing/copying does not initialize arbitrary typed representations.
 */
int32_t hps_memory_array_clear(void *destination, size_t count,
                              size_t element_size);

/* Source/destination byte ranges must be disjoint. */
int32_t hps_memory_array_copy(void *destination, const void *source,
                             size_t count, size_t element_size);

/* Source/destination byte ranges may overlap. */
int32_t hps_memory_array_move(void *destination, const void *source,
                             size_t count, size_t element_size);

/* comparison is non-NULL, writable and disjoint from the compared ranges.
 * Writes the exact first-byte difference in [-255,255], or zero when equal;
 * empty ranges write zero. Never compare structure padding as meaningful data.
 * The comparison stops on a mismatch and is unsuitable for secrets. */
int32_t hps_memory_array_compare(const void *left, const void *right,
                                size_t count, size_t element_size,
                                int32_t *comparison);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_HPS_ARRAY_H */
