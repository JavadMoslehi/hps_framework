#ifndef HPS_BYTES_VIEW_H
#define HPS_BYTES_VIEW_H

/**
 * @file hps/bytes/view.h
 * @brief Borrowed, length-bounded read-only byte sequences.
 */

#include <stddef.h>
#include <stdint.h>

typedef uint8_t hps_byte_status;
#define HPS_BYTE_OK ((hps_byte_status)0U)
#define HPS_BYTE_INVALID_ARGUMENT ((hps_byte_status)1U)
#define HPS_BYTE_OUT_OF_RANGE ((hps_byte_status)2U)
#define HPS_BYTE_NO_SPACE ((hps_byte_status)3U)
#define HPS_BYTE_INVALID_STATE ((hps_byte_status)4U)

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
 * Failure leaves all outputs unchanged. No arbitrary pointer/extent validation.
 */

/* Writes result on success; NULL data with positive length is INVALID_ARGUMENT.
 * Every empty result is normalized to {NULL, 0}. O(1), no payload access. */
hps_byte_status hps_byte_view_init(hps_byte_view *result,
                                   const uint8_t *data, size_t length);

/* offset <= view.length and length <= view.length-offset, otherwise OUT_OF_RANGE.
 * Writes a borrowed slice, normalizing an empty slice without pointer arithmetic.
 * Input descriptor is passed by value, so result may be its original descriptor.
 * Malformed view/NULL result is INVALID_ARGUMENT. O(1), no payload access. */
hps_byte_status hps_byte_view_slice(hps_byte_view view, size_t offset,
                                    size_t length, hps_byte_view *result);

/* index < length; reads exactly one byte into result. OUT_OF_RANGE on bad index,
 * INVALID_ARGUMENT on malformed view/NULL result. O(1). */
hps_byte_status hps_byte_view_read_u8(hps_byte_view view, size_t index,
                                    uint8_t *result);

/* Writes exactly 0 or 1. INVALID_ARGUMENT on malformed views/NULL result.
 * O(minimum length) worst-case; returns early on different lengths/bytes.
 * This is not a constant-time comparison for secrets. */
hps_byte_status hps_byte_view_equal(hps_byte_view left, hps_byte_view right,
                                    uint8_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BYTES_VIEW_H */
