#include "../test_suites.h"
#include "hps/enum/hps_traits.h"
#include <assert.h>
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/hps_bool.h"
#include <string.h>

/**
 * @file tests/enum/test_enum.c
 * @brief Prepared native enum-traits and legacy API contract tests.
 */

typedef enum {
    HPS_TEST_ENUM_TEN = 10,
    HPS_TEST_ENUM_NEGATIVE = -5,
    HPS_TEST_ENUM_FORTY_TWO = 42
} hps_test_enum_type;

#define HPS_TEST_ENUM_ITEMS(item) \
    item(HPS_TEST_ENUM_TEN, "Ten") \
    item(HPS_TEST_ENUM_NEGATIVE, "Negative") \
    item(HPS_TEST_ENUM_FORTY_TWO, "FortyTwo")

HPS_ENUM_DEFINE_INLINE(hps_test_enum_type, hps_test_enum_inline,
                       HPS_TEST_ENUM_ITEMS)
HPS_ENUM_DECLARE(hps_test_enum_type, hps_test_enum_external,
                 HPS_TEST_ENUM_ITEMS)
HPS_ENUM_IMPLEMENT(hps_test_enum_type, hps_test_enum_external,
                   HPS_TEST_ENUM_ITEMS)

typedef struct {
    size_t visited;
    size_t stop_after;
} hps_test_enum_iteration;

static bool hps_test_enum_visit(hps_test_enum_type value, const char *name,
                                void *ctx)
{
    const hps_test_enum_type expected_values[3] = {
        HPS_TEST_ENUM_TEN, HPS_TEST_ENUM_NEGATIVE, HPS_TEST_ENUM_FORTY_TWO
    };
    const char * const expected_names[3] = {"Ten", "Negative", "FortyTwo"};
    hps_test_enum_iteration *state = ctx;
    assert(state != NULL);
    assert(state->visited < (size_t)3U);
    assert(value == expected_values[state->visited]);
    assert(strcmp(name, expected_names[state->visited]) == 0);
    state->visited += (size_t)1U;
    return state->visited < state->stop_after;
}

static void hps_test_enum_names_and_parse(void)
{
    hps_test_enum_type value = (hps_test_enum_type)INT32_C(99);
    bool flag = hps_test_enum_inline_from_cstr("Ten", &value);
    assert(flag == true && value == HPS_TEST_ENUM_TEN);
    flag = hps_test_enum_inline_from_name("Negative", &value);
    assert(flag == true && value == HPS_TEST_ENUM_NEGATIVE);
    flag = hps_test_enum_inline_from_name_ignore_case("fOrTyTwO", &value);
    assert(flag == true && value == HPS_TEST_ENUM_FORTY_TWO);
    flag = hps_test_enum_inline_from_name("ten", &value);
    assert(flag == false && value == HPS_TEST_ENUM_FORTY_TWO);
    flag = hps_test_enum_inline_from_name("missing", &value);
    assert(flag == false && value == HPS_TEST_ENUM_FORTY_TWO);
    flag = hps_test_enum_inline_from_name(NULL, &value);
    assert(flag == false && value == HPS_TEST_ENUM_FORTY_TWO);
    flag = hps_test_enum_inline_from_name("Ten", NULL);
    assert(flag == false);

    flag = hps_test_enum_inline_try_parse("Ten", &value);
    assert(flag == true && value == HPS_TEST_ENUM_TEN);
    flag = hps_test_enum_inline_try_parse_ignore_case("fOrTyTwO", &value);
    assert(flag == true && value == HPS_TEST_ENUM_FORTY_TWO);

    assert(strcmp(hps_test_enum_inline_to_name(HPS_TEST_ENUM_TEN), "Ten") == 0);
    assert(strcmp(hps_test_enum_inline_to_cstr(INT32_C(0)), "UNKNOWN") == 0);
    assert(strcmp(hps_test_enum_inline_safe_name(INT32_C(0), "fallback"),
                  "fallback") == 0);
    assert(strcmp(hps_test_enum_inline_safe_name(INT32_C(0), NULL), "UNKNOWN") == 0);
    assert(strcmp(hps_test_enum_inline_safe_name(HPS_TEST_ENUM_NEGATIVE, "fallback"),
                  "Negative") == 0);

    /* Caller-owned text buffers use the imported legacy failure behavior. */
    flag = hps_test_enum_inline_to_string(HPS_TEST_ENUM_TEN, NULL, (size_t)8U);
    assert(flag == false);
    flag = hps_test_enum_inline_format(HPS_TEST_ENUM_TEN, NULL, (size_t)64U);
    assert(flag == false);
    flag = hps_enum_cstr_copy(NULL, (size_t)8U, "Ten");
    assert(flag == false);
    flag = hps_enum_cstr_append(NULL, (size_t)8U, "Ten");
    assert(flag == false);
}

