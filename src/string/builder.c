#include "hps/string/hps_builder.h"

#include <string.h>
#include "hps/string/hps_char.h"
#include "../internal/text.h"

/**
 * @file src/string/builder.c
 * @brief Preflighted string edits with caller-owned transactional scratch.
 */

#if ('A' != 65) || ('Z' != 90) || ('a' != 97) || ('z' != 122) || ('0' != 48) || ('\\' != 92)
#error "HPS string builder requires an ASCII-compatible execution character set"
#endif

int32_t hps_string_builder_init(hps_string_builder *builder, char *storage, const size_t capacity)
{
    return hps_string_buffer_init(builder, storage, capacity);
}

int32_t hps_string_builder_clear(hps_string_builder *builder)
{
    return hps_string_buffer_clear(builder);
}

int32_t hps_string_builder_as_view(const hps_string_builder *builder, hps_string_view *result)
{
    return hps_string_buffer_as_view(builder, result);
}

int32_t hps_string_builder_assign_view(hps_string_builder *builder, const hps_string_view source)
{
    return hps_string_buffer_assign(builder, source);
}

int32_t hps_string_builder_append_view(hps_string_builder *builder, const hps_string_view source)
{
    return hps_string_buffer_append(builder, source);
}

int32_t hps_string_builder_reserve(const hps_string_builder *builder, const size_t usable_capacity)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(builder, &active);
    if ((status == HPS_STRING_OK) && (usable_capacity >= builder->capacity)) {
        status = HPS_STRING_NO_SPACE;
    }
    return status;
}

int32_t hps_string_builder_swap(hps_string_builder *left, hps_string_builder *right)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(left, &active);
    if (status == HPS_STRING_OK) { status = hps_string_buffer_as_view(right, &active); }
    if (status == HPS_STRING_OK) {
        const hps_string_builder temporary = *left;
        *left = *right;
        *right = temporary;
    }
    return status;
}

int32_t hps_string_builder_append_repeat_char(hps_string_builder *builder,
                                              const uint8_t value, const size_t count)
{
    size_t remaining = 0U;
    int32_t status = hps_string_buffer_remaining(builder, &remaining);
    if (status == HPS_STRING_OK) {
        if ((value == 0U) || (value > 127U)) { status = HPS_STRING_INVALID_ARGUMENT; }
        else if (count > remaining) { status = HPS_STRING_NO_SPACE; }
        else {
            size_t index = builder->length;
            const size_t end = index + count;
            while (index < end) { builder->data[index] = (char)value; index += 1U; }
            builder->data[end] = '\0';
            builder->length = end;
        }
    }
    return status;
}

int32_t hps_string_builder_append_char(hps_string_builder *builder, const uint8_t value)
{
    return hps_string_builder_append_repeat_char(builder, value, 1U);
}

int32_t hps_string_builder_append_newline(hps_string_builder *builder)
{
    return hps_string_builder_append_char(builder, UINT8_C(10));
}

int32_t hps_string_builder_resize(hps_string_builder *builder,
                                  const size_t length, const uint8_t fill)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) {
        if ((fill == 0U) || (fill > 127U)) { status = HPS_STRING_INVALID_ARGUMENT; }
        else if (length <= active.length) { status = hps_string_buffer_truncate(builder, length); }
        else { status = hps_string_builder_append_repeat_char(builder, fill, length - active.length); }
    }
    return status;
}

/* Structural/extent validation always precedes scratch writes. Source NUL is
 * checked only after the resulting length fits builder and scratch capacities. */
