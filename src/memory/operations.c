#include "hps/memory/hps_operations.h"

#include <string.h>

void *hps_memset(void *destination, uint8_t value, size_t count)
{
    if (count != 0U) {
        (void)memset(destination, value, count);
    }
    return destination;
}

void *hps_memcpy(void *destination, const void *source, size_t count)
{
    if (count != 0U) {
        (void)memcpy(destination, source, count);
    }
    return destination;
}

void *hps_memmove(void *destination, const void *source, size_t count)
{
    /* libc owns overlap handling; do not order pointers to separate objects. */
    if (count != 0U) {
        (void)memmove(destination, source, count);
    }
    return destination;
}

int32_t hps_memcmp(const void *left, const void *right, size_t count)
{
    const uint8_t *left_bytes = left;
    const uint8_t *right_bytes = right;
    int32_t result = INT32_C(0);
    size_t index = 0U;

    /* libc memcmp promises only a sign; retain the draft's exact difference. */
    while ((index < count) && (result == INT32_C(0))) {
        result = (int32_t)left_bytes[index] - (int32_t)right_bytes[index];
        index += 1U;
    }
    return result;
}

const void *hps_memchr(const void *source, uint8_t value, size_t count)
{
    const void *result = NULL;
    if (count != 0U) {
        result = memchr(source, value, count);
    }
    return result;
}

uint8_t hps_pointer_is_nonnull(const void *pointer)
{
    uint8_t result = UINT8_C(0);
    if (pointer != NULL) {
        result = UINT8_C(1);
    }
    return result;
}

void hps_memswap(void *left, void *right, size_t count)
{
    uint8_t temporary[HPS_MEMSWAP_SCRATCH_BYTES];
    uint8_t *left_bytes = left;
    uint8_t *right_bytes = right;
    size_t remaining = count;

    if (left != right) {
        while (remaining != 0U) {
            size_t chunk = sizeof(temporary);
            if (remaining < chunk) {
                chunk = remaining;
            }
            (void)hps_memcpy(temporary, left_bytes, chunk);
            (void)hps_memcpy(left_bytes, right_bytes, chunk);
            (void)hps_memcpy(right_bytes, temporary, chunk);
            left_bytes += chunk;
            right_bytes += chunk;
            remaining -= chunk;
        }
    }
}

uint8_t hps_memequal_full_scan(const void *left, const void *right,
                              size_t count)
{
    const uint8_t *left_bytes = left;
    const uint8_t *right_bytes = right;
    uint8_t difference = UINT8_C(0);
    uint8_t result = UINT8_C(0);

    for (size_t index = 0U; index < count; index += 1U) {
        difference = (uint8_t)(difference |
                               (uint8_t)(left_bytes[index] ^ right_bytes[index]));
    }
    if (difference == UINT8_C(0)) {
        result = UINT8_C(1);
    }
    return result;
}

void hps_memzero_volatile(void *destination, size_t count)
{
    volatile uint8_t *bytes = destination;
    size_t index = 0U;
    while (index < count) {
        bytes[index] = 0U;
        index += 1U;
    }
}