static void hps_test_enum_ranges_and_values(void)
{
    assert(hps_test_enum_inline_count() == (size_t)3U);
    assert(hps_test_enum_inline_first() == HPS_TEST_ENUM_TEN);
    assert(hps_test_enum_inline_last() == HPS_TEST_ENUM_FORTY_TWO);
    assert(hps_test_enum_inline_min_value() == HPS_TEST_ENUM_NEGATIVE);
    assert(hps_test_enum_inline_max_value() == HPS_TEST_ENUM_FORTY_TWO);
    assert(hps_test_enum_inline_is_valid(HPS_TEST_ENUM_NEGATIVE) == true);
    assert(hps_test_enum_inline_is_unknown(INT32_C(0)) == true);
    assert(hps_test_enum_inline_is_defined(INT32_C(0)) == false);
    assert(hps_test_enum_inline_contains(HPS_TEST_ENUM_TEN) == true);
    assert(hps_test_enum_inline_contains(INT32_C(0)) == false);
    assert(hps_test_enum_inline_is_first(HPS_TEST_ENUM_TEN) == true);
    assert(hps_test_enum_inline_is_first(HPS_TEST_ENUM_NEGATIVE) == false);
    assert(hps_test_enum_inline_is_last(HPS_TEST_ENUM_FORTY_TWO) == true);
    assert(hps_test_enum_inline_compare(HPS_TEST_ENUM_NEGATIVE, HPS_TEST_ENUM_TEN) ==
           -INT32_C(1));
    assert(hps_test_enum_inline_compare(HPS_TEST_ENUM_TEN, HPS_TEST_ENUM_NEGATIVE) ==
           INT32_C(1));
    assert(hps_test_enum_inline_compare(HPS_TEST_ENUM_TEN, HPS_TEST_ENUM_TEN) ==
           INT32_C(0));
    assert(hps_test_enum_inline_to_i32(HPS_TEST_ENUM_NEGATIVE) == -INT32_C(5));

    hps_test_enum_type value = (hps_test_enum_type)INT32_C(99);
    bool flag = hps_test_enum_inline_from_i32(INT32_MIN, &value);
    assert(flag == false && value == INT32_C(99));
    flag = hps_test_enum_inline_from_i32(INT32_MAX, &value);
    assert(flag == false && value == INT32_C(99));
    flag = hps_test_enum_inline_from_i32(INT32_C(10), &value);
    assert(flag == true && value == HPS_TEST_ENUM_TEN);

    hps_test_enum_type values[3] = {
        (hps_test_enum_type)INT32_C(99), (hps_test_enum_type)INT32_C(99),
        (hps_test_enum_type)INT32_C(99)
    };
    size_t count = hps_test_enum_inline_values(values, (size_t)2U);
    assert(count == (size_t)2U);
    assert(values[0] == HPS_TEST_ENUM_TEN && values[1] == HPS_TEST_ENUM_NEGATIVE);
    assert(values[2] == INT32_C(99));
    count = hps_test_enum_inline_values(NULL, (size_t)3U);
    assert(count == (size_t)0U);
    size_t index = SIZE_MAX;
    flag = hps_test_enum_inline_index_of(HPS_TEST_ENUM_NEGATIVE, &index);
    assert(flag == true && index == (size_t)1U);
    flag = hps_test_enum_inline_index_of(INT32_C(0), &index);
    assert(flag == false && index == HPS_ENUM_NPOS);
    flag = hps_test_enum_inline_index_of(HPS_TEST_ENUM_TEN, NULL);
    assert(flag == true);
    index = (size_t)1U;
    flag = hps_test_enum_inline_value_at(index, &value);
    assert(flag == true && value == HPS_TEST_ENUM_NEGATIVE);
    assert(strcmp(hps_test_enum_inline_name_at(index), "Negative") == 0);
    flag = hps_test_enum_inline_value_at(SIZE_MAX, &value);
    assert(flag == false && value == HPS_TEST_ENUM_NEGATIVE);
    assert(strcmp(hps_test_enum_inline_name_at(SIZE_MAX), "UNKNOWN") == 0);
}

