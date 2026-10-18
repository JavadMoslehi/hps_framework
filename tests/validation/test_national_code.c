#include "../test_suites.h"
#include "hps/validation/national_code.h"
#include <assert.h>

/**
 * @file tests/validation/test_national_code.c
 * @brief Synthetic checksum fixtures and bounded-input/output preservation.
 */

int32_t test_national_code(void)
{
    /* For prefix123456789 the weighted sum210 leaves remainder1. */
    uint8_t valid = 97U;
    int32_t status = hps_iran_national_code_validate("1234567891", 11U, &valid);
    assert(status == HPS_STRING_OK && valid == 1U);
    status = hps_iran_national_code_validate("1234567892", 11U, &valid);
    assert(status == HPS_STRING_OK && valid == 0U);
    status = hps_iran_national_code_validate("0000000000", 11U, &valid);
    assert(status == HPS_STRING_OK && valid == 0U);
    status = hps_iran_national_code_validate("9999999999", 11U, &valid);
    assert(status == HPS_STRING_OK && valid == 0U);
    status = hps_iran_national_code_validate("123x567891", 11U, &valid);
    assert(status == HPS_STRING_OK && valid == 0U);
    status = hps_iran_national_code_validate("123", 4U, &valid);
    assert(status == HPS_STRING_OK && valid == 0U);
    valid = 97U;
    const char missing_nul[10] = {'1','2','3','4','5','6','7','8','9','1'};
    status = hps_iran_national_code_validate(missing_nul, sizeof(missing_nul), &valid);
    assert(status == HPS_STRING_NOT_TERMINATED && valid == 97U);
    status = hps_iran_national_code_validate(NULL, 11U, &valid);
    assert(status == HPS_STRING_INVALID_ARGUMENT && valid == 97U);
    status = hps_iran_national_code_validate("1234567891", 0U, &valid);
    assert(status == HPS_STRING_INVALID_ARGUMENT && valid == 97U);
    status = hps_iran_national_code_validate("1234567891", 11U, NULL);
    assert(status == HPS_STRING_INVALID_ARGUMENT);
    /* Opposite checksum branch: prefix100000000 sum10 => expected1. */
    status = hps_iran_national_code_validate("1000000001", 11U, &valid);
    assert(status == HPS_STRING_OK && valid == 1U);
    return INT32_C(0);
}
