#include "../test_suites.h"
#include "hps/data_structure/optional.h"
#include "hps/data_structure/optional.h"

#include <assert.h>
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"

/**
 * @file tests/data_structure/test_optional.c
 * @brief Typed absent/present values, pointer nullness and checked failure paths.
 */

typedef struct {
    uint16_t code;
    uint8_t flag;
} hps_test_optional_pair;

typedef const uint16_t *hps_test_optional_pointer;

HPS_OPTIONAL_DEFINE(uint32_t, hps_test_optional_scalar)
HPS_OPTIONAL_DEFINE(hps_test_optional_pair, hps_test_optional_struct)
HPS_OPTIONAL_DEFINE(hps_test_optional_pointer, hps_test_optional_ref)

static void test_scalar_lifecycle(void)
{
    hps_test_optional_scalar_t optional = hps_test_optional_scalar_none();
    const hps_test_optional_scalar_t present = hps_test_optional_scalar_some(UINT32_MAX);
    uint32_t value = UINT32_C(77);
    uint8_t found = UINT8_C(77);
    assert(optional.has_value == UINT8_C(0) && optional.value == UINT32_C(0));
    assert(present.has_value == UINT8_C(1) && present.value == UINT32_MAX);
    assert(hps_test_optional_scalar_check(&optional) == HPS_OPTIONAL_OK);
    assert(hps_test_optional_scalar_has(&optional, &found) == HPS_OPTIONAL_OK && found == UINT8_C(0));
    assert(hps_test_optional_scalar_get(&optional, &value) == HPS_OPTIONAL_EMPTY && value == UINT32_C(77));
    assert(hps_test_optional_scalar_get_or(&optional, UINT32_C(12), &value) == HPS_OPTIONAL_OK && value == UINT32_C(12));
    assert(hps_test_optional_scalar_set_some(&optional, UINT32_MAX) == HPS_OPTIONAL_OK);
    assert(hps_test_optional_scalar_has(&optional, &found) == HPS_OPTIONAL_OK && found == UINT8_C(1));
    assert(hps_test_optional_scalar_get(&optional, &value) == HPS_OPTIONAL_OK && value == UINT32_MAX);
    assert(hps_test_optional_scalar_get_or(&optional, UINT32_C(12), &value) == HPS_OPTIONAL_OK && value == UINT32_MAX);
    assert(hps_test_optional_scalar_set_none(&optional) == HPS_OPTIONAL_OK);
    /* Clearing changes presence only: it supplies no sensitive-erasure claim. */
    assert(optional.has_value == UINT8_C(0) && optional.value == UINT32_MAX);
    assert(hps_test_optional_scalar_set_none(&optional) == HPS_OPTIONAL_OK);
    value = UINT32_C(77);
    assert(hps_test_optional_scalar_get(&optional, &value) == HPS_OPTIONAL_EMPTY && value == UINT32_C(77));
    optional = present;
    assert(hps_test_optional_scalar_get(&optional, &value) == HPS_OPTIONAL_OK && value == UINT32_MAX);
    assert(hps_test_optional_scalar_set_some(&optional, UINT32_C(0)) == HPS_OPTIONAL_OK);
    assert(hps_test_optional_scalar_get(&optional, &value) == HPS_OPTIONAL_OK && value == UINT32_C(0));
}

static uint32_t counted_value(size_t *calls)
{
    *calls += 1U;
    return UINT32_C(42);
}

static hps_test_optional_scalar_t *counted_optional(
    hps_test_optional_scalar_t *optional, size_t *calls)
{
    *calls += 1U;
    return optional;
}

static void test_argument_evaluation(void)
{
    size_t calls = 0U;
    hps_test_optional_scalar_t optional = hps_test_optional_scalar_some(counted_value(&calls));
    uint32_t value = UINT32_C(0);
    uint8_t found = UINT8_C(77);
    assert(calls == 1U);
    calls = 0U;
    assert(hps_test_optional_scalar_get_or(&optional, counted_value(&calls), &value) == HPS_OPTIONAL_OK);
    assert(calls == 1U && value == UINT32_C(42));
    calls = 0U;
    assert(hps_test_optional_scalar_has(counted_optional(&optional, &calls), &found) == HPS_OPTIONAL_OK);
    assert(calls == 1U && found == UINT8_C(1));
    calls = 0U;
    assert(hps_test_optional_scalar_set_none(counted_optional(&optional, &calls)) == HPS_OPTIONAL_OK);
    assert(calls == 1U && optional.has_value == UINT8_C(0));
    calls = 0U;
    assert(hps_test_optional_scalar_set_some(&optional, counted_value(&calls)) == HPS_OPTIONAL_OK);
    assert(calls == 1U && optional.value == UINT32_C(42));
}

