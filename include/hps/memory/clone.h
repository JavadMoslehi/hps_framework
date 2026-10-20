#ifndef HPS_MEMORY_CLONE_H
#define HPS_MEMORY_CLONE_H

/**
 * @file hps/memory/clone.h
 * @brief Optional development proposal for array cloning with cleanup callbacks.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/attributes.h"
#include "hps/memory/allocator.h"
#include "hps/memory/operations.h"

/* DEVELOPMENT PROPOSAL: this allocator-dependent extension is outside default
 * production core paths. HPS-MEM-001 prohibits dependency allocator hooks in
 * those paths; neither this header nor a supplied callback approves an exception.
 */

#define HPS_CLONE_OK ((int32_t)INT32_C(0))
#define HPS_CLONE_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_CLONE_SIZE_OVERFLOW ((int32_t)-INT32_C(2))
#define HPS_CLONE_ELEMENT_FAILED ((int32_t)-INT32_C(3))

#ifdef __cplusplus
extern "C" {
#endif

/* dst/src identify one correctly aligned object of the callback's declared
 * element type. Zero succeeds and establishes ownership in dst; every nonzero
 * result fails. A failed callback must release its own partial resources before
 * returning: the array engine cleans only earlier successful elements. alloc is
 * forwarded unchanged and may be NULL only if the callback permits it.
 */
typedef int32_t (*hps_clone_fn)(void *dst, const void *src,
                               const hps_allocator_t *alloc);

/* Releases resources of one successfully cloned element. Must complete without
 * failure or allocation; alloc is the same table passed to its cloner. */
typedef void (*hps_destruct_fn)(void *item, const hps_allocator_t *alloc);

/* dst_arr/src_arr must be non-NULL even for count==0, preserving the draft's
 * pointer validation. For count>0, elem_size must be positive. Both arrays have
 * at least count*elem_size bytes in live storage; source objects are initialized
 * and destination slots have the alignment/type required by the callback.
 * Destination slots initially own no resources. Array ranges are disjoint from
 * each other and from allocator/context metadata, and callbacks preserve source
 * objects and array metadata. No lifetime/actual-extent validation is possible.
 * Generic representation traversal requires target evidence that uint8_t is an
 * unsigned character type; this is not promised for every exact-width target.
 *
 * NULL cloner performs a shallow byte copy; the caller must ensure that copying
 * representations creates valid destination objects and no duplicate ownership.
 * Otherwise clones in ascending index order and forwards alloc unchanged. A
 * resource-owning cloner MUST have a non-NULL dtor; an omitted dtor is permitted
 * only when successful elements have no resources requiring cleanup.
 *
 * Invalid pointers/element size return INVALID_ARGUMENT before multiplication;
 * overflow returns SIZE_OVERFLOW before any copy/callback. Any nonzero cloner
 * result maps to ELEMENT_FAILED. Cleanup visits only completed elements in
 * ascending order, preserving the source draft. It does not restore destination
 * bytes; discard all destination values after failure. No unconditional
 * transactional/no-leak guarantee is made for arbitrary callbacks.
 *
 * No shared runtime state, locks, retries, recursion or temporary payload array.
 * Caller serializes source/destination/backend access. Engine automatic storage
 * is O(1); work is O(count) plus callback work, or O(count*elem_size) for shallow
 * copy. The caller budgets count, element size, callback execution and storage.
 */
static HPS_INLINE int32_t hps_array_deep_copy(
    void *dst_arr, const void *src_arr, const size_t count,
    const size_t elem_size, hps_clone_fn cloner, hps_destruct_fn dtor,
    const hps_allocator_t *alloc)
{
    int32_t status = HPS_CLONE_INVALID_ARGUMENT;
    size_t total_size = 0U;

    if ((dst_arr != NULL) && (src_arr != NULL)) {
        if ((count == 0U) || (elem_size != 0U)) {
            if (hps_allocator_check_mul_overflow(count, elem_size,
                                                  &total_size) != UINT8_C(0)) {
                status = HPS_CLONE_SIZE_OVERFLOW;
            } else if (cloner == NULL) {
                (void)hps_memcpy(dst_arr, src_arr, total_size);
                status = HPS_CLONE_OK;
            } else {
                uint8_t *destination = (uint8_t *)dst_arr;
                const uint8_t *source = (const uint8_t *)src_arr;
                size_t index = 0U;

                status = HPS_CLONE_OK;
                while ((index < count) && (status == HPS_CLONE_OK)) {
                    /* The checked total bounds every element offset. */
                    void *current_dst = &destination[index * elem_size];
                    const void *current_src = &source[index * elem_size];
                    const int32_t clone_result = cloner(current_dst,
                                                        current_src, alloc);

                    if (clone_result != INT32_C(0)) {
                        status = HPS_CLONE_ELEMENT_FAILED;
                        if (dtor != NULL) {
                            for (size_t rollback_index = 0U;
                                 rollback_index < index;
                                 rollback_index += 1U) {
                                void *completed_dst =
                                    &destination[rollback_index * elem_size];
                                dtor(completed_dst, alloc);
                            }
                        }
                    } else {
                        index += 1U;
                    }
                }
            }
        }
    }
    return status;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_CLONE_H */
