#include "hps/cstr/hps_core.h"
#include <string.h>

/**
 * @file src/cstr/core.c
 * @brief Capacity-checked C-string algorithms over caller-owned buffers.
 */

static int32_t check_view(hps_string_view view)
{
    hps_string_view checked = {NULL, 0U};
    return hps_string_view_init(&checked, view.data, view.length);
}

static int32_t check_pattern(hps_string_view pattern)
{
    int32_t status = check_view(pattern);
    if (status == HPS_STRING_OK) {
        size_t index = 0U;
        while ((index < pattern.length) && (status == HPS_STRING_OK)) {
            if (pattern.data[index] == '\0') {
                status = HPS_STRING_INVALID_ARGUMENT;
            }
            index += 1U;
        }
    }
    return status;
}

int32_t hps_cstr_length(const char *data, const size_t capacity,
                        size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_string_view view = {NULL, 0U};
        status = hps_string_view_init_cstr(&view, data, capacity);
        if (status == HPS_STRING_OK) {
            *result = view.length;
        }
    }
    return status;
}

int32_t hps_cstr_is_empty(const char *data, const size_t capacity,
                          uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        size_t length = 0U;
        status = hps_cstr_length(data, capacity, &length);
        if (status == HPS_STRING_OK) {
            *result = (uint8_t)(length == 0U);
        }
    }
    return status;
}

int32_t hps_cstr_is_terminated(const char *data,
                               const size_t capacity, uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        size_t length = 0U;
        status = hps_cstr_length(data, capacity, &length);
        if ((status == HPS_STRING_OK) || (status == HPS_STRING_NOT_TERMINATED)) {
            *result = (uint8_t)(status == HPS_STRING_OK);
            status = HPS_STRING_OK;
        }
    }
    return status;
}

int32_t hps_cstr_has_capacity_for(const char *data,
                                  const size_t capacity,
                                  const size_t additional,
                                  uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        size_t length = 0U;
        status = hps_cstr_length(data, capacity, &length);
        if (status == HPS_STRING_OK) {
            *result = (uint8_t)(additional <= (capacity - length - 1U));
        }
    }
    return status;
}

int32_t hps_cstr_copy_length(const char *data, const size_t capacity,
                             const size_t output_capacity,
                             size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        size_t length = 0U;
        status = hps_cstr_length(data, capacity, &length);
        if (status == HPS_STRING_OK) {
            const size_t available = (output_capacity == 0U) ?
                0U : output_capacity - 1U;
            *result = (length < available) ? length : available;
        }
    }
    return status;
}

static int32_t clear_storage(hps_string_buffer *buffer,
                             const uint8_t tail_only)
{
    hps_string_view view = {NULL, 0U};
    const int32_t status = hps_string_buffer_as_view(buffer, &view);
    if (status == HPS_STRING_OK) {
        const size_t start = (tail_only == 1U) ? view.length : 0U;
        (void)memset(&buffer->data[start], 0, buffer->capacity - start);
        if (tail_only == 0U) {
            buffer->length = 0U;
        }
    }
    return status;
}

int32_t hps_cstr_clear(hps_string_buffer *buffer)
{
    return clear_storage(buffer, 0U);
}

int32_t hps_cstr_clear_tail(hps_string_buffer *buffer)
{
    return clear_storage(buffer, 1U);
}

/* A table mapping avoids arithmetic on char and does not assume contiguous
 * execution-character codes for the Latin alphabet. Unmatched bytes stay exact. */
static const char *case_character(const char *input, const uint8_t mode)
{
    const char *upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lower = "abcdefghijklmnopqrstuvwxyz";
    const char *result = input;
    const char *source = (mode == HPS_CSTR_LOWER_CASE) ? upper : lower;
    const char *destination = (mode == HPS_CSTR_LOWER_CASE) ? lower : upper;
    size_t index = 0U;
    uint8_t found = 0U;
    if (mode != HPS_CSTR_KEEP_CASE) {
        while ((index < 26U) && (found == 0U)) {
            if (*input == source[index]) {
                result = &destination[index];
                found = 1U;
            }
            index += 1U;
        }
    }
    return result;
}

