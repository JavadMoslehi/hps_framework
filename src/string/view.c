#include "hps/string/hps_view.h"
#include "../internal/text.h"
#include <string.h>

/**
 * @file src/string/view.c
 * @brief Extent-checked borrowed text operations and exact C-string export.
 */

int32_t hps_string_view_init(hps_string_view *result,
                             const char *data, const size_t length)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    const hps_string_view input = {data, length};
    if (result != NULL) {
        status = hps_string_check_view(input);
        if (status == HPS_STRING_OK) {
            result->data = (length == 0U) ? NULL : data;
            result->length = length;
        }
    }
    return status;
}

int32_t hps_string_view_init_cstr(hps_string_view *result,
                                  const char *data,
                                  const size_t capacity)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (data != NULL) && (capacity != 0U)) {
        size_t length = 0U;
        status = HPS_STRING_NOT_TERMINATED;
        while ((length < capacity) && (status == HPS_STRING_NOT_TERMINATED)) {
            if (data[length] == '\0') {
                status = hps_string_view_init(result, data, length);
            } else {
                length += 1U;
            }
        }
    }
    return status;
}

int32_t hps_string_view_slice(const hps_string_view view,
                              const size_t offset, const size_t count,
                              hps_string_view *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            status = HPS_STRING_OUT_OF_RANGE;
            if ((offset <= view.length) && (count <= (view.length - offset))) {
                result->data = (count == 0U) ? NULL : &view.data[offset];
                result->length = count;
                status = HPS_STRING_OK;
            }
        }
    }
    return status;
}

int32_t hps_string_view_equal(const hps_string_view left,
                              const hps_string_view right,
                              uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(left);
        if (status == HPS_STRING_OK) {
            status = hps_string_check_view(right);
            if (status == HPS_STRING_OK) {
                uint8_t equal = 0U;
                if (left.length == right.length) {
                    equal = (left.length == 0U) ? 1U :
                        (uint8_t)(memcmp(left.data, right.data, left.length) == 0);
                }
                *result = equal;
            }
        }
    }
    return status;
}

int32_t hps_string_view_compare(const hps_string_view left,
                                const hps_string_view right,
                                int32_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(left);
        if (status == HPS_STRING_OK) {
            status = hps_string_check_view(right);
            if (status == HPS_STRING_OK) {
                const size_t common = (left.length < right.length) ?
                    left.length : right.length;
                size_t index = 0U;
                int32_t comparison = INT32_C(0);
                while ((index < common) && (comparison == INT32_C(0))) {
                    if (left.data[index] != right.data[index]) {
                        /* Normalize the sign directly: an external int result
                         * need not be representable in int32_t on every target. */
                        comparison = (memcmp(&left.data[index],
                                             &right.data[index], 1U) > 0) ?
                            INT32_C(1) : -INT32_C(1);
                    }
                    index += 1U;
                }
                if (comparison == INT32_C(0)) {
                    if (left.length < right.length) {
                        comparison = -INT32_C(1);
                    } else if (left.length > right.length) {
                        comparison = INT32_C(1);
                    } else {
                        /* Equal lengths and payloads retain zero. */
                    }
                }
                *result = comparison;
            }
        }
    }
    return status;
}

/* Both edge predicates have the same validation/output and bounded-copy
 * contracts; from_end is private and always 0 or 1. */
static int32_t view_match_edge(const hps_string_view view,
                               const hps_string_view pattern,
                               const uint8_t from_end,
                               uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            status = hps_string_check_view(pattern);
            if (status == HPS_STRING_OK) {
                uint8_t match = 0U;
                if (pattern.length <= view.length) {
                    const size_t offset = (from_end == 1U) ?
                        (view.length - pattern.length) : 0U;
                    match = (pattern.length == 0U) ? 1U :
                        (uint8_t)(memcmp(&view.data[offset], pattern.data,
                                        pattern.length) == 0);
                }
                *result = match;
            }
        }
    }
    return status;
}

