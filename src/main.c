#include "hps/memory/pool.h"
#include <stdint.h>

/**
 * @file src/main.c
 * @brief Hosted example of the static byte-block malloc-family API.
 */

/* Hosted entry-point exception: HPS-C-005. One context, no retries or I/O. */
int main(void)
{
    int32_t status = hps_pool_init();
    if (status == HPS_POOL_OK) {
        uint8_t *bytes = hps_calloc(16U, sizeof(uint8_t));
        status = hps_pool_last_status();
        if (bytes != NULL) {
            bytes[0] = 42U;
            /* Resize stays in the same fixed block. Preserve the old owner on
             * failure rather than assigning realloc directly to it. */
            uint8_t *resized = hps_realloc(bytes, 64U);
            status = hps_pool_last_status();
            if (resized != NULL) {
                bytes = resized;
            }
            hps_free(bytes);
            if (status == HPS_POOL_OK) {
                status = hps_pool_last_status();
            }
        }
    }
    return (status == HPS_POOL_OK) ? 0 : 1;
}
