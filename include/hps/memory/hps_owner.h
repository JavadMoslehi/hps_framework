#ifndef HPS_MEMORY_HPS_OWNER_H
#define HPS_MEMORY_HPS_OWNER_H

/**
 * @file hps/memory/hps_owner.h
 * @brief Optional typed manual ownership generator for explicit allocators.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/hps_attributes.h"
#include "hps/memory/hps_allocator.h"
#include "hps/memory/hps_array.h"

#define HPS_MEMORY_OWNER_OK ((int32_t)INT32_C(0))
#define HPS_MEMORY_OWNER_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_MEMORY_OWNER_SIZE_OVERFLOW ((int32_t)-INT32_C(2))
#define HPS_MEMORY_OWNER_NOT_EMPTY ((int32_t)-INT32_C(3))
#define HPS_MEMORY_OWNER_RELEASE_UNAVAILABLE ((int32_t)-INT32_C(4))

/* DEVELOPMENT PROPOSAL, outside default production core paths under the same
 * pending HPS-MEM-001 allocator-hook disposition as memory/allocator.h. No heap
 * provider or pool adapter is supplied. This is explicit manual ownership;
 * scope exit performs no cleanup, on any compiler. No compiler-cleanup fallback
 * or attribute is hidden behind the generator.
 *
 * Define once per translation unit using a unique hps_-prefixed name and a
 * complete writable object type whose storage/alignment the provider supports.
 * Expansion declares name { type *pointer; size_t count;
 * const hps_allocator_t *allocator; } and static inline functions:
 *
 * void name_init(name *owner)
 * int32_t name_adopt(name *owner, type *pointer, size_t count,
 *                   const hps_allocator_t *allocator)
 * int32_t name_reset(name *owner)
 * type *name_release(name *owner, size_t *count,
 *                    const hps_allocator_t **allocator)
 *
 * init establishes an empty owner, and accepts NULL as a no-op. It is only for
 * previously uninitialized or empty storage, never a live owner. adopt requires
 * an initialized empty owner, positive count, a live exclusively owned typed
 * array, checked count*sizeof(type), and an allocator with release. Table/context
 * outlive the owner. The array and metadata are disjoint. Invalid arguments take
 * priority over NOT_EMPTY, unavailable release and overflow, in that order.
 * Every failing adopt/reset preserves the owner and resource. Once adopted,
 * modify owner metadata only through these functions; do not copy a live owner.
 *
 * reset releases a nonempty array exactly once then empties the owner; empty
 * reset succeeds without callbacks. Release-unavailable/overflow errors preserve
 * the live pointer. The provider's release must complete without failure.
 * release transfers ownership to the caller and empties the owner without a
 * callback. Its count/allocator outputs must be non-NULL, writable, mutually
 * disjoint and disjoint from owner/array storage. NULL arguments return NULL
 * without mutation; a valid empty owner writes zero/NULL and returns NULL.
 * Caller uses the transferred count and table for eventual explicit cleanup.
 *
 * Every alias becomes invalid after reset; release transfers that responsibility.
 * Initialized owner metadata and actual storage/type/lifetime remain caller
 * proofs. No arbitrary pointer validation or concurrent use is supported.
 * Per owner: one typed pointer, size_t and allocator pointer; functions use O(1)
 * automatic storage/work excluding provider release. Token generation and this
 * optional ownership abstraction remain subject to their pending policy review.
 */
#define HPS_MEMORY_OWNER_DEFINE(name, element_type) \
    typedef struct name { \
        element_type *pointer; \
        size_t count; \
        const hps_allocator_t *allocator; \
    } name; \
    static HPS_INLINE void name##_init(name *owner) \
    { \
        if (owner != NULL) { \
            owner->pointer = NULL; \
            owner->count = 0U; \
            owner->allocator = NULL; \
        } \
    } \
    static HPS_INLINE int32_t name##_adopt( \
        name *owner, element_type *pointer, size_t count, \
        const hps_allocator_t *allocator) \
    { \
        int32_t status = HPS_MEMORY_OWNER_INVALID_ARGUMENT; \
        size_t size = 0U; \
        if ((owner != NULL) && (pointer != NULL) && (count != 0U) && \
            (allocator != NULL)) { \
            if (owner->pointer != NULL) { \
                status = HPS_MEMORY_OWNER_NOT_EMPTY; \
            } else if (allocator->release == NULL) { \
                status = HPS_MEMORY_OWNER_RELEASE_UNAVAILABLE; \
            } else if (hps_memory_array_size(count, sizeof(element_type), \
                                             &size) != HPS_MEMORY_ARRAY_OK) { \
                status = HPS_MEMORY_OWNER_SIZE_OVERFLOW; \
            } else { \
                owner->pointer = pointer; \
                owner->count = count; \
                owner->allocator = allocator; \
                status = HPS_MEMORY_OWNER_OK; \
            } \
        } \
        return status; \
    } \
    static HPS_INLINE int32_t name##_reset(name *owner) \
    { \
        int32_t status = HPS_MEMORY_OWNER_INVALID_ARGUMENT; \
        size_t size = 0U; \
        if (owner != NULL) { \
            if (owner->pointer == NULL) { \
                name##_init(owner); \
                status = HPS_MEMORY_OWNER_OK; \
            } else if ((owner->allocator == NULL) || \
                       (owner->allocator->release == NULL)) { \
                status = HPS_MEMORY_OWNER_RELEASE_UNAVAILABLE; \
            } else if (hps_memory_array_size(owner->count, \
                         sizeof(element_type), &size) != HPS_MEMORY_ARRAY_OK) { \
                status = HPS_MEMORY_OWNER_SIZE_OVERFLOW; \
            } else { \
                hps_allocator_free(owner->allocator, owner->pointer, size); \
                name##_init(owner); \
                status = HPS_MEMORY_OWNER_OK; \
            } \
        } \
        return status; \
    } \
    static HPS_INLINE element_type *name##_release( \
        name *owner, size_t *count, const hps_allocator_t **allocator) \
    { \
        element_type *pointer = NULL; \
        if ((owner != NULL) && (count != NULL) && (allocator != NULL)) { \
            pointer = owner->pointer; \
            *count = owner->count; \
            *allocator = owner->allocator; \
            name##_init(owner); \
        } \
        return pointer; \
    }

#endif /* HPS_MEMORY_HPS_OWNER_H */