int32_t hps_cstr_compare(const char *left, const size_t left_capacity,
                         const char *right, const size_t right_capacity,
                         const size_t limit, const uint8_t ignore_case,
                         int32_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (ignore_case <= 1U)) {
        hps_string_view lhs = {NULL, 0U};
        hps_string_view rhs = {NULL, 0U};
        status = hps_string_view_init_cstr(&lhs, left, left_capacity);
        if (status == HPS_STRING_OK) {
            status = hps_string_view_init_cstr(&rhs, right, right_capacity);
        }
        if (status == HPS_STRING_OK) {
            if (lhs.length > limit) {
                lhs.length = limit;
            }
            if (rhs.length > limit) {
                rhs.length = limit;
            }
            if (ignore_case == 0U) {
                status = hps_string_view_compare(lhs, rhs, result);
            } else {
                const size_t common = (lhs.length < rhs.length) ?
                    lhs.length : rhs.length;
                size_t index = 0U;
                int32_t order = INT32_C(0);
                while ((index < common) && (order == INT32_C(0))) {
                    const char *left_char = case_character(&lhs.data[index],
                                                            HPS_CSTR_LOWER_CASE);
                    const char *right_char = case_character(&rhs.data[index],
                                                             HPS_CSTR_LOWER_CASE);
                    if (*left_char != *right_char) {
                        order = (memcmp(left_char, right_char, 1U) < 0) ?
                            -INT32_C(1) : INT32_C(1);
                    }
                    index += 1U;
                }
                if (order == INT32_C(0)) {
                    if (lhs.length < rhs.length) {
                        order = -INT32_C(1);
                    } else if (lhs.length > rhs.length) {
                        order = INT32_C(1);
                    } else {
                        /* Both compared prefixes are equal. */
                    }
                }
                *result = order;
            }
        }
    }
    return status;
}

int32_t hps_cstr_copy_range(hps_string_buffer *output,
                            const char *input,
                            const size_t input_capacity,
                            const size_t offset, const size_t count,
                            const uint8_t mode)
{
    hps_string_view source = {NULL, 0U};
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((output != NULL) && (mode <= HPS_CSTR_UPPER_CASE)) {
        status = hps_string_view_init_cstr(&source, input, input_capacity);
        if (status == HPS_STRING_OK) {
            if (offset > source.length) {
                status = HPS_STRING_OUT_OF_RANGE;
            } else {
                const size_t actual = (count == HPS_STRING_NPOS) ?
                    source.length - offset : count;
                status = hps_string_view_slice(source, offset, actual, &source);
            }
        }
        if (status == HPS_STRING_OK) {
            status = hps_string_buffer_assign(output, source);
        }
        if (status == HPS_STRING_OK) {
            size_t index;
            for (index = 0U; index < output->length; ++index) {
                const char *mapped = case_character(&output->data[index], mode);
                output->data[index] = *mapped;
            }
        }
    }
    return status;
}

/* Membership is a one-character substring search over a previously checked set.
 * Its index output is local and disjoint; validated extents make failure impossible. */
static uint8_t set_contains(hps_string_view set, const char *character)
{
    const hps_string_view needle = {character, 1U};
    size_t index = HPS_STRING_NPOS;
    const int32_t status = hps_string_view_find(set, needle, 0U, &index);
    return (uint8_t)((status == HPS_STRING_OK) && (index != HPS_STRING_NPOS));
}

int32_t hps_cstr_consists_of(const char *data, const size_t capacity,
                             hps_string_view allowed, uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_string_view input = {NULL, 0U};
        status = hps_string_view_init_cstr(&input, data, capacity);
        if (status == HPS_STRING_OK) {
            status = check_view(allowed);
        }
        if (status == HPS_STRING_OK) {
            uint8_t consists = (uint8_t)((input.length != 0U) &&
                                        (allowed.length != 0U));
            size_t index = 0U;
            while ((index < input.length) && (consists == 1U)) {
                consists = set_contains(allowed, &input.data[index]);
                index += 1U;
            }
            *result = consists;
        }
    }
    return status;
}