int32_t hps_string_view_starts_with(const hps_string_view view,
                                    const hps_string_view prefix,
                                    uint8_t *result)
{
    return view_match_edge(view, prefix, 0U, result);
}

int32_t hps_string_view_ends_with(const hps_string_view view,
                                  const hps_string_view suffix,
                                  uint8_t *result)
{
    return view_match_edge(view, suffix, 1U, result);
}

int32_t hps_string_view_find(const hps_string_view view,
                             const hps_string_view needle,
                             const size_t offset, size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            status = hps_string_check_view(needle);
            if (status == HPS_STRING_OK) {
                if (offset > view.length) {
                    status = HPS_STRING_OUT_OF_RANGE;
                } else {
                    size_t found = HPS_STRING_NPOS;
                    if (needle.length == 0U) {
                        found = offset;
                    } else if (needle.length <= (view.length - offset)) {
                        const size_t last = view.length - needle.length;
                        size_t position = offset;
                        /* Nonempty needle proves last < SIZE_MAX, so the final
                         * increment cannot wrap even when there is no match. */
                        while ((position <= last) && (found == HPS_STRING_NPOS)) {
                            if (memcmp(&view.data[position], needle.data,
                                       needle.length) == 0) {
                                found = position;
                            }
                            position += 1U;
                        }
                    }
                    *result = found;
                }
            }
        }
    }
    return status;
}

int32_t hps_string_view_rfind(const hps_string_view view,
                              const hps_string_view needle,
                              const size_t position, size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            status = hps_string_check_view(needle);
            if (status == HPS_STRING_OK) {
                size_t found = HPS_STRING_NPOS;
                if (needle.length == 0U) {
                    found = (position < view.length) ? position : view.length;
                } else if (needle.length <= view.length) {
                    const size_t last = view.length - needle.length;
                    size_t index = (position < last) ? position : last;
                    uint8_t exhausted = 0U;
                    while ((exhausted == 0U) && (found == HPS_STRING_NPOS)) {
                        if (memcmp(&view.data[index], needle.data,
                                   needle.length) == 0) {
                            found = index;
                        }
                        if (index == 0U) {
                            exhausted = 1U;
                        } else {
                            index -= 1U;
                        }
                    }
                }
                *result = found;
            }
        }
    }
    return status;
}

int32_t hps_string_view_contains(const hps_string_view view,
                                 const hps_string_view needle,
                                 uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        size_t found = HPS_STRING_NPOS;
        status = hps_string_view_find(view, needle, 0U, &found);
        if (status == HPS_STRING_OK) {
            *result = (uint8_t)(found != HPS_STRING_NPOS);
        }
    }
    return status;
}

/* Only identity comparisons on character storage; no scalar char arithmetic. */
static uint8_t view_character_in_set(const char *character,
                                     const hps_string_view chars)
{
    size_t index = 0U;
    uint8_t found = 0U;
    while ((index < chars.length) && (found == 0U)) {
        if (*character == chars.data[index]) {
            found = 1U;
        }
        index += 1U;
    }
    return found;
}

int32_t hps_string_view_trim(const hps_string_view view,
                             const hps_string_view chars,
                             hps_string_view *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            status = hps_string_check_view(chars);
            if (status == HPS_STRING_OK) {
                size_t start = 0U;
                size_t end = view.length;
                while ((start < end) &&
                       (view_character_in_set(&view.data[start], chars) == 1U)) {
                    start += 1U;
                }
                while ((start < end) &&
                       (view_character_in_set(&view.data[end - 1U], chars) == 1U)) {
                    end -= 1U;
                }
                status = hps_string_view_slice(view, start, end - start, result);
            }
        }
    }
    return status;
}

int32_t hps_string_view_trim_whitespace(const hps_string_view view,
                                        hps_string_view *result)
{
    const hps_string_view whitespace = {" \t\n\r\v\f", 6U};
    return hps_string_view_trim(view, whitespace, result);
}