static void test_failure_priority_and_preservation(void)
{
    hps_test_optional_scalar_t optional = hps_test_optional_scalar_some(UINT32_C(42));
    uint32_t value = UINT32_C(77);
    uint8_t found = UINT8_C(77);
    assert(hps_test_optional_scalar_check(NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_has(NULL, &found) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_get(NULL, &value) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_get_or(NULL, UINT32_C(12), &value) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_set_none(NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_set_some(NULL, UINT32_C(12)) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(value == UINT32_C(77) && found == UINT8_C(77));
    assert(hps_test_optional_scalar_has(&optional, NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_get(&optional, NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_get_or(&optional, UINT32_C(12), NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(optional.has_value == UINT8_C(1) && optional.value == UINT32_C(42));

    optional.has_value = UINT8_MAX;
    assert(hps_test_optional_scalar_check(&optional) == HPS_OPTIONAL_INVALID_STATE);
    assert(hps_test_optional_scalar_has(&optional, &found) == HPS_OPTIONAL_INVALID_STATE);
    assert(hps_test_optional_scalar_get(&optional, &value) == HPS_OPTIONAL_INVALID_STATE);
    assert(hps_test_optional_scalar_get_or(&optional, UINT32_C(12), &value) == HPS_OPTIONAL_INVALID_STATE);
    assert(hps_test_optional_scalar_set_none(&optional) == HPS_OPTIONAL_INVALID_STATE);
    assert(hps_test_optional_scalar_set_some(&optional, UINT32_C(12)) == HPS_OPTIONAL_INVALID_STATE);
    assert(optional.has_value == UINT8_MAX && optional.value == UINT32_C(42));
    assert(value == UINT32_C(77) && found == UINT8_C(77));
    assert(hps_test_optional_scalar_has(&optional, NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_get(&optional, NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(hps_test_optional_scalar_get_or(&optional, UINT32_C(12), NULL) == HPS_OPTIONAL_INVALID_ARGUMENT);
    assert(optional.has_value == UINT8_MAX && optional.value == UINT32_C(42));
}

static void test_struct_values(void)
{
    const hps_test_optional_pair input = {UINT16_MAX, UINT8_C(1)};
    const hps_test_optional_pair fallback = {UINT16_C(7), UINT8_C(0)};
    hps_test_optional_pair value = fallback;
    hps_test_optional_struct_t optional = hps_test_optional_struct_none();
    const hps_test_optional_struct_t present = hps_test_optional_struct_some(input);
    uint8_t found = UINT8_C(77);
    assert(optional.value.code == UINT16_C(0) && optional.value.flag == UINT8_C(0));
    assert(hps_test_optional_struct_check(&optional) == HPS_OPTIONAL_OK);
    assert(hps_test_optional_struct_has(&optional, &found) == HPS_OPTIONAL_OK && found == UINT8_C(0));
    assert(hps_test_optional_struct_get(&optional, &value) == HPS_OPTIONAL_EMPTY);
    assert(value.code == fallback.code && value.flag == fallback.flag);
    assert(hps_test_optional_struct_get_or(&optional, fallback, &value) == HPS_OPTIONAL_OK);
    assert(value.code == fallback.code && value.flag == fallback.flag);
    assert(hps_test_optional_struct_set_some(&optional, input) == HPS_OPTIONAL_OK);
    assert(hps_test_optional_struct_get(&optional, &value) == HPS_OPTIONAL_OK);
    assert(value.code == UINT16_MAX && value.flag == UINT8_C(1));
    assert(hps_test_optional_struct_get_or(&present, fallback, &value) == HPS_OPTIONAL_OK);
    assert(value.code == UINT16_MAX && value.flag == UINT8_C(1));
    assert(hps_test_optional_struct_set_none(&optional) == HPS_OPTIONAL_OK);
    assert(optional.value.code == UINT16_MAX && optional.value.flag == UINT8_C(1));
}

static void test_const_pointer_values(void)
{
    const uint16_t target = UINT16_C(42);
    const uint16_t fallback = UINT16_C(7);
    hps_test_optional_pointer value = &fallback;
    hps_test_optional_ref_t optional = hps_test_optional_ref_none();
    const hps_test_optional_ref_t present_null = hps_test_optional_ref_some(NULL);
    uint8_t found = UINT8_C(77);
    assert(optional.value == NULL);
    assert(hps_test_optional_ref_check(&optional) == HPS_OPTIONAL_OK);
    assert(hps_test_optional_ref_has(&optional, &found) == HPS_OPTIONAL_OK && found == UINT8_C(0));
    assert(hps_test_optional_ref_get(&optional, &value) == HPS_OPTIONAL_EMPTY && value == &fallback);
    assert(hps_test_optional_ref_get_or(&optional, &fallback, &value) == HPS_OPTIONAL_OK && value == &fallback);
    assert(hps_test_optional_ref_get(&present_null, &value) == HPS_OPTIONAL_OK && value == NULL);
    assert(hps_test_optional_ref_has(&present_null, &found) == HPS_OPTIONAL_OK && found == UINT8_C(1));
    assert(hps_test_optional_ref_set_some(&optional, &target) == HPS_OPTIONAL_OK);
    assert(hps_test_optional_ref_get(&optional, &value) == HPS_OPTIONAL_OK && value == &target && *value == UINT16_C(42));
    assert(hps_test_optional_ref_get_or(&optional, NULL, &value) == HPS_OPTIONAL_OK && value == &target);
    assert(hps_test_optional_ref_set_none(&optional) == HPS_OPTIONAL_OK);
    assert(optional.value == &target && target == UINT16_C(42));
}

int32_t test_optional(void)
{
    test_scalar_lifecycle();
    test_argument_evaluation();
    test_failure_priority_and_preservation();
    test_struct_values();
    test_const_pointer_values();
    return INT32_C(0);
}