static int32_t search_string(const char *data, const size_t capacity,
                             const char *needle,
                             const size_t needle_capacity,
                             const size_t position,
                             const uint8_t reverse, size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_string_view input = {NULL, 0U};
        hps_string_view pattern = {NULL, 0U};
        status = hps_string_view_init_cstr(&input, data, capacity);
        if (status == HPS_STRING_OK) {
            status = hps_string_view_init_cstr(&pattern, needle, needle_capacity);
        }
        if (status == HPS_STRING_OK) {
            status = (reverse == 1U) ?
                hps_string_view_rfind(input, pattern, position, result) :
                hps_string_view_find(input, pattern, position, result);
        }
    }
    return status;
}

int32_t hps_cstr_find(const char *data, const size_t capacity,
                      const char *needle, const size_t needle_capacity,
                      const size_t offset, size_t *result)
{
    return search_string(data, capacity, needle, needle_capacity, offset, 0U, result);
}

int32_t hps_cstr_rfind(const char *data, const size_t capacity,
                       const char *needle, const size_t needle_capacity,
                       const size_t position, size_t *result)
{
    return search_string(data, capacity, needle, needle_capacity, position, 1U, result);
}

static int32_t search_character(const char *data,
                                const size_t capacity,
                                hps_string_view character,
                                const size_t position,
                                const uint8_t reverse, size_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((result != NULL) && (character.length == 1U)) {
        status = check_view(character);
        if (status == HPS_STRING_OK) {
            hps_string_view input = {NULL, 0U};
            status = hps_string_view_init_cstr(&input, data, capacity);
            if (status == HPS_STRING_OK) {
                status = (reverse == 1U) ?
                    hps_string_view_rfind(input, character, position, result) :
                    hps_string_view_find(input, character, position, result);
            }
        }
    }
    return status;
}

int32_t hps_cstr_find_char(const char *data, const size_t capacity,
                           hps_string_view character,
                           const size_t offset, size_t *result)
{
    return search_character(data, capacity, character, offset, 0U, result);
}

int32_t hps_cstr_rfind_char(const char *data, const size_t capacity,
                            hps_string_view character,
                            const size_t position, size_t *result)
{
    return search_character(data, capacity, character, position, 1U, result);
}

int32_t hps_cstr_reverse(hps_string_buffer *buffer, const size_t offset,
                         const size_t count)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    if (status == HPS_STRING_OK) {
        if (offset > active.length) {
            status = HPS_STRING_OUT_OF_RANGE;
        } else {
            const size_t actual = (count == HPS_STRING_NPOS) ?
                active.length - offset : count;
            if (actual > (active.length - offset)) {
                status = HPS_STRING_OUT_OF_RANGE;
            } else if (actual > 1U) {
                size_t left = offset;
                size_t right = offset + actual - 1U;
                char temporary[1];
                while (left < right) {
                    temporary[0] = buffer->data[left];
                    buffer->data[left] = buffer->data[right];
                    buffer->data[right] = temporary[0];
                    left += 1U;
                    right -= 1U;
                }
            } else {
                /* Empty and singleton ranges require no payload writes. */
            }
        }
    }
    return status;
}

int32_t hps_cstr_replace_char(hps_string_buffer *buffer,
                              hps_string_view old_character,
                              hps_string_view new_character)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    if (status == HPS_STRING_OK) {
        status = check_pattern(old_character);
        if (status == HPS_STRING_OK) {
            status = check_pattern(new_character);
        }
        if ((status == HPS_STRING_OK) &&
            ((old_character.length != 1U) || (new_character.length != 1U))) {
            status = HPS_STRING_INVALID_ARGUMENT;
        }
        if (status == HPS_STRING_OK) {
            char values[2];
            size_t index;
            values[0] = old_character.data[0];
            values[1] = new_character.data[0];
            for (index = 0U; index < active.length; ++index) {
                if (buffer->data[index] == values[0]) {
                    buffer->data[index] = values[1];
                }
            }
        }
    }
    return status;
}