int32_t hps_string_view_copy_cstr(const hps_string_view view,
                                  char *output,
                                  const size_t capacity)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (output != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            if (view.length >= capacity) {
                status = HPS_STRING_NO_SPACE;
            } else if (hps_string_has_nul(view) == 1U) {
                status = HPS_STRING_INVALID_ARGUMENT;
            } else {
                if (view.length != 0U) {
                    (void)memmove(output, view.data, view.length);
                }
                output[view.length] = '\0';
            }
        }
    }
    return status;
}
int32_t hps_string_view_at(const hps_string_view view,
                           const size_t index, uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            if (index >= view.length) { status = HPS_STRING_OUT_OF_RANGE; }
            else { *result = (uint8_t)view.data[index]; }
        }
    }
    return status;
}

int32_t hps_string_view_front(const hps_string_view view, uint8_t *result)
{
    return hps_string_view_at(view, 0U, result);
}

int32_t hps_string_view_back(const hps_string_view view, uint8_t *result)
{
    const size_t index = (view.length == 0U) ? 0U : view.length - 1U;
    return hps_string_view_at(view, index, result);
}

uint8_t hps_string_view_is_valid(const hps_string_view view)
{
    return (uint8_t)(hps_string_check_view(view) == HPS_STRING_OK);
}

int32_t hps_string_view_data(const hps_string_view view, const char * *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) { *result = (view.length == 0U) ? NULL : view.data; }
    }
    return status;
}

int32_t hps_string_view_length(const hps_string_view view, size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) { *result = view.length; }
    }
    return status;
}

int32_t hps_string_view_is_empty(const hps_string_view view, uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) { *result = (uint8_t)(view.length == 0U); }
    }
    return status;
}

int32_t hps_string_view_left(const hps_string_view view,
                             const size_t count, hps_string_view *result)
{
    return hps_string_view_slice(view, 0U, count, result);
}

int32_t hps_string_view_right(const hps_string_view view,
                              const size_t count, hps_string_view *result)
{
    int32_t status = hps_string_check_view(view);
    if (result == NULL) { status = HPS_STRING_INVALID_ARGUMENT; }
    if (status == HPS_STRING_OK) {
        if (count > view.length) { status = HPS_STRING_OUT_OF_RANGE; }
        else { status = hps_string_view_slice(view, view.length - count, count, result); }
    }
    return status;
}

static int32_t view_remove(hps_string_view *view, const size_t count,
                           const uint8_t prefix)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (view != NULL) {
        status = hps_string_check_view(*view);
        if (status == HPS_STRING_OK) {
            if (count > view->length) { status = HPS_STRING_OUT_OF_RANGE; }
            else {
                status = hps_string_view_slice(*view, (prefix == 1U) ? count : 0U,
                                              view->length - count, view);
            }
        }
    }
    return status;
}

int32_t hps_string_view_remove_prefix(hps_string_view *view, const size_t count)
{
    return view_remove(view, count, 1U);
}

int32_t hps_string_view_remove_suffix(hps_string_view *view, const size_t count)
{
    return view_remove(view, count, 0U);
}

static int32_t view_trim_char(const hps_string_view view,
                              const uint8_t value, const uint8_t prefix,
                              const uint8_t suffix, hps_string_view *result)
{
    int32_t status = hps_string_check_view(view);
    if (result == NULL) { status = HPS_STRING_INVALID_ARGUMENT; }
    if (status == HPS_STRING_OK) {
        size_t start = 0U;
        size_t end = view.length;
        while ((prefix == 1U) && (start < end) && ((uint8_t)view.data[start] == value)) { start += 1U; }
        while ((suffix == 1U) && (start < end) && ((uint8_t)view.data[end - 1U] == value)) { end -= 1U; }
        status = hps_string_view_slice(view, start, end - start, result);
    }
    return status;
}