static void hps_test_enum_navigation_and_callbacks(void)
{
    hps_test_enum_type value = (hps_test_enum_type)INT32_C(99);
    bool flag = hps_test_enum_inline_next(HPS_TEST_ENUM_TEN, &value);
    assert(flag == true && value == HPS_TEST_ENUM_NEGATIVE);
    flag = hps_test_enum_inline_previous(HPS_TEST_ENUM_FORTY_TWO, &value);
    assert(flag == true && value == HPS_TEST_ENUM_NEGATIVE);
    flag = hps_test_enum_inline_next(HPS_TEST_ENUM_FORTY_TWO, &value);
    assert(flag == false && value == HPS_TEST_ENUM_NEGATIVE);
    flag = hps_test_enum_inline_previous(HPS_TEST_ENUM_TEN, &value);
    assert(flag == false && value == HPS_TEST_ENUM_NEGATIVE);
    flag = hps_test_enum_inline_next_wrap(HPS_TEST_ENUM_FORTY_TWO, &value);
    assert(flag == true && value == HPS_TEST_ENUM_TEN);
    flag = hps_test_enum_inline_previous_wrap(HPS_TEST_ENUM_TEN, &value);
    assert(flag == true && value == HPS_TEST_ENUM_FORTY_TWO);
    flag = hps_test_enum_inline_next_wrap(INT32_C(0), &value);
    assert(flag == false && value == HPS_TEST_ENUM_FORTY_TWO);
    int32_t distance = INT32_C(99);
    flag = hps_test_enum_inline_distance(HPS_TEST_ENUM_TEN, HPS_TEST_ENUM_FORTY_TWO,
                                        &distance);
    assert(flag == true && distance == INT32_C(2));
    flag = hps_test_enum_inline_distance(HPS_TEST_ENUM_FORTY_TWO, HPS_TEST_ENUM_TEN,
                                        &distance);
    assert(flag == true && distance == -INT32_C(2));
    flag = hps_test_enum_inline_distance(INT32_C(0), HPS_TEST_ENUM_TEN, &distance);
    assert(flag == false && distance == -INT32_C(2));

    hps_test_enum_iteration state = {(size_t)0U, (size_t)4U};
    flag = hps_test_enum_inline_for_each(hps_test_enum_visit, &state);
    assert(flag == true && state.visited == (size_t)3U);
    state.visited = (size_t)0U;
    state.stop_after = (size_t)2U;
    flag = hps_test_enum_inline_for_each(hps_test_enum_visit, &state);
    assert(flag == false && state.visited == (size_t)2U);
    flag = hps_test_enum_inline_for_each(NULL, &state);
    assert(flag == false && state.visited == (size_t)2U);

    /* Separate declaration/implementation mode retains the same sparse order. */
    assert(hps_test_enum_external_count() == (size_t)3U);
    assert(hps_test_enum_external_first() == HPS_TEST_ENUM_TEN);
    assert(hps_test_enum_external_min_value() == HPS_TEST_ENUM_NEGATIVE);
    flag = hps_test_enum_external_from_cstr("FortyTwo", &value);
    assert(flag == true && value == HPS_TEST_ENUM_FORTY_TWO);
    flag = hps_test_enum_external_previous_wrap(HPS_TEST_ENUM_TEN, &value);
    assert(flag == true && value == HPS_TEST_ENUM_FORTY_TWO);
}

int32_t test_enum(void)
{
    hps_test_enum_names_and_parse();
    hps_test_enum_ranges_and_values();
    hps_test_enum_navigation_and_callbacks();
    return INT32_C(0);
}
