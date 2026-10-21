#include "../test_suites.h"
#include "hps/string/hps_fixed.h"
#include "hps/string/hps_indent.h"

#include <assert.h>
#include <string.h>

/**
 * @file tests/string/test_string_indent.c
 * @brief Persistent indentation, alias staging, capacity bounds and failure state.
 */

HPS_STRING_FIXED_DEFINE(indent_text, 64U)
HPS_STRING_FIXED_DEFINE(indent_small, 8U)

static void test_persistent_lines(void)
{
    indent_text_t storage = HPS_STRING_FIXED_STATIC_INIT;
    indent_text_t scratch = HPS_STRING_FIXED_STATIC_INIT;
    hps_string_builder builder = {NULL, 0U, 0U};
    hps_string_indent context = {NULL, 0U, 0U};
    assert(indent_text_capacity() == sizeof(storage.data));
    assert(indent_text_bind(&storage, &builder) == HPS_STRING_OK);
    assert(indent_text_adopt(&storage, &builder) == HPS_STRING_OK);
    assert(hps_string_indent_init(&context, &builder) == HPS_STRING_OK);
    assert(context.level == 0U && context.at_line_start == 1U && context.builder == &builder);
    assert(hps_string_indent_set_level(&context, 1U) == HPS_STRING_OK);
    const hps_string_view first = {"one", 3U};
    const hps_string_view second = {"two", 3U};
    assert(hps_string_indent_append_view(&context, first, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(hps_string_indent_append_view(&context, second, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(strcmp(storage.data, "    onetwo") == 0 && context.at_line_start == 0U);
    assert(hps_string_indent_append_newline(&context) == HPS_STRING_OK);
    assert(hps_string_indent_set_level(&context, 2U) == HPS_STRING_OK);
    assert(hps_string_indent_append_line(&context, second, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(strcmp(storage.data, "    onetwo\n        two\n") == 0 && context.at_line_start == 1U);
    assert(hps_string_indent_clear(&context) == HPS_STRING_OK);
    assert(context.level == 0U && context.at_line_start == 1U && builder.length == 0U && storage.data[0] == '\0');

    assert(hps_string_indent_set_level(&context, 1U) == HPS_STRING_OK);
    const hps_string_view embedded_newline = {"a\nb", 3U};
    assert(hps_string_indent_append_view(&context, embedded_newline, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(strcmp(storage.data, "    a\nb") == 0 && context.at_line_start == 0U);
    assert(hps_string_indent_append_newline(&context) == HPS_STRING_OK);
    assert(hps_string_indent_append_indent(&context) == HPS_STRING_OK);
    const hps_string_view empty = {NULL, 0U};
    assert(hps_string_indent_append_view(&context, empty, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(strcmp(storage.data, "    a\nb\n        ") == 0 && context.at_line_start == 0U);
}

static void test_source_alias_and_binding(void)
{
    indent_text_t storage = HPS_STRING_FIXED_STATIC_INIT;
    indent_text_t scratch = HPS_STRING_FIXED_STATIC_INIT;
    hps_string_builder builder = {NULL, 0U, 0U};
    hps_string_indent context = {NULL, 0U, 0U};
    assert(indent_text_bind(&storage, &builder) == HPS_STRING_OK);
    const hps_string_view initial = {"xy\n", 3U};
    assert(hps_string_builder_assign_view(&builder, initial) == HPS_STRING_OK);
    assert(hps_string_indent_init(&context, &builder) == HPS_STRING_OK);
    assert(context.at_line_start == 1U && strcmp(storage.data, "xy\n") == 0);
    assert(hps_string_indent_set_level(&context, 1U) == HPS_STRING_OK);
    const hps_string_view alias = {storage.data, 2U};
    assert(hps_string_indent_append_line(&context, alias, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(strcmp(storage.data, "xy\n    xy\n") == 0 && context.at_line_start == 1U);
    assert(hps_string_indent_clear(&context) == HPS_STRING_OK);
    assert(hps_string_indent_append_view(&context, alias, scratch.data, sizeof(scratch.data)) == HPS_STRING_INVALID_ARGUMENT);

    /* Abandon the old borrowing context before preparing another bind fixture. */
    context.builder = NULL;
    assert(hps_string_builder_assign_view(&builder, initial) == HPS_STRING_OK);
    builder.data[2] = 'z';
    assert(hps_string_indent_init(&context, &builder) == HPS_STRING_OK);
    assert(context.at_line_start == 0U);
}

static void test_capacity_and_atomic_failures(void)
{
    indent_small_t storage = HPS_STRING_FIXED_STATIC_INIT;
    indent_text_t scratch = HPS_STRING_FIXED_STATIC_INIT;
    hps_string_builder builder = {NULL, 0U, 0U};
    hps_string_indent context = {NULL, 0U, 0U};
    assert(indent_small_capacity() == sizeof(storage.data));
    assert(indent_small_bind(&storage, &builder) == HPS_STRING_OK);
    assert(indent_small_adopt(&storage, &builder) == HPS_STRING_OK);
    assert(hps_string_indent_init(&context, &builder) == HPS_STRING_OK);
    assert(hps_string_indent_set_level(&context, 1U) == HPS_STRING_OK);
    const hps_string_view exact = {"abc", 3U};
    const hps_string_view two = {"ab", 2U};
    (void)memset(scratch.data, 's', sizeof(scratch.data));
    const indent_text_t before_scratch = scratch;
    const indent_small_t before_storage = storage;
    assert(hps_string_indent_append_line(&context, exact, scratch.data, sizeof(scratch.data)) == HPS_STRING_NO_SPACE);
    assert(memcmp(scratch.data, before_scratch.data, sizeof(scratch.data)) == 0);
    assert(memcmp(storage.data, before_storage.data, sizeof(storage.data)) == 0);
    assert(context.level == 1U && context.at_line_start == 1U && builder.length == 0U);
    assert(hps_string_indent_append_view(&context, exact, scratch.data, 7U) == HPS_STRING_NO_SPACE);
    assert(memcmp(scratch.data, before_scratch.data, sizeof(scratch.data)) == 0);
    assert(hps_string_indent_append_view(&context, exact, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(strcmp(storage.data, "    abc") == 0 && builder.length == 7U && context.at_line_start == 0U);
    const indent_small_t full_storage = storage;
    assert(hps_string_indent_append_newline(&context) == HPS_STRING_NO_SPACE);
    assert(context.at_line_start == 0U && builder.length == 7U && memcmp(storage.data, full_storage.data, sizeof(storage.data)) == 0);
    assert(hps_string_indent_clear(&context) == HPS_STRING_OK);
    assert(hps_string_indent_set_level(&context, 1U) == HPS_STRING_OK);
    assert(hps_string_indent_append_line(&context, two, scratch.data, sizeof(scratch.data)) == HPS_STRING_OK);
    assert(strcmp(storage.data, "    ab\n") == 0 && context.at_line_start == 1U);
    assert(hps_string_indent_clear(&context) == HPS_STRING_OK);
    assert(hps_string_indent_set_level(&context, SIZE_MAX / HPS_STRING_INDENT_WIDTH) == HPS_STRING_OK);
    assert(hps_string_indent_append_indent(&context) == HPS_STRING_NO_SPACE);
    assert(hps_string_indent_append_view(&context, two, scratch.data, sizeof(scratch.data)) == HPS_STRING_NO_SPACE);
    assert(hps_string_indent_set_level(&context, SIZE_MAX) == HPS_STRING_OUT_OF_RANGE);
    assert(context.level == SIZE_MAX / HPS_STRING_INDENT_WIDTH && context.at_line_start == 1U && builder.length == 0U);
}

static void test_invalid_inputs(void)
{
    indent_text_t storage = HPS_STRING_FIXED_STATIC_INIT;
    indent_text_t scratch = HPS_STRING_FIXED_STATIC_INIT;
    hps_string_builder builder = {NULL, 0U, 0U};
    hps_string_indent context = {NULL, 0U, 0U};
    assert(indent_text_bind(&storage, &builder) == HPS_STRING_OK);
    assert(hps_string_indent_init(&context, &builder) == HPS_STRING_OK);
    const hps_string_view text = {"x", 1U};
    const hps_string_view malformed = {NULL, 1U};
    const hps_string_view embedded_nul = {"a\0b", 3U};
    (void)memset(scratch.data, 's', sizeof(scratch.data));
    const indent_text_t original_scratch = scratch;
    assert(hps_string_indent_init(NULL, &builder) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_indent_init(&context, NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(context.builder == &builder && context.at_line_start == 1U);
    assert(hps_string_indent_clear(NULL) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_indent_set_level(NULL, 0U) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_indent_append_view(&context, text, NULL, 0U) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_indent_append_view(&context, malformed, scratch.data, sizeof(scratch.data)) == HPS_STRING_INVALID_ARGUMENT);
    assert(hps_string_indent_append_view(&context, embedded_nul, scratch.data, sizeof(scratch.data)) == HPS_STRING_INVALID_ARGUMENT);
    assert(memcmp(scratch.data, original_scratch.data, sizeof(scratch.data)) == 0 && builder.length == 0U && context.at_line_start == 1U);
    context.at_line_start = 2U;
    assert(hps_string_indent_clear(&context) == HPS_STRING_INVALID_STATE);
    assert(hps_string_indent_append_newline(&context) == HPS_STRING_INVALID_STATE);
    assert(hps_string_indent_append_view(&context, text, scratch.data, sizeof(scratch.data)) == HPS_STRING_INVALID_STATE);
    assert(context.at_line_start == 2U && builder.length == 0U);
    context.at_line_start = 1U;
    builder.length = builder.capacity;
    assert(hps_string_indent_append_indent(&context) == HPS_STRING_INVALID_STATE);
    assert(context.at_line_start == 1U && builder.length == builder.capacity);
}

int32_t test_string_indent(void)
{
    test_persistent_lines();
    test_source_alias_and_binding();
    test_capacity_and_atomic_failures();
    test_invalid_inputs();
    return INT32_C(0);
}