int32_t hps_string_view_trim_prefix_char(hps_string_view *view, const uint8_t value)
{
    return (view == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        view_trim_char(*view, value, 1U, 0U, view);
}

int32_t hps_string_view_trim_suffix_char(hps_string_view *view, const uint8_t value)
{
    return (view == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        view_trim_char(*view, value, 0U, 1U, view);
}

int32_t hps_string_view_trim_char(const hps_string_view view,
                                  const uint8_t value, hps_string_view *result)
{
    return view_trim_char(view, value, 1U, 1U, result);
}

int32_t hps_string_view_copy(const hps_string_view view, char *output,
                             const size_t capacity, const size_t count,
                             const size_t offset, size_t *copied)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((copied != NULL) && ((output != NULL) || (capacity == 0U))) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            if ((offset > view.length) || (count > view.length - offset)) { status = HPS_STRING_OUT_OF_RANGE; }
            else if (count > capacity) { status = HPS_STRING_NO_SPACE; }
            else {
                if (count != 0U) { (void)memmove(output, &view.data[offset], count); }
                *copied = count;
            }
        }
    }
    return status;
}

static int32_t view_find_byte(const hps_string_view view, const uint8_t value,
                              const size_t position, const uint8_t reverse,
                              size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) {
            size_t found = HPS_STRING_NPOS;
            if ((reverse == 0U) && (position > view.length)) { status = HPS_STRING_OUT_OF_RANGE; }
            else if (view.length != 0U) {
                size_t index = (reverse == 1U) ?
                    ((position < view.length) ? position : view.length - 1U) : position;
                uint8_t done = 0U;
                while ((index < view.length) && (done == 0U)) {
                    if ((uint8_t)view.data[index] == value) { found = index; done = 1U; }
                    if (reverse == 1U) {
                        if (index == 0U) { done = 1U; } else { index -= 1U; }
                    } else { index += 1U; }
                }
            } else { /* Empty input has no character match. */ }
            if (status == HPS_STRING_OK) { *result = found; }
        }
    }
    return status;
}

int32_t hps_string_view_find_char(const hps_string_view view, const uint8_t value,
                                  const size_t offset, size_t *result)
{
    return view_find_byte(view, value, offset, 0U, result);
}

int32_t hps_string_view_rfind_char(const hps_string_view view, const uint8_t value,
                                   const size_t position, size_t *result)
{
    return view_find_byte(view, value, position, 1U, result);
}

int32_t hps_string_view_contains_char(const hps_string_view view,
                                      const uint8_t value, uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        size_t found = HPS_STRING_NPOS;
        status = hps_string_view_find_char(view, value, 0U, &found);
        if (status == HPS_STRING_OK) { *result = (uint8_t)(found != HPS_STRING_NPOS); }
    }
    return status;
}

static int32_t view_find_set(const hps_string_view view,
                             const hps_string_view chars, const size_t position,
                             const uint8_t reverse, const uint8_t want_member,
                             size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        status = hps_string_check_view(view);
        if (status == HPS_STRING_OK) { status = hps_string_check_view(chars); }
        if (status == HPS_STRING_OK) {
            size_t found = HPS_STRING_NPOS;
            if ((reverse == 0U) && (position > view.length)) { status = HPS_STRING_OUT_OF_RANGE; }
            else if (view.length != 0U) {
                size_t index = (reverse == 1U) ?
                    ((position < view.length) ? position : view.length - 1U) : position;
                uint8_t done = 0U;
                while ((index < view.length) && (done == 0U)) {
                    if (view_character_in_set(&view.data[index], chars) == want_member) {
                        found = index; done = 1U;
                    }
                    if (reverse == 1U) {
                        if (index == 0U) { done = 1U; } else { index -= 1U; }
                    } else { index += 1U; }
                }
            } else { /* Empty input has no character match. */ }
            if (status == HPS_STRING_OK) { *result = found; }
        }
    }
    return status;
}

int32_t hps_string_view_find_first_of(const hps_string_view view,
                                      const hps_string_view chars,
                                      const size_t position, size_t *result)
{
    return view_find_set(view, chars, position, 0U, 1U, result);
}

int32_t hps_string_view_find_first_not_of(const hps_string_view view,
                                          const hps_string_view chars,
                                          const size_t position, size_t *result)
{
    return view_find_set(view, chars, position, 0U, 0U, result);
}