int32_t hps_cstr_filter(hps_string_buffer *buffer,
                        hps_string_view chars, const uint8_t mode)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    if (status == HPS_STRING_OK) {
        status = check_view(chars);
        if ((status == HPS_STRING_OK) &&
            ((mode > HPS_CSTR_REMOVE_DIGITS) ||
             ((mode != HPS_CSTR_REMOVE_SET) && (chars.length != 0U)))) {
            status = HPS_STRING_INVALID_ARGUMENT;
        }
        if (status == HPS_STRING_OK) {
            size_t read;
            size_t written = 0U;
            for (read = 0U; read < active.length; ++read) {
                uint8_t keep;
                if (mode == HPS_CSTR_REMOVE_SET) {
                    keep = (uint8_t)(set_contains(chars, &active.data[read]) == 0U);
                } else {
                    const uint8_t digit = (uint8_t)((active.data[read] >= '0') &&
                                                  (active.data[read] <= '9'));
                    keep = (mode == HPS_CSTR_KEEP_DIGITS) ?
                        digit : (uint8_t)(digit == 0U);
                }
                if (keep == 1U) {
                    buffer->data[written] = active.data[read];
                    written += 1U;
                }
            }
            buffer->data[written] = '\0';
            buffer->length = written;
        }
    }
    return status;
}

int32_t hps_cstr_remove_substring(hps_string_buffer *buffer,
                                  const char *needle,
                                  const size_t needle_capacity)
{
    hps_string_view active = {NULL, 0U};
    hps_string_view pattern = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    if (status == HPS_STRING_OK) {
        status = hps_string_view_init_cstr(&pattern, needle, needle_capacity);
    }
    if ((status == HPS_STRING_OK) && (pattern.length != 0U)) {
        size_t read = 0U;
        size_t written = 0U;
        while (read < active.length) {
            size_t found = HPS_STRING_NPOS;
            /* Shape and offsets were checked before mutation. Only original
             * unread suffixes are searched; compaction writes strictly behind. */
            status = hps_string_view_find(active, pattern, read, &found);
            if (status == HPS_STRING_OK) {
                const size_t end = (found == HPS_STRING_NPOS) ?
                    active.length : found;
                const size_t count = end - read;
                if (count != 0U) {
                    (void)memmove(&buffer->data[written], &active.data[read], count);
                    written += count;
                }
                read = (found == HPS_STRING_NPOS) ?
                    active.length : found + pattern.length;
            } else {
                /* Unreachable for validated views in the single-owner context. */
                read = active.length;
            }
        }
        if (status == HPS_STRING_OK) {
            buffer->data[written] = '\0';
            buffer->length = written;
        }
    }
    return status;
}

int32_t hps_cstr_trim(hps_string_buffer *buffer,
                      hps_string_view left_chars,
                      hps_string_view right_chars)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    if (status == HPS_STRING_OK) {
        status = check_view(left_chars);
    }
    if (status == HPS_STRING_OK) {
        status = check_view(right_chars);
    }
    if (status == HPS_STRING_OK) {
        size_t start = 0U;
        size_t end = active.length;
        while ((start < end) &&
               (set_contains(left_chars, &active.data[start]) == 1U)) {
            start += 1U;
        }
        while ((start < end) &&
               (set_contains(right_chars, &active.data[end - 1U]) == 1U)) {
            end -= 1U;
        }
        status = hps_string_view_slice(active, start, end - start, &active);
        if (status == HPS_STRING_OK) {
            status = hps_string_buffer_assign(buffer, active);
        }
    }
    return status;
}

int32_t hps_cstr_trim_substrings(hps_string_buffer *buffer,
                                 hps_string_view left_pattern,
                                 hps_string_view right_pattern)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    if (status == HPS_STRING_OK) {
        status = check_view(left_pattern);
    }
    if (status == HPS_STRING_OK) {
        status = check_view(right_pattern);
    }
    if (status == HPS_STRING_OK) {
        size_t start = 0U;
        size_t end = active.length;
        uint8_t match = 1U;
        while ((status == HPS_STRING_OK) && (left_pattern.length != 0U) &&
               (left_pattern.length <= (end - start)) && (match == 1U)) {
            const hps_string_view remaining = {&active.data[start], end - start};
            status = hps_string_view_starts_with(remaining, left_pattern, &match);
            if ((status == HPS_STRING_OK) && (match == 1U)) {
                start += left_pattern.length;
            }
        }
        match = 1U;
        while ((status == HPS_STRING_OK) && (right_pattern.length != 0U) &&
               (right_pattern.length <= (end - start)) && (match == 1U)) {
            const hps_string_view remaining = {&active.data[start], end - start};
            status = hps_string_view_ends_with(remaining, right_pattern, &match);
            if ((status == HPS_STRING_OK) && (match == 1U)) {
                end -= right_pattern.length;
            }
        }
        if (status == HPS_STRING_OK) {
            status = hps_string_view_slice(active, start, end - start, &active);
        }
        if (status == HPS_STRING_OK) {
            status = hps_string_buffer_assign(buffer, active);
        }
    }
    return status;
}

