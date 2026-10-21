#ifndef HPS_MEMORY_HPS_ALLOCATED_ARRAY_H
#define HPS_MEMORY_HPS_ALLOCATED_ARRAY_H

/**
 * @file hps/memory/hps_allocated_array.h
 * @brief Optional allocator proposal for checked array creation and resizing.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/hps_attributes.h"
#include "hps/memory/hps_allocator.h"
#include "hps/memory/hps_array.h"
#include "hps/memory/hps_operations.h"

/* DEVELOPMENT PROPOSAL, outside default production core paths: HPS-MEM-001
 * prohibits allocator hooks there. No exception is approved by these helpers.
 * Only caller-owned/static development providers are in scope; no heap/OS
 * backend, byte-pool adapter or arbitrary typed-storage guarantee is supplied.
 * The allocator table/context outlive all returned storage. Every provider
 * defines supported types/alignment, extent, bounds and single-owner context.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Request count positive elements of positive size, initialized to zero bytes.
 * NULL allocator, missing release, invalid/overflowing sizes, missing allocation
 * callbacks or provider failure return NULL. Checks precede every callback.
 * Successful results are exclusively owned by the caller and are released with
 * the same allocator and checked count*element_size. Byte zeroing does not imply
 * valid pointer, floating or aggregate representations. Wrapper work/space is
 * O(1), plus zeroing at most count*element_size bytes and provider work.
 */
static HPS_INLINE void *hps_allocated_array_create_zeroed(
    const hps_allocator_t *allocator, size_t count, size_t element_size)
{
    void *result = NULL;
    size_t size = 0U;

    if ((allocator != NULL) && (count != 0U)) {
        if ((allocator->release != NULL) &&
            (hps_memory_array_size(count, element_size, &size) ==
             HPS_MEMORY_ARRAY_OK)) {
            result = hps_allocator_calloc(allocator, count, element_size);
        }
    }
    return result;
}

/* Duplicate count elements from live initialized source storage. The source is
 * non-NULL, readable for the checked product and disjoint from returned storage
 * and allocator metadata. Only representation copying is performed: callers
 * prove destination type suitability and absence of duplicated resource owners.
 * Failure leaves source unchanged and returns NULL; successful results follow
 * create_zeroed ownership/release rules. O(product) work, O(1) wrapper space.
 */
static HPS_INLINE void *hps_allocated_array_duplicate(
    const hps_allocator_t *allocator, const void *source,
    size_t count, size_t element_size)
{
    void *result = NULL;
    size_t size = 0U;

    if ((source != NULL) && (count != 0U) && (allocator != NULL)) {
        if ((allocator->release != NULL) &&
            (hps_memory_array_size(count, element_size, &size) ==
             HPS_MEMORY_ARRAY_OK)) {
            /* An ordinary allocate callback avoids a redundant zeroing pass;
             * an allocate_zeroed-only provider remains usable. */
            if (allocator->allocate != NULL) {
                result = hps_allocator_alloc(allocator, size);
            } else {
                result = hps_allocator_calloc(allocator, count, element_size);
            }
            if (result != NULL) {
                (void)hps_memcpy(result, source, size);
            }
        }
    }
    return result;
}

/* With NULL pointer, old_count must be zero; this delegates to create_zeroed.
 * With a live pointer, old_count is positive and exactly describes its current
 * allocation from allocator. new_count and element_size are positive. A backend
 * resize preserves min(old_size,new_size) representation bytes, owns no aliases
 * on success and preserves the old allocation completely on failure. Growth
 * clears only newly added bytes after successful resize. Byte zeroing is not
 * arbitrary typed initialization. No allocate/copy/release resize fallback.
 *
 * Both products are checked before callbacks. A zero new_count is rejected
 * without releasing anything: call hps_allocator_free explicitly instead.
 * Missing release/resize, invalid input, overflow or backend failure return NULL
 * and preserve the old owner. Retain the original pointer until non-NULL success;
 * success invalidates all aliases to the previous allocation. There are no
 * hidden callbacks on validation failures. O(1) wrapper space/work plus at most
 * new_size-old_size zero stores and the provider's bounded resize work.
 */
static HPS_INLINE void *hps_allocated_array_resize_zeroed(
    const hps_allocator_t *allocator, void *pointer, size_t old_count,
    size_t new_count, size_t element_size)
{
    void *result = NULL;
    size_t old_size = 0U;
    size_t new_size = 0U;

    if ((allocator != NULL) && (new_count != 0U)) {
        if ((allocator->release != NULL) &&
            (hps_memory_array_size(new_count, element_size, &new_size) ==
             HPS_MEMORY_ARRAY_OK)) {
            if (pointer == NULL) {
                if (old_count == 0U) {
                    result = hps_allocated_array_create_zeroed(
                        allocator, new_count, element_size);
                }
            } else if ((old_count != 0U) && (allocator->resize != NULL)) {
                if (hps_memory_array_size(old_count, element_size, &old_size) ==
                    HPS_MEMORY_ARRAY_OK) {
                    result = hps_allocator_realloc(allocator, pointer, old_size,
                                                   new_size);
                    if ((result != NULL) && (new_size > old_size)) {
                        uint8_t *bytes = (uint8_t *)result;
                        (void)hps_memset(&bytes[old_size], UINT8_C(0),
                                         new_size - old_size);
                    }
                }
            } else {
                /* Preserve the existing allocation when it cannot be resized. */
            }
        }
    }
    return result;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_HPS_ALLOCATED_ARRAY_H */
