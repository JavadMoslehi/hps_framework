#ifndef HPS_MEMORY_REFCOUNT_H
#define HPS_MEMORY_REFCOUNT_H

/**
 * @file hps/memory/refcount.h
 * @brief Checked reference counts for one caller-controlled execution context.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/attributes.h"

#define HPS_REFCOUNT_OK ((int32_t)INT32_C(0))
#define HPS_REFCOUNT_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_REFCOUNT_INACTIVE ((int32_t)-INT32_C(2))
#define HPS_REFCOUNT_OVERFLOW ((int32_t)-INT32_C(3))

typedef struct hps_refcount {
    uint32_t count;
} hps_refcount;

/* One counter accompanies one resource whose storage/lifetime the caller owns.
 * Initialize once before publication; do not copy or reinitialize a live count.
 * Zero is inactive and cannot be retained or released. A final release reports
 * that the caller may perform its own typed cleanup; no callback, allocation,
 * automatic destruction or validation of resource pointers occurs here.
 *
 * All access, resource publication and cleanup use one serialized execution
 * context. Task/ISR interleaving is concurrent use and is prohibited. The count
 * does not detect stale aliases or prove that each holder releases exactly once.
 * Metadata stays live until all accesses finish, including final-release output.
 * Each operation uses O(1) work/automatic storage and calls no dependencies.
 * Per counter storage is one uint32_t plus ordinary structure padding.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Set uninitialized/inactive caller-owned storage to one reference. NULL is a
 * no-op. Reinitializing requires proof that no prior resource/alias remains. */
static HPS_INLINE void hps_refcount_init(hps_refcount *counter)
{
    if (counter != NULL) {
        counter->count = UINT32_C(1);
    }
}

/* NULL returns INVALID_ARGUMENT; inactive zero returns INACTIVE; UINT32_MAX
 * returns OVERFLOW. Every error preserves the counter. */
static HPS_INLINE int32_t hps_refcount_retain(hps_refcount *counter)
{
    int32_t status = HPS_REFCOUNT_INVALID_ARGUMENT;

    if (counter != NULL) {
        if (counter->count == UINT32_C(0)) {
            status = HPS_REFCOUNT_INACTIVE;
        } else if (counter->count == UINT32_MAX) {
            status = HPS_REFCOUNT_OVERFLOW;
        } else {
            counter->count += UINT32_C(1);
            status = HPS_REFCOUNT_OK;
        }
    }
    return status;
}

/* final_release is non-NULL, writable and disjoint from counter/resource
 * storage. Success decrements a positive count and writes exactly 0 or 1; 1
 * means it became inactive. NULL arguments precede INACTIVE. Every error
 * preserves both counter and output; even an inactive counter needs a live
 * metadata object. Discard resource aliases before their storage is reused. */
static HPS_INLINE int32_t hps_refcount_release(hps_refcount *counter,
                                              uint8_t *final_release)
{
    int32_t status = HPS_REFCOUNT_INVALID_ARGUMENT;

    if ((counter != NULL) && (final_release != NULL)) {
        if (counter->count == UINT32_C(0)) {
            status = HPS_REFCOUNT_INACTIVE;
        } else {
            counter->count -= UINT32_C(1);
            *final_release = (counter->count == UINT32_C(0)) ?
                UINT8_C(1) : UINT8_C(0);
            status = HPS_REFCOUNT_OK;
        }
    }
    return status;
}

/* Borrow initialized live metadata. NULL and inactive storage both report zero;
 * this query never establishes ownership of the associated resource. */
static HPS_INLINE uint32_t hps_refcount_get(const hps_refcount *counter)
{
    uint32_t count = UINT32_C(0);

    if (counter != NULL) {
        count = counter->count;
    }
    return count;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MEMORY_REFCOUNT_H */