/* Derive a repetition's byte count against remaining real storage. Zero-length
 * patterns do not evaluate count*length or enter a count-driven write loop. */
static int32_t pattern_bytes(hps_string_view pattern,
                             const size_t count,
                             const size_t available, size_t *result)
{
    int32_t status = check_pattern(pattern);
    if (status == HPS_STRING_OK) {
        if (pattern.length == 0U) {
            *result = 0U;
        } else if (count > (available / pattern.length)) {
            status = HPS_STRING_NO_SPACE;
        } else {
            *result = count * pattern.length;
        }
    }
    return status;
}

/* All spans were validated and patterns are disjoint from writable storage.
 * This bounded writer also supplies the shortened last repetition in pad_to. */
static void write_pattern(char *output, hps_string_view pattern,
                          const size_t bytes)
{
    size_t written = 0U;
    while (written < bytes) {
        const size_t remaining = bytes - written;
        const size_t chunk = (pattern.length < remaining) ?
            pattern.length : remaining;
        (void)memcpy(&output[written], pattern.data, chunk);
        written += chunk;
    }
}

int32_t hps_cstr_pad(hps_string_buffer *buffer,
                     hps_string_view left_pattern,
                     const size_t left_count,
                     hps_string_view right_pattern,
                     const size_t right_count)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    size_t left_bytes = 0U;
    size_t right_bytes = 0U;
    if (status == HPS_STRING_OK) {
        const size_t available = buffer->capacity - active.length - 1U;
        status = pattern_bytes(left_pattern, left_count, available, &left_bytes);
        if (status == HPS_STRING_OK) {
            status = pattern_bytes(right_pattern, right_count,
                                   available - left_bytes, &right_bytes);
        }
    }
    if (status == HPS_STRING_OK) {
        const size_t after_text = left_bytes + active.length;
        const size_t length = after_text + right_bytes;
        if ((left_bytes != 0U) && (active.length != 0U)) {
            (void)memmove(&buffer->data[left_bytes], buffer->data, active.length);
        }
        write_pattern(buffer->data, left_pattern, left_bytes);
        write_pattern(&buffer->data[after_text], right_pattern, right_bytes);
        buffer->data[length] = '\0';
        buffer->length = length;
    }
    return status;
}

int32_t hps_cstr_repeat(hps_string_buffer *buffer,
                        hps_string_view pattern, const size_t count)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(buffer, &active);
    size_t bytes = 0U;
    if (status == HPS_STRING_OK) {
        status = pattern_bytes(pattern, count, buffer->capacity - 1U, &bytes);
    }
    if (status == HPS_STRING_OK) {
        write_pattern(buffer->data, pattern, bytes);
        buffer->data[bytes] = '\0';
        buffer->length = bytes;
    }
    return status;
}

int32_t hps_cstr_pad_to(hps_string_buffer *buffer,
                        hps_string_view pattern, const size_t target,
                        const uint8_t left)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (left <= 1U) {
        status = hps_string_buffer_as_view(buffer, &active);
        if (status == HPS_STRING_OK) {
            status = check_pattern(pattern);
        }
        if ((status == HPS_STRING_OK) && (target > active.length)) {
            if (pattern.length == 0U) {
                status = HPS_STRING_INVALID_ARGUMENT;
            } else if (target >= buffer->capacity) {
                status = HPS_STRING_NO_SPACE;
            } else {
                const size_t padding = target - active.length;
                if (left == 1U) {
                    if (active.length != 0U) {
                        (void)memmove(&buffer->data[padding], buffer->data,
                                      active.length);
                    }
                    write_pattern(buffer->data, pattern, padding);
                } else {
                    write_pattern(&buffer->data[active.length], pattern, padding);
                }
                buffer->data[target] = '\0';
                buffer->length = target;
            }
        }
    }
    return status;
}
