#ifndef HPS_BYTES_VIEW_H
#define HPS_BYTES_VIEW_H

/**
 * @file hps/bytes/view.h
 * @brief Borrowed, length-bounded read-only byte sequences.
 */

#include <stddef.h>
#include <stdint.h>

/* Local int32_t results: zero is success; negative values identify byte errors.
 * These codes belong to the bytes API and are not globally unique. */
#define HPS_BYTE_OK ((int32_t)INT32_C(0))
#define HPS_BYTE_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_BYTE_OUT_OF_RANGE ((int32_t)-INT32_C(2))
#define HPS_BYTE_NO_SPACE ((int32_t)-INT32_C(3))
#define HPS_BYTE_INVALID_STATE ((int32_t)-INT32_C(4))

typedef struct {
    const uint8_t *data;
    size_t length;
} hps_byte_view;

#ifdef __cplusplus
extern "C" {
#endif

/* Module contract: nonempty views refer to at least length initialized, readable
 * uint8_t elements in one live array. NULL data is valid only with length zero.
 * All descriptor/scalar output pointers designate writable objects of their
 * declared type and must not overlap any referenced payload storage. Views are
 * borrowed: no ownership, allocation, copying at construction or lifetime check.
 * const prevents writes through this API, not writes through another alias.
 * Caller serializes access against mutation/release of the backing storage.
 * There is no NUL, encoding, synchronization or secret-comparison guarantee.
 * Functions return int32_t results using the HPS_BYTE_* constants above.
 * Failure leaves all outputs unchanged. No arbitrary pointer/extent validation.
 */

/* Writes result on success; NULL data with positive length is INVALID_ARGUMENT.
 * Every empty result is normalized to {NULL, 0}. O(1), no payload access. */
int32_t hps_byte_view_init(hps_byte_view *result,
                           const uint8_t *data, size_t length);

/* offset <= view.length and length <= view.length-offset, otherwise OUT_OF_RANGE.
 * Writes a borrowed slice, normalizing an empty slice without pointer arithmetic.
 * Input descriptor is passed by value, so result may be its original descriptor.
 * Malformed view/NULL result is INVALID_ARGUMENT. O(1), no payload access. */
int32_t hps_byte_view_slice(hps_byte_view view, size_t offset,
                            size_t length, hps_byte_view *result);

/* index < length; reads exactly one byte into result. OUT_OF_RANGE on bad index,
 * INVALID_ARGUMENT on malformed view/NULL result. O(1). */
int32_t hps_byte_view_read_u8(hps_byte_view view, size_t index,
                              uint8_t *result);

/* Writes exactly 0 or 1. INVALID_ARGUMENT on malformed views/NULL result.
 * O(minimum length) worst-case; returns early on different lengths/bytes.
 * This is not a constant-time comparison for secrets. */
int32_t hps_byte_view_equal(hps_byte_view left, hps_byte_view right,
                            uint8_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BYTES_VIEW_H */
