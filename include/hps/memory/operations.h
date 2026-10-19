#ifndef HPS_MEMORY_OPERATIONS_H
#define HPS_MEMORY_OPERATIONS_H

/**
 * @file hps/memory/operations.h
 * @brief Byte copying, movement, comparison, search and volatile clearing.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Callers prove live readable/writable extents of at least count bytes and
 * serialize conflicting access. NULL is permitted only when count is zero.
 * No function allocates, changes ownership or validates arbitrary pointers.
 * Comparison/volatile clearing require a target where uint8_t is an unsigned
 * character type for object-representation access; other targets are unqualified.
 * Object-byte copies do not grant alignment, effective type or valid values to
 * otherwise unsuitable storage. Never compare padding as meaningful object data.
 * HPS wrapper bookkeeping uses O(1) automatic storage. HPS-owned loops perform
 * at most count byte iterations; delegated libc work/stack/resource bounds
 * require target qualification.
 */

/* Fill count bytes with value, returning destination. This initializes bytes,
 * not necessarily valid pointer/floating/aggregate values. */
void *hps_memset(void *destination, uint8_t value, size_t count);

/* Copy count bytes from a disjoint source range; return destination. */
void *hps_memcpy(void *destination, const void *source, size_t count);

/* Copy as if through temporary byte storage; overlapping ranges are supported.
 * Return destination. Actual stack/runtime behavior depends on target libc. */
void *hps_memmove(void *destination, const void *source, size_t count);

/* Exact difference of the first differing unsigned bytes, in [-255, 255];
 * zero if equal. Stops on mismatch; unsuitable for comparing secrets. */
int32_t hps_memcmp(const void *left, const void *right, size_t count);

/* Borrow the first matching byte or return NULL; retain source constness and
 * lifetime. An empty range returns NULL without accessing source. */
const void *hps_memchr(const void *source, uint8_t value, size_t count);

/* Perform count volatile zero stores. No approved sensitive-erasure, cache,
 * synchronization or constant-time guarantee; target review is still required. */
void hps_memzero_volatile(void *destination, size_t count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_OPERATIONS_H */
