#include "hps/json/writer.h"
#include "internal.h"

#include <string.h>

/**
 * @file src/json/writer.c
 * @brief Validate full input and output extent before compact JSON emission.
 */

int32_t hps_json_write_compact(hps_string_buffer *destination,
                                 const hps_string_view input,
                                 hps_json_frame *frames, const size_t frame_capacity)
{
    int32_t status = HPS_JSON_INVALID_ARGUMENT;
    if (destination != NULL) {
        hps_string_view old;
        const int32_t string_status = hps_string_buffer_as_view(destination, &old);
        status = (string_status == HPS_STRING_OK) ? HPS_JSON_OK : HPS_JSON_INVALID_STATE;
        if ((status == HPS_JSON_OK) &&
            (((frames == NULL) && (frame_capacity > 0U)) ||
             (frame_capacity > SIZE_MAX / sizeof(hps_json_frame)))) {
            status = HPS_JSON_INVALID_ARGUMENT;
        }
        size_t count = 0U;
        size_t peak = 0U;
        size_t length = 0U;
        if (status == HPS_JSON_OK) {
            status = hps_json_scan_document(input, frames, frame_capacity, NULL,
                                              &count, &peak, &length);
        }
        if ((status == HPS_JSON_OK) && (length >= destination->capacity)) {
            status = HPS_JSON_NO_SPACE;
        }
        if (status == HPS_JSON_OK) {
            hps_json_tokenizer tokenizer;
            hps_json_token token;
            size_t written = 0U;
            status = hps_json_tokenizer_init(&tokenizer, input);
            /* Immutable input was fully preflighted; exactly count tokens remain.
             * Their cumulative length fits the validated destination capacity. */
            for (size_t index = 0U;
                 (index < count) && (status == HPS_JSON_OK); index += 1U) {
                status = hps_json_tokenizer_next(&tokenizer, &token);
                if (status == HPS_JSON_OK) {
                    (void)memcpy(&destination->data[written], token.raw.data, token.raw.length);
                    written += token.raw.length;
                }
            }
            if (status == HPS_JSON_OK) {
                destination->data[written] = '\0';
                destination->length = written;
            }
        }
    }
    return status;
}
