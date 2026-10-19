#include "hps/bytes/view.h"

/**
 * @file src/bytes/view.c
 * @brief Checked construction, slicing, ordering and searching of borrowed bytes.
 */

static int32_t view_check(const hps_byte_view view)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if ((view.data != NULL) || (view.length == 0U)) {
        status = HPS_BYTE_OK;
    }
    return status;
}

/* Callers first prove count <= left.length-offset and count <= right.length.
 * No pointer is formed for an empty range. One typed comparison engine keeps
 * equality, ordering, prefix/suffix and substring matching consistent. */
static int32_t view_compare_range(const hps_byte_view left, const size_t offset,
                                   const hps_byte_view right, const size_t count)
{
    size_t index = 0U;
    int32_t order = INT32_C(0);

    while ((index < count) && (order == INT32_C(0))) {
        if (left.data[offset + index] < right.data[index]) {
            order = -INT32_C(1);
        } else if (left.data[offset + index] > right.data[index]) {
            order = INT32_C(1);
        } else {
            /* Equal bytes leave the next comparison relevant. */
        }
        index += 1U;
    }
    return order;
}

int32_t hps_byte_view_init(hps_byte_view *result,
                           const uint8_t *data, const size_t length)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if ((result != NULL) && ((data != NULL) || (length == 0U))) {
        result->data = (length == 0U) ? NULL : data;
        result->length = length;
        status = HPS_BYTE_OK;
    }
    return status;
}

int32_t hps_byte_view_slice(const hps_byte_view view, const size_t offset,
                            const size_t length, hps_byte_view *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = view_check(view);
        if (status == HPS_BYTE_OK) {
            status = HPS_BYTE_OUT_OF_RANGE;
            if (offset <= view.length) {
                if (length <= (view.length - offset)) {
                    result->data = (length == 0U) ? NULL : &view.data[offset];
                    result->length = length;
                    status = HPS_BYTE_OK;
                }
            }
        }
    }
    return status;
}

int32_t hps_byte_view_read_u8(const hps_byte_view view, const size_t index,
                              uint8_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = view_check(view);
        if (status == HPS_BYTE_OK) {
            if (index >= view.length) {
                status = HPS_BYTE_OUT_OF_RANGE;
            } else {
                *result = view.data[index];
            }
        }
    }
    return status;
}

int32_t hps_byte_view_equal(const hps_byte_view left,
                            const hps_byte_view right, uint8_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;
    if (result != NULL) {
        status = view_check(left);
        if (status == HPS_BYTE_OK) {
            status = view_check(right);
            if (status == HPS_BYTE_OK) {
                uint8_t equal = 0U;
                if (left.length == right.length) {
                    equal = (uint8_t)(view_compare_range(left, 0U, right,
                        left.length) == INT32_C(0));
                }
                *result = equal;
            }
        }
    }
    return status;
}

int32_t hps_byte_view_compare(const hps_byte_view left,
                              const hps_byte_view right, int32_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;

    if (result != NULL) {
        status = view_check(left);
        if (status == HPS_BYTE_OK) {
            status = view_check(right);
        }
        if (status == HPS_BYTE_OK) {
            const size_t count = (left.length < right.length) ?
                left.length : right.length;
            int32_t order = view_compare_range(left, 0U, right, count);
            if (order == INT32_C(0)) {
                if (left.length < right.length) {
                    order = -INT32_C(1);
                } else if (left.length > right.length) {
                    order = INT32_C(1);
                } else {
                    /* Matching bytes and lengths compare equal. */
                }
            }
            *result = order;
        }
    }
    return status;
}

/* Prefix and suffix share all validation, empty-pattern and failure contracts;
 * suffix differs only in its checked comparison offset. */
static int32_t view_matches_edge(const hps_byte_view view,
                                   const hps_byte_view pattern,
                                   const uint8_t suffix, uint8_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;

    if (result != NULL) {
        status = view_check(view);
        if (status == HPS_BYTE_OK) {
            status = view_check(pattern);
        }
        if (status == HPS_BYTE_OK) {
            uint8_t matches = 0U;
            if (pattern.length <= view.length) {
                const size_t offset = (suffix == 1U) ?
                    view.length - pattern.length : 0U;
                matches = (uint8_t)(view_compare_range(view, offset, pattern,
                    pattern.length) == INT32_C(0));
            }
            *result = matches;
        }
    }
    return status;
}

int32_t hps_byte_view_starts_with(const hps_byte_view view,
                                  const hps_byte_view prefix, uint8_t *result)
{
    return view_matches_edge(view, prefix, 0U, result);
}

int32_t hps_byte_view_ends_with(const hps_byte_view view,
                                const hps_byte_view suffix, uint8_t *result)
{
    return view_matches_edge(view, suffix, 1U, result);
}

/* Both search directions share argument, no-match and match contracts. A
 * nonempty needle proves the candidate count fits size_t even at SIZE_MAX.
 * Reverse search decrements only positive remaining, avoiding unsigned wrap. */
static int32_t view_find(const hps_byte_view view,
                           const hps_byte_view needle, const uint8_t reverse,
                           size_t *result)
{
    int32_t status = HPS_BYTE_INVALID_ARGUMENT;

    if (result != NULL) {
        status = view_check(view);
        if (status == HPS_BYTE_OK) {
            status = view_check(needle);
        }
        if (status == HPS_BYTE_OK) {
            size_t found = HPS_BYTE_NPOS;
            if ((needle.length != 0U) && (needle.length <= view.length)) {
                const size_t candidates = view.length - needle.length + 1U;
                size_t remaining = candidates;
                while ((remaining != 0U) && (found == HPS_BYTE_NPOS)) {
                    remaining -= 1U;
                    /* remaining < candidates proves both forward subtractions;
                     * every offset is <= view.length-needle.length. */
                    const size_t offset = (reverse == 1U) ?
                        remaining : candidates - remaining - 1U;
                    if (view_compare_range(view, offset, needle,
                        needle.length) == INT32_C(0)) {
                        found = offset;
                    }
                }
            }
            *result = found;
        }
    }
    return status;
}

int32_t hps_byte_view_find(const hps_byte_view view,
                           const hps_byte_view needle, size_t *result)
{
    return view_find(view, needle, 0U, result);
}

int32_t hps_byte_view_rfind(const hps_byte_view view,
                            const hps_byte_view needle, size_t *result)
{
    return view_find(view, needle, 1U, result);
}