int32_t hps_string_builder_replace_range_view(hps_string_builder *builder,
                                              const size_t offset, const size_t count,
                                              const hps_string_view replacement,
                                              char *scratch, const size_t scratch_capacity)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) { status = hps_string_check_view(replacement); }
    if (status == HPS_STRING_OK) {
        if (scratch == NULL) { status = HPS_STRING_INVALID_ARGUMENT; }
        else if ((offset > active.length) || (count > active.length - offset)) { status = HPS_STRING_OUT_OF_RANGE; }
        else {
            const size_t retained = active.length - count;
            if (replacement.length > builder->capacity - retained - 1U) { status = HPS_STRING_NO_SPACE; }
            else {
                const size_t length = retained + replacement.length;
                if (length >= scratch_capacity) { status = HPS_STRING_NO_SPACE; }
                else if (hps_string_has_nul(replacement) == 1U) { status = HPS_STRING_INVALID_ARGUMENT; }
                else {
                    const size_t tail = offset + count;
                    if (offset != 0U) { (void)memmove(scratch, active.data, offset); }
                    if (replacement.length != 0U) { (void)memmove(&scratch[offset], replacement.data, replacement.length); }
                    if (tail < active.length) {
                        (void)memmove(&scratch[offset + replacement.length], &active.data[tail], active.length - tail);
                    }
                    scratch[length] = '\0';
                    const hps_string_view complete = {scratch, length};
                    status = hps_string_buffer_assign(builder, complete);
                }
            }
        }
    }
    return status;
}

int32_t hps_string_builder_insert_view(hps_string_builder *builder, const size_t offset,
                                       const hps_string_view source, char *scratch,
                                       const size_t scratch_capacity)
{
    return hps_string_builder_replace_range_view(builder, offset, 0U, source, scratch, scratch_capacity);
}

int32_t hps_string_builder_prepend_view(hps_string_builder *builder, const hps_string_view source,
                                        char *scratch, const size_t scratch_capacity)
{
    return hps_string_builder_insert_view(builder, 0U, source, scratch, scratch_capacity);
}

int32_t hps_string_builder_erase(hps_string_builder *builder,
                                 const size_t offset, const size_t count)
{
    return hps_string_buffer_erase(builder, offset, count);
}

int32_t hps_string_builder_replace_first(hps_string_builder *builder,
                                         const hps_string_view pattern,
                                         const hps_string_view replacement, char *scratch,
                                         const size_t scratch_capacity, uint8_t *changed)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = (changed == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) { status = hps_string_check_view(pattern); }
    if (status == HPS_STRING_OK) { status = hps_string_check_view(replacement); }
    if (status == HPS_STRING_OK) {
        if (pattern.length == 0U) { status = HPS_STRING_INVALID_ARGUMENT; }
        else {
            size_t index = HPS_STRING_NPOS;
            status = hps_string_view_find(active, pattern, 0U, &index);
            if ((status == HPS_STRING_OK) && (index != HPS_STRING_NPOS)) {
                status = hps_string_builder_replace_range_view(builder, index, pattern.length,
                                                               replacement, scratch, scratch_capacity);
            }
            if (status == HPS_STRING_OK) { *changed = (uint8_t)(index != HPS_STRING_NPOS); }
        }
    }
    return status;
}

/* Count and size original nonoverlapping matches before writing scratch. This
 * avoids replacement cascades and makes aliases into the original builder safe. */
int32_t hps_string_builder_replace_all(hps_string_builder *builder,
                                       const hps_string_view pattern,
                                       const hps_string_view replacement,
                                       char *scratch, const size_t scratch_capacity,
                                       size_t *count)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = (count == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) { status = hps_string_check_view(pattern); }
    if (status == HPS_STRING_OK) { status = hps_string_check_view(replacement); }
    if (status == HPS_STRING_OK) {
        if (pattern.length == 0U) { status = HPS_STRING_INVALID_ARGUMENT; }
        else {
            size_t cursor = 0U;
            size_t matches = 0U;
            size_t length = active.length;
            uint8_t exhausted = 0U;
            while ((exhausted == 0U) && (status == HPS_STRING_OK)) {
                size_t index = HPS_STRING_NPOS;
                status = hps_string_view_find(active, pattern, cursor, &index);
                if (index == HPS_STRING_NPOS) { exhausted = 1U; }
                else {
                    if (replacement.length >= pattern.length) {
                        const size_t growth = replacement.length - pattern.length;
                        if (growth > builder->capacity - length - 1U) { status = HPS_STRING_NO_SPACE; }
                        else { length += growth; }
                    } else { length -= pattern.length - replacement.length; }
                    matches += 1U;
                    cursor = index + pattern.length;
                }
            }
            if ((status == HPS_STRING_OK) && (matches != 0U)) {
                if (scratch == NULL) { status = HPS_STRING_INVALID_ARGUMENT; }
                else if (length >= scratch_capacity) { status = HPS_STRING_NO_SPACE; }
                else if (hps_string_has_nul(replacement) == 1U) { status = HPS_STRING_INVALID_ARGUMENT; }
                else {
                    size_t input = 0U;
                    size_t output = 0U;
                    size_t remaining_matches = matches;
                    while (remaining_matches != 0U) {
                        size_t index = HPS_STRING_NPOS;
                        (void)hps_string_view_find(active, pattern, input, &index);
                        const size_t prefix = index - input;
                        if (prefix != 0U) { (void)memmove(&scratch[output], &active.data[input], prefix); }
                        output += prefix;
                        if (replacement.length != 0U) {
                            (void)memmove(&scratch[output], replacement.data, replacement.length);
                        }
                        output += replacement.length;
                        input = index + pattern.length;
                        remaining_matches -= 1U;
                    }
                    if (input < active.length) {
                        (void)memmove(&scratch[output], &active.data[input], active.length - input);
                    }
                    scratch[length] = '\0';
                    const hps_string_view complete = {scratch, length};
                    status = hps_string_buffer_assign(builder, complete);
                }
            }
            if (status == HPS_STRING_OK) { *count = matches; }
        }
    }
    return status;
}