int32_t hps_string_view_find_last_of(const hps_string_view view,
                                     const hps_string_view chars,
                                     const size_t position, size_t *result)
{
    return view_find_set(view, chars, position, 1U, 1U, result);
}

int32_t hps_string_view_find_last_not_of(const hps_string_view view,
                                         const hps_string_view chars,
                                         const size_t position, size_t *result)
{
    return view_find_set(view, chars, position, 1U, 0U, result);
}

int32_t hps_string_view_equals_cstr(const hps_string_view view,
                                    const char *text, const size_t capacity,
                                    uint8_t *result)
{
    hps_string_view other = {NULL, 0U};
    int32_t status = (result == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_check_view(view);
    if (status == HPS_STRING_OK) { status = hps_string_view_init_cstr(&other, text, capacity); }
    if (status == HPS_STRING_OK) { status = hps_string_view_equal(view, other, result); }
    return status;
}

int32_t hps_string_view_starts_with_cstr(const hps_string_view view,
                                         const char *text, const size_t capacity,
                                         uint8_t *result)
{
    hps_string_view other = {NULL, 0U};
    int32_t status = (result == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_check_view(view);
    if (status == HPS_STRING_OK) { status = hps_string_view_init_cstr(&other, text, capacity); }
    if (status == HPS_STRING_OK) { status = hps_string_view_starts_with(view, other, result); }
    return status;
}

int32_t hps_string_view_ends_with_cstr(const hps_string_view view,
                                       const char *text, const size_t capacity,
                                       uint8_t *result)
{
    hps_string_view other = {NULL, 0U};
    int32_t status = (result == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_check_view(view);
    if (status == HPS_STRING_OK) { status = hps_string_view_init_cstr(&other, text, capacity); }
    if (status == HPS_STRING_OK) { status = hps_string_view_ends_with(view, other, result); }
    return status;
}

int32_t hps_string_view_contains_cstr(const hps_string_view view,
                                      const char *text, const size_t capacity,
                                      uint8_t *result)
{
    hps_string_view other = {NULL, 0U};
    int32_t status = (result == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_check_view(view);
    if (status == HPS_STRING_OK) { status = hps_string_view_init_cstr(&other, text, capacity); }
    if (status == HPS_STRING_OK) { status = hps_string_view_contains(view, other, result); }
    return status;
}

int32_t hps_string_view_find_cstr(const hps_string_view view,
                                  const char *text, const size_t capacity,
                                  const size_t position, size_t *result)
{
    hps_string_view other = {NULL, 0U};
    int32_t status = (result == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_check_view(view);
    if (status == HPS_STRING_OK) { status = hps_string_view_init_cstr(&other, text, capacity); }
    if (status == HPS_STRING_OK) { status = hps_string_view_find(view, other, position, result); }
    return status;
}

int32_t hps_string_view_rfind_cstr(const hps_string_view view,
                                   const char *text, const size_t capacity,
                                   const size_t position, size_t *result)
{
    hps_string_view other = {NULL, 0U};
    int32_t status = (result == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_check_view(view);
    if (status == HPS_STRING_OK) { status = hps_string_view_init_cstr(&other, text, capacity); }
    if (status == HPS_STRING_OK) { status = hps_string_view_rfind(view, other, position, result); }
    return status;
}

static int32_t view_split_next(const hps_string_view view,
                               const hps_string_view delimiter,
                               const uint8_t byte_mode, const uint8_t byte,
                               size_t *cursor, hps_string_view *part,
                               const uint8_t keep_empty, uint8_t *found)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((cursor != NULL) && (part != NULL) && (found != NULL) && (keep_empty <= 1U)) {
        status = hps_string_check_view(view);
        if ((status == HPS_STRING_OK) && (byte_mode == 0U)) {
            status = hps_string_check_view(delimiter);
            if ((status == HPS_STRING_OK) && (delimiter.length == 0U)) { status = HPS_STRING_INVALID_ARGUMENT; }
        }
        if (status == HPS_STRING_OK) {
            if ((*cursor > view.length) && (*cursor != HPS_STRING_NPOS)) { status = HPS_STRING_OUT_OF_RANGE; }
            else {
                size_t next = *cursor;
                uint8_t available = 0U;
                hps_string_view piece = {NULL, 0U};
                while ((next != HPS_STRING_NPOS) && (available == 0U)) {
                    const size_t start = next;
                    size_t end = HPS_STRING_NPOS;
                    if (byte_mode == 1U) { status = hps_string_view_find_char(view, byte, start, &end); }
                    else { status = hps_string_view_find(view, delimiter, start, &end); }
                    if (end == HPS_STRING_NPOS) { end = view.length; next = HPS_STRING_NPOS; }
                    else { next = end + ((byte_mode == 1U) ? 1U : delimiter.length); }
                    if ((end > start) || (keep_empty == 1U)) {
                        status = hps_string_view_slice(view, start, end - start, &piece);
                        available = 1U;
                    }
                }
                /* Every internal operation has already-proved shape/range;
                 * progress consumes a nonempty delimiter or reaches NPOS. */
                if (status == HPS_STRING_OK) {
                    *cursor = next;
                    if (available == 1U) { *part = piece; }
                    *found = available;
                }
            }
        }
    }
    return status;
}

int32_t hps_string_view_split_next(const hps_string_view view,
                                   const hps_string_view delimiter, size_t *cursor,
                                   hps_string_view *part, const uint8_t keep_empty,
                                   uint8_t *found)
{
    return view_split_next(view, delimiter, 0U, 0U, cursor, part, keep_empty, found);
}

int32_t hps_string_view_split_next_char(const hps_string_view view,
                                        const uint8_t delimiter, size_t *cursor,
                                        hps_string_view *part,
                                        const uint8_t keep_empty, uint8_t *found)
{
    const hps_string_view unused = {NULL, 0U};
    return view_split_next(view, unused, 1U, delimiter, cursor, part, keep_empty, found);
}

static int32_t view_split_list(const hps_string_view view,
                               const hps_string_view delimiter,
                               const uint8_t byte_mode, const uint8_t byte,
                               hps_string_view *list, const size_t capacity,
                               const uint8_t keep_empty, size_t *count)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((count != NULL) && ((list != NULL) || (capacity == 0U))) {
        size_t cursor = 0U;
        size_t required = 0U;
        uint8_t found = 1U;
        hps_string_view piece = {NULL, 0U};
        while ((found == 1U) && (status != HPS_STRING_NO_SPACE)) {
            status = view_split_next(view, delimiter, byte_mode, byte, &cursor,
                                     &piece, keep_empty, &found);
            if (status != HPS_STRING_OK) { found = 0U; }
            else if (found == 1U) {
                if (required == capacity) { status = HPS_STRING_NO_SPACE; }
                else { required += 1U; }
            } else { /* Preflight reached exhaustion. */ }
        }
        if (status == HPS_STRING_OK) {
            size_t index = 0U;
            cursor = 0U;
            while (index < required) {
                (void)view_split_next(view, delimiter, byte_mode, byte, &cursor,
                                      &list[index], keep_empty, &found);
                index += 1U;
            }
            *count = required;
        }
    }
    return status;
}

int32_t hps_string_view_split(const hps_string_view view,
                              const hps_string_view delimiter,
                              hps_string_view *list, const size_t capacity,
                              const uint8_t keep_empty, size_t *count)
{
    return view_split_list(view, delimiter, 0U, 0U, list, capacity, keep_empty, count);
}

int32_t hps_string_view_split_char(const hps_string_view view,
                                   const uint8_t delimiter,
                                   hps_string_view *list, const size_t capacity,
                                   const uint8_t keep_empty, size_t *count)
{
    const hps_string_view unused = {NULL, 0U};
    return view_split_list(view, unused, 1U, delimiter, list, capacity, keep_empty, count);
}
