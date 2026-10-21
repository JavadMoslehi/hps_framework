#ifndef HPS_BYTES_HPS_VIEW_H
#define HPS_BYTES_HPS_VIEW_H

/**
 * @file hps/bytes/hps_view.h
 * @brief Borrowed, length-bounded read-only byte sequences.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"

/* Local int32_t results: zero is success; negative values identify byte errors.
 * These codes belong to the bytes API and are not globally unique. */
#define HPS_BYTE_OK ((int32_t)INT32_C(0))
#define HPS_BYTE_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_BYTE_OUT_OF_RANGE ((int32_t)-INT32_C(2))
#define HPS_BYTE_NO_SPACE ((int32_t)-INT32_C(3))
#define HPS_BYTE_INVALID_STATE ((int32_t)-INT32_C(4))
#define HPS_BYTE_NPOS SIZE_MAX

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

/* Compare in unsigned-byte lexicographic order, writing exactly -1, 0 or 1.
 * A matching prefix sorts before its longer extension. NULL result/malformed
 * input gives INVALID_ARGUMENT. Inputs may share/overlap readable payloads;
 * result must be disjoint from payloads. O(minimum length), early mismatch exit.
 * Like equal, this is unsuitable for comparing secrets. */
int32_t hps_byte_view_compare(hps_byte_view left, hps_byte_view right,
                              int32_t *result);

/* Write exactly 0 or 1 for a prefix/suffix match. An empty pattern matches every
 * valid view; a longer pattern does not match and reads no payload. Shared or
 * overlapping inputs are permitted. NULL result/malformed input gives
 * INVALID_ARGUMENT. O(pattern.length), with early mismatch exit. */
int32_t hps_byte_view_starts_with(hps_byte_view view, hps_byte_view prefix,
                                  uint8_t *result);
int32_t hps_byte_view_ends_with(hps_byte_view view, hps_byte_view suffix,
                                uint8_t *result);

/* Search the whole view, writing the first/last matching start index, or NPOS
 * for no match. Absence is a successful result. An empty needle never matches,
 * preserving the legacy byte-buffer search contract; this differs from String view
 * search. Inputs may overlap; result must be disjoint from both payloads.
 * NULL result/malformed inputs give INVALID_ARGUMENT and preserve result.
 * Empty/too-long needles read no payload. NPOS cannot be a valid element index,
 * including when a caller proves an actual SIZE_MAX-element view. No index is
 * narrowed to ptrdiff_t or a basic numerical type. Worst-case work is bounded
 * by view.length candidate starts times needle.length byte comparisons; O(1)
 * automatic scalar storage, no payload scratch or allocation. */
int32_t hps_byte_view_find(hps_byte_view view, hps_byte_view needle,
                           size_t *result);
int32_t hps_byte_view_rfind(hps_byte_view view, hps_byte_view needle,
                            size_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BYTES_HPS_VIEW_H */
