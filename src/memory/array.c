#include "hps/memory/array.h"

#include "hps/memory/operations.h"

int32_t hps_memory_array_size(size_t count, size_t element_size, size_t *size)
{
    int32_t status = HPS_MEMORY_ARRAY_INVALID_ARGUMENT;

    if ((size != NULL) && ((count == 0U) || (element_size != 0U))) {
        if ((element_size != 0U) && (count > (SIZE_MAX / element_size))) {
            status = HPS_MEMORY_ARRAY_SIZE_OVERFLOW;
        } else {
            *size = count * element_size;
            status = HPS_MEMORY_ARRAY_OK;
        }
    }
    return status;
}

int32_t hps_memory_array_clear(void *destination, size_t count,
                              size_t element_size)
{
    int32_t status = HPS_MEMORY_ARRAY_INVALID_ARGUMENT;
    size_t size = 0U;

    if ((count == 0U) || (destination != NULL)) {
        status = hps_memory_array_size(count, element_size, &size);
        if (status == HPS_MEMORY_ARRAY_OK) {
            (void)hps_memset(destination, UINT8_C(0), size);
        }
    }
    return status;
}

int32_t hps_memory_array_copy(void *destination, const void *source,
                             size_t count, size_t element_size)
{
    int32_t status = HPS_MEMORY_ARRAY_INVALID_ARGUMENT;
    size_t size = 0U;

    if ((count == 0U) || ((destination != NULL) && (source != NULL))) {
        status = hps_memory_array_size(count, element_size, &size);
        if (status == HPS_MEMORY_ARRAY_OK) {
            (void)hps_memcpy(destination, source, size);
        }
    }
    return status;
}

int32_t hps_memory_array_move(void *destination, const void *source,
                             size_t count, size_t element_size)
{
    int32_t status = HPS_MEMORY_ARRAY_INVALID_ARGUMENT;
    size_t size = 0U;

    if ((count == 0U) || ((destination != NULL) && (source != NULL))) {
        status = hps_memory_array_size(count, element_size, &size);
        if (status == HPS_MEMORY_ARRAY_OK) {
            (void)hps_memmove(destination, source, size);
        }
    }
    return status;
}

int32_t hps_memory_array_compare(const void *left, const void *right,
                                size_t count, size_t element_size,
                                int32_t *comparison)
{
    int32_t status = HPS_MEMORY_ARRAY_INVALID_ARGUMENT;
    size_t size = 0U;

    if (comparison != NULL) {
        if ((count == 0U) || ((left != NULL) && (right != NULL))) {
            status = hps_memory_array_size(count, element_size, &size);
            if (status == HPS_MEMORY_ARRAY_OK) {
                *comparison = hps_memcmp(left, right, size);
            }
        }
    }
    return status;
}
