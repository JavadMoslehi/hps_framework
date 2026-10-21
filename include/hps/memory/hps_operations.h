#ifndef HPS_MEMORY_HPS_OPERATIONS_H
#define HPS_MEMORY_HPS_OPERATIONS_H

/**
 * @file hps/memory/hps_operations.h
 * @brief Byte copying, movement, comparison, exchange and volatile clearing.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"

/* Payload scratch budget used by hps_memswap, excluding scalar bookkeeping. */
#define HPS_MEMSWAP_SCRATCH_BYTES 256U

#ifdef __cplusplus
extern "C" {
#endif

/* Callers prove live readable/writable extents of at least count bytes and
 * serialize conflicting access. NULL is permitted only when count is zero.
 * No function allocates, changes ownership or validates arbitrary pointers.
 * Representation traversal (comparison, swap, full-scan equality and volatile
 * clearing) requires uint8_t to be an unsigned character type on the target;
 * other targets are unqualified for these object-representation operations.
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

/* Return 1 for a non-NULL pointer, otherwise 0. This tests only nullness; it
 * establishes no storage validity, extent, lifetime, alignment or ownership.
 * The input must itself be an initialized, determinate pointer value. O(1). */
uint8_t hps_pointer_is_nonnull(const void *pointer);

/* Exchange count representation bytes. Ranges must be disjoint or identical;
 * partial overlap is unsupported. Each range is readable and writable. Uses
 * HPS_MEMSWAP_SCRATCH_BYTES automatic payload bytes plus O(1) scalars; loops
 * perform at most count byte-equivalent exchanges, excluding libc work. */
void hps_memswap(void *left, void *right, size_t count);

/* Return 1 when every byte matches, otherwise 0. Empty ranges compare equal.
 * The source loop visits all count bytes without a data-dependent early exit.
 * Compiler transformations and target timing remain unverified: this API has
 * no constant-time or cryptographic-provider guarantee. O(count), O(1) space. */
uint8_t hps_memequal_full_scan(const void *left, const void *right,
                              size_t count);

/* Perform count volatile zero stores. No approved sensitive-erasure, cache,
 * synchronization or constant-time guarantee; target review is still required. */
void hps_memzero_volatile(void *destination, size_t count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_HPS_OPERATIONS_H */
