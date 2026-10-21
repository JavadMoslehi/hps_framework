#ifndef HPS_MEMORY_HPS_ALLOCATOR_H
#define HPS_MEMORY_HPS_ALLOCATOR_H

/**
 * @file hps/memory/hps_allocator.h
 * @brief Optional development proposal for an explicit allocator interface.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/hps_attributes.h"
#include "hps/memory/hps_operations.h"

/* DEVELOPMENT PROPOSAL: HPS-MEM-001 prohibits dependency allocator hooks in
 * production core paths. This optional extension is outside the default core;
 * including it or supplying a static backend does not approve an exception.
 * No backend, heap call, OS dependency or adapter to the byte pool is supplied.
 * Development providers must use caller-owned/static resource budgets; heap/OS
 * providers are outside this proposal's scope. Static backing alone does not
 * resolve the hook prohibition or establish typed-object storage suitability.
 */

/* Preserved request hint, not a proof of alignment for arbitrary object types.
 * A backend must document which types/alignments it actually supports. */
#define HPS_ALLOCATOR_DEFAULT_ALIGNMENT (sizeof(void *))

typedef struct hps_allocator_i hps_allocator_t;

#ifdef __cplusplus
extern "C" {
#endif

/* The member order and callback signatures match the supplied draft; member
 * names distinguish this interface from the existing process-wide byte pool.
 * Every callback is optional. The caller owns the table/context and keeps both
 * alive throughout all operations and the lifetime of returned storage.
 * Callbacks define resource bounds, supported sizes/alignment, zero-size
 * behavior, initialization, ownership and execution context. Callbacks must
 * preserve an existing allocation when a nonzero resize request fails.
 * Byte clearing does not initialize arbitrary C object representations.
 */
struct hps_allocator_i {
    void *(*allocate)(void *ctx, size_t size, size_t alignment);
    void *(*allocate_zeroed)(void *ctx, size_t count, size_t size,
                             size_t alignment);
    void *(*resize)(void *ctx, void *ptr, size_t old_size, size_t new_size,
                    size_t alignment);
    void (*release)(void *ctx, void *ptr, size_t size);
    void *ctx;
};

/* Returns 0 and writes the product on success; returns 1 for overflow or NULL
 * result and preserves a non-NULL result on failure. result designates a
 * writable size_t object. O(1), no callback or allocation. */
static HPS_INLINE uint8_t hps_allocator_check_mul_overflow(
    const size_t left, const size_t right, size_t *result)
{
    uint8_t failed = UINT8_C(1);

    if (result != NULL) {
        if ((left == 0U) || (right <= (SIZE_MAX / left))) {
            *result = left * right;
            failed = UINT8_C(0);
        }
    }
    return failed;
}

/* NULL table/missing allocate returns NULL. Otherwise forwards size unchanged,
 * including zero, with the default alignment hint. Callback-owned failures and
 * returned-storage contracts apply. Wrapper work/automatic storage is O(1). */
static HPS_INLINE void *hps_allocator_alloc(const hps_allocator_t *allocator,
                                           const size_t size)
{
    void *result = NULL;

    if (allocator != NULL) {
        if (allocator->allocate != NULL) {
            result = allocator->allocate(allocator->ctx, size,
                                         HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
        }
    }
    return result;
}

/* Checks count*size before any callback. Prefers allocate_zeroed, passing the
 * original count/size; otherwise uses allocate and clears exactly the checked
 * product bytes. Missing callbacks, overflow and backend failure return NULL.
 * Zero products are forwarded as in the supplied draft. A zeroed callback must
 * initialize every requested byte to zero. The fallback requires writable byte
 * storage suitable for hps_memset; no typed-object initialization is implied.
 * O(1) wrapper work plus at most count*size writes, excluding backend work.
 */
static HPS_INLINE void *hps_allocator_calloc(const hps_allocator_t *allocator,
                                            const size_t count,
                                            const size_t size)
{
    void *result = NULL;
    size_t total_size = 0U;

    if (allocator != NULL) {
        if (hps_allocator_check_mul_overflow(count, size, &total_size) ==
            UINT8_C(0)) {
            if (allocator->allocate_zeroed != NULL) {
                result = allocator->allocate_zeroed(
                    allocator->ctx, count, size,
                    HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
            } else if (allocator->allocate != NULL) {
                result = allocator->allocate(
                    allocator->ctx, total_size,
                    HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
                if (result != NULL) {
                    (void)hps_memset(result, UINT8_C(0), total_size);
                }
            } else {
                /* An incomplete table has no allocation fallback. */
            }
        }
    }
    return result;
}

/* NULL table returns NULL. NULL ptr forwards to alloc, including new_size==0.
 * A non-NULL ptr with new_size==0 invokes release if present and returns NULL.
 * If release is absent, this NULL result does not release the original object;
 * the caller retains its owner and must arrange cleanup. Missing resize for a
 * positive size returns NULL and preserves the original allocation. Otherwise
 * forwards ptr, old_size and new_size unchanged. ptr/old_size must identify a
 * live allocation from this backend; retain the old owner until success or a
 * known release. O(1) wrapper work; backend work/resources require its contract.
 */
static HPS_INLINE void *hps_allocator_realloc(const hps_allocator_t *allocator,
                                             void *ptr, const size_t old_size,
                                             const size_t new_size)
{
    void *result = NULL;

    if (allocator != NULL) {
        if (ptr == NULL) {
            result = hps_allocator_alloc(allocator, new_size);
        } else if (new_size == 0U) {
            if (allocator->release != NULL) {
                allocator->release(allocator->ctx, ptr, old_size);
            }
        } else if (allocator->resize != NULL) {
            result = allocator->resize(allocator->ctx, ptr, old_size,
                                        new_size,
                                        HPS_ALLOCATOR_DEFAULT_ALIGNMENT);
        } else {
            /* Preserve the existing owner when no resize operation exists. */
        }
    }
    return result;
}

/* NULL table, missing release or NULL ptr is a no-op. Otherwise ptr/size must
 * identify a live allocation from the backend. A successful release invalidates
 * every alias. O(1) wrapper work; backend work/resources require its contract. */
static HPS_INLINE void hps_allocator_free(const hps_allocator_t *allocator,
                                         void *ptr, const size_t size)
{
    if ((allocator != NULL) && (ptr != NULL)) {
        if (allocator->release != NULL) {
            allocator->release(allocator->ctx, ptr, size);
        }
    }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_HPS_ALLOCATOR_H */
