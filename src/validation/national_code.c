#include "hps/validation/hps_national_code.h"

/**
 * @file src/validation/national_code.c
 * @brief Length-checked legacy checksum using bounded exact-width arithmetic.
 */

int32_t hps_iran_national_code_validate(const char *text,
                                        const size_t capacity,
                                        uint8_t *result)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if (result != NULL) {
        hps_string_view view = {NULL, 0U};
        status = hps_string_view_init_cstr(&view, text, capacity);
        if (status == HPS_STRING_OK) {
            uint8_t valid = 0U;
            if (view.length == 10U) {
                const char *digits = "0123456789";
                uint8_t values[10] = {0U};
                uint8_t all_digits = 1U;
                uint8_t all_same = 1U;
                size_t index = 0U;
                while (index < 10U) {
                    size_t digit = 0U;
                    while ((digit < 10U) && (view.data[index] != digits[digit])) {
                        digit += 1U;
                    }
                    if (digit == 10U) {
                        all_digits = 0U;
                    } else {
                        values[index] = (uint8_t)digit;
                    }
                    if (view.data[index] != view.data[0]) {
                        all_same = 0U;
                    }
                    index += 1U;
                }
                if ((all_digits == 1U) && (all_same == 0U)) {
                    uint32_t sum = 0U;
                    uint32_t weight = 10U;
                    for (index = 0U; index < 9U; index += 1U) {
                        /* Maximum sum is 9*(10+...+2)=486. */
                        sum += (uint32_t)values[index] * weight;
                        weight -= 1U;
                    }
                    const uint32_t remainder = sum % UINT32_C(11);
                    const uint32_t expected = (remainder < 2U) ? remainder :
                        UINT32_C(11) - remainder;
                    valid = (uint8_t)((uint32_t)values[9] == expected);
                }
            }
            *result = valid;
        }
    }
    return status;
}