int32_t hps_string_builder_replace_char_all(hps_string_builder *builder,
                                            const uint8_t old_value,
                                            const uint8_t new_value, size_t *count)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = (count == NULL) ? HPS_STRING_INVALID_ARGUMENT :
        hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) {
        if ((new_value == 0U) || (new_value > 127U)) { status = HPS_STRING_INVALID_ARGUMENT; }
        else {
            size_t index = 0U;
            size_t matches = 0U;
            while (index < active.length) {
                if ((uint8_t)builder->data[index] == old_value) {
                    builder->data[index] = (char)new_value;
                    matches += 1U;
                }
                index += 1U;
            }
            *count = matches;
        }
    }
    return status;
}

int32_t hps_string_builder_trim_ascii_whitespace(hps_string_builder *builder)
{
    hps_string_view active = {NULL, 0U};
    hps_string_view trimmed = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) { status = hps_string_view_trim_whitespace(active, &trimmed); }
    if (status == HPS_STRING_OK) { status = hps_string_buffer_assign(builder, trimmed); }
    return status;
}

int32_t hps_string_builder_trim_char(hps_string_builder *builder, const uint8_t value)
{
    hps_string_view active = {NULL, 0U};
    hps_string_view trimmed = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) { status = hps_string_view_trim_char(active, value, &trimmed); }
    if (status == HPS_STRING_OK) { status = hps_string_buffer_assign(builder, trimmed); }
    return status;
}

static int32_t builder_fold(hps_string_builder *builder, const uint8_t upper)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) {
        size_t index = 0U;
        while (index < active.length) {
            const uint8_t value = (uint8_t)builder->data[index];
            if (upper == 1U) {
                if (hps_char_is_alpha_lower(value) == 1U) { builder->data[index] = (char)hps_char_to_upper(value); }
            } else {
                if (hps_char_is_alpha_upper(value) == 1U) { builder->data[index] = (char)hps_char_to_lower(value); }
            }
            index += 1U;
        }
    }
    return status;
}

int32_t hps_string_builder_to_upper_ascii(hps_string_builder *builder)
{
    return builder_fold(builder, 1U);
}

int32_t hps_string_builder_to_lower_ascii(hps_string_builder *builder)
{
    return builder_fold(builder, 0U);
}

/* One preflight/commit engine handles plain lines/quotes and JSON escapes.
 * Modes: 0 plain quoted, 1 escaped, 2 JSON quoted, 3 plain line. */
