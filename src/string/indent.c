#include "hps/string/hps_indent.h"

#include "../internal/text.h"

/**
 * @file src/string/indent.c
 * @brief Indentation state with preflighted composition through existing builders.
 */

static int32_t indent_check(const hps_string_indent *context,
                            hps_string_view *active)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (context != NULL) {
        if ((context->builder == NULL) || (context->at_line_start > 1U) ||
            (context->level > (SIZE_MAX / HPS_STRING_INDENT_WIDTH))) {
            status = HPS_STRING_INVALID_STATE;
        } else { status = hps_string_builder_as_view(context->builder, active); }
    }
    return status;
}

int32_t hps_string_indent_init(hps_string_indent *context,
                                hps_string_builder *builder)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (context != NULL) {
        hps_string_view active = {NULL, 0U};
        status = hps_string_builder_as_view(builder, &active);
        if (status == HPS_STRING_OK) {
            context->builder = builder;
            context->level = 0U;
            context->at_line_start = (uint8_t)((active.length == 0U) ||
                (active.data[active.length - 1U] == '\n'));
        }
    }
    return status;
}

int32_t hps_string_indent_clear(hps_string_indent *context)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = indent_check(context, &active);
    if (status == HPS_STRING_OK) { status = hps_string_builder_clear(context->builder); }
    if (status == HPS_STRING_OK) {
        context->level = 0U;
        context->at_line_start = 1U;
    }
    return status;
}

int32_t hps_string_indent_set_level(hps_string_indent *context, const size_t level)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = indent_check(context, &active);
    if (status == HPS_STRING_OK) {
        if (level > (SIZE_MAX / HPS_STRING_INDENT_WIDTH)) { status = HPS_STRING_OUT_OF_RANGE; }
        else { context->level = level; }
    }
    return status;
}

int32_t hps_string_indent_append_indent(hps_string_indent *context)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = indent_check(context, &active);
    if (status == HPS_STRING_OK) {
        status = hps_string_builder_append_repeat_char(context->builder, (uint8_t)' ',
            context->level * HPS_STRING_INDENT_WIDTH);
    }
    return status;
}

int32_t hps_string_indent_append_newline(hps_string_indent *context)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = indent_check(context, &active);
    if (status == HPS_STRING_OK) { status = hps_string_builder_append_newline(context->builder); }
    if (status == HPS_STRING_OK) { context->at_line_start = 1U; }
    return status;
}

static int32_t indent_compose(hps_string_indent *context,
                              const hps_string_view source,
                              char *scratch, const size_t scratch_capacity,
                              const uint8_t newline)
{
    hps_string_view active = {NULL, 0U};
    int32_t status = indent_check(context, &active);
    if (status == HPS_STRING_OK) { status = hps_string_check_view(source); }
    if (status == HPS_STRING_OK) {
        if (scratch == NULL) { status = HPS_STRING_INVALID_ARGUMENT; }
        else {
            const size_t spaces = (context->at_line_start == 1U) ?
                context->level * HPS_STRING_INDENT_WIDTH : 0U;
            const size_t remaining = context->builder->capacity - active.length - 1U;
            if (spaces > remaining) { status = HPS_STRING_NO_SPACE; }
            else if (source.length > (remaining - spaces)) { status = HPS_STRING_NO_SPACE; }
            else if ((size_t)newline > (remaining - spaces - source.length)) { status = HPS_STRING_NO_SPACE; }
            else {
                const size_t length = active.length + spaces + source.length + (size_t)newline;
                if (length >= scratch_capacity) { status = HPS_STRING_NO_SPACE; }
                else if (hps_string_has_nul(source) != 0U) { status = HPS_STRING_INVALID_ARGUMENT; }
                else {
                    hps_string_builder staged = {NULL, 0U, 0U};
                    /* Shape, NUL and total-space proofs above cover each staged
                     * builder call. No new fallible input is introduced after
                     * the first scratch write; reuse the existing copy engine. */
                    (void)hps_string_builder_init(&staged, scratch, scratch_capacity);
                    (void)hps_string_builder_assign_view(&staged, active);
                    (void)hps_string_builder_append_repeat_char(&staged, (uint8_t)' ', spaces);
                    (void)hps_string_builder_append_view(&staged, source);
                    if (newline == 1U) { (void)hps_string_builder_append_newline(&staged); }
                    const hps_string_view complete = {scratch, length};
                    status = hps_string_builder_assign_view(context->builder, complete);
                    if (status == HPS_STRING_OK) { context->at_line_start = newline; }
                }
            }
        }
    }
    return status;
}

int32_t hps_string_indent_append_view(hps_string_indent *context,
                                       const hps_string_view source,
                                       char *scratch, const size_t scratch_capacity)
{
    return indent_compose(context, source, scratch, scratch_capacity, 0U);
}

int32_t hps_string_indent_append_line(hps_string_indent *context,
                                       const hps_string_view source,
                                       char *scratch, const size_t scratch_capacity)
{
    return indent_compose(context, source, scratch, scratch_capacity, 1U);
}