static int32_t builder_compose(hps_string_builder *builder,
                               const hps_string_view source, char *scratch,
                               const size_t scratch_capacity, const uint8_t mode)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = hps_string_buffer_as_view(builder, &active);
    if (status == HPS_STRING_OK) { status = hps_string_check_view(source); }
    if (status == HPS_STRING_OK) {
        if (scratch == NULL) { status = HPS_STRING_INVALID_ARGUMENT; }
        else {
            const size_t wrappers = ((mode == 0U) || (mode == 2U)) ? 2U : ((mode == 3U) ? 1U : 0U);
            size_t length = active.length;
            size_t index = 0U;
            if (wrappers > builder->capacity - length - 1U) { status = HPS_STRING_NO_SPACE; }
            else { length += wrappers; }
            while ((index < source.length) && (status == HPS_STRING_OK)) {
                const uint8_t value = (uint8_t)source.data[index];
                size_t width = 1U;
                if ((mode == 1U) || (mode == 2U)) {
                    if ((value == 34U) || (value == 92U) || (value == 8U) ||
                        (value == 9U) || (value == 10U) || (value == 12U) || (value == 13U)) { width = 2U; }
                    else if (value < 32U) { width = 6U; }
                    else { /* Ordinary octets remain unchanged. */ }
                } else if (value == 0U) { status = HPS_STRING_INVALID_ARGUMENT; }
                else { /* Plain source has one character per input character. */ }
                if (status == HPS_STRING_OK) {
                    if (width > builder->capacity - length - 1U) {
                        status = HPS_STRING_NO_SPACE;
                    } else {
                        length += width;
                    }
                }
                index += 1U;
            }
            if ((status == HPS_STRING_OK) && (length >= scratch_capacity)) { status = HPS_STRING_NO_SPACE; }
            if (status == HPS_STRING_OK) {
                size_t output = active.length;
                if (active.length != 0U) { (void)memmove(scratch, active.data, active.length); }
                if ((mode == 0U) || (mode == 2U)) { scratch[output] = '"'; output += 1U; }
                index = 0U;
                while (index < source.length) {
                    const uint8_t value = (uint8_t)source.data[index];
                    if (((mode == 1U) || (mode == 2U)) && (value < 32U || value == 34U || value == 92U)) {
                        scratch[output] = '\\'; output += 1U;
                        if (value == 34U) { scratch[output] = '"'; output += 1U; }
                        else if (value == 92U) { scratch[output] = '\\'; output += 1U; }
                        else if (value == 8U) { scratch[output] = 'b'; output += 1U; }
                        else if (value == 9U) { scratch[output] = 't'; output += 1U; }
                        else if (value == 10U) { scratch[output] = 'n'; output += 1U; }
                        else if (value == 12U) { scratch[output] = 'f'; output += 1U; }
                        else if (value == 13U) { scratch[output] = 'r'; output += 1U; }
                        else {
                            scratch[output] = 'u'; scratch[output + 1U] = '0'; scratch[output + 2U] = '0';
                            scratch[output + 3U] = (char)hps_char_nibble_to_hex_lower((uint8_t)(value / 16U));
                            scratch[output + 4U] = (char)hps_char_nibble_to_hex_lower(value);
                            output += 5U;
                        }
                    } else { scratch[output] = source.data[index]; output += 1U; }
                    index += 1U;
                }
                if ((mode == 0U) || (mode == 2U)) { scratch[output] = '"'; output += 1U; }
                else if (mode == 3U) { scratch[output] = '\n'; output += 1U; }
                else { /* Escaped output has no enclosing syntax. */ }
                scratch[output] = '\0';
                const hps_string_view complete = {scratch, output};
                status = hps_string_buffer_assign(builder, complete);
            }
        }
    }
    return status;
}

int32_t hps_string_builder_append_quoted(hps_string_builder *builder, const hps_string_view source,
                                         char *scratch, const size_t scratch_capacity)
{
    return builder_compose(builder, source, scratch, scratch_capacity, 0U);
}

int32_t hps_string_builder_append_json_escaped(hps_string_builder *builder,
                                               const hps_string_view source, char *scratch,
                                               const size_t scratch_capacity)
{
    return builder_compose(builder, source, scratch, scratch_capacity, 1U);
}

int32_t hps_string_builder_append_json_quoted(hps_string_builder *builder,
                                              const hps_string_view source, char *scratch,
                                              const size_t scratch_capacity)
{
    return builder_compose(builder, source, scratch, scratch_capacity, 2U);
}

int32_t hps_string_builder_append_line(hps_string_builder *builder, const hps_string_view source,
                                       char *scratch, const size_t scratch_capacity)
{
    return builder_compose(builder, source, scratch, scratch_capacity, 3U);
}
