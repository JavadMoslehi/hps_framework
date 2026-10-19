#include "hps/json/reader.h"
#include "internal.h"

/**
 * @file src/json/reader.c
 * @brief Two-pass strict JSON grammar validation with caller-owned frames.
 */

#define ARRAY_FIRST ((uint8_t)0U)
#define ARRAY_VALUE ((uint8_t)1U)
#define ARRAY_NEXT ((uint8_t)2U)
#define OBJECT_FIRST ((uint8_t)3U)
#define OBJECT_KEY ((uint8_t)4U)
#define OBJECT_COLON ((uint8_t)5U)
#define OBJECT_VALUE ((uint8_t)6U)
#define OBJECT_NEXT ((uint8_t)7U)

static int32_t accept_value(const uint8_t kind, hps_json_frame *frames,
                               const size_t capacity, size_t *depth)
{
    int32_t status = HPS_JSON_OK;
    if ((kind == HPS_JSON_ARRAY_BEGIN) || (kind == HPS_JSON_OBJECT_BEGIN)) {
        if (*depth == capacity) {
            status = HPS_JSON_DEPTH_EXCEEDED;
        } else {
            frames[*depth].kind = kind;
            frames[*depth].state = (kind == HPS_JSON_ARRAY_BEGIN) ?
                ARRAY_FIRST : OBJECT_FIRST;
            *depth += 1U;
        }
    } else if ((kind != HPS_JSON_STRING) && (kind != HPS_JSON_NUMBER) &&
               (kind != HPS_JSON_TRUE) && (kind != HPS_JSON_FALSE) &&
               (kind != HPS_JSON_NULL)) {
        status = HPS_JSON_SYNTAX;
    } else {
        /* Scalar tokens are complete values with no frame reservation. */
    }
    return status;
}

static int32_t accept_nested(const uint8_t kind, hps_json_frame *frames,
                                const size_t capacity, size_t *depth)
{
    hps_json_frame *frame = &frames[*depth - 1U];
    int32_t status = HPS_JSON_OK;
    switch (frame->state) {
        case ARRAY_FIRST:
            if (kind == HPS_JSON_ARRAY_END) {
                *depth -= 1U;
            } else {
                frame->state = ARRAY_NEXT;
                status = accept_value(kind, frames, capacity, depth);
            }
            break;
        case ARRAY_VALUE:
            frame->state = ARRAY_NEXT;
            status = accept_value(kind, frames, capacity, depth);
            break;
        case ARRAY_NEXT:
            if (kind == HPS_JSON_COMMA) {
                frame->state = ARRAY_VALUE;
            } else if (kind == HPS_JSON_ARRAY_END) {
                *depth -= 1U;
            } else {
                status = HPS_JSON_SYNTAX;
            }
            break;
        case OBJECT_FIRST:
            if (kind == HPS_JSON_OBJECT_END) {
                *depth -= 1U;
            } else if (kind == HPS_JSON_STRING) {
                frame->state = OBJECT_COLON;
            } else {
                status = HPS_JSON_SYNTAX;
            }
            break;
        case OBJECT_KEY:
            if (kind == HPS_JSON_STRING) {
                frame->state = OBJECT_COLON;
            } else {
                status = HPS_JSON_SYNTAX;
            }
            break;
        case OBJECT_COLON:
            if (kind == HPS_JSON_COLON) {
                frame->state = OBJECT_VALUE;
            } else {
                status = HPS_JSON_SYNTAX;
            }
            break;
        case OBJECT_VALUE:
            frame->state = OBJECT_NEXT;
            status = accept_value(kind, frames, capacity, depth);
            break;
        case OBJECT_NEXT:
            if (kind == HPS_JSON_COMMA) {
                frame->state = OBJECT_KEY;
            } else if (kind == HPS_JSON_OBJECT_END) {
                *depth -= 1U;
            } else {
                status = HPS_JSON_SYNTAX;
            }
            break;
        default: status = HPS_JSON_INVALID_STATE; break;
    }
    return status;
}

int32_t hps_json_scan_document(const hps_string_view input,
                                  hps_json_frame *frames, const size_t frame_capacity,
                                  hps_json_token *tokens, size_t *count,
                                  size_t *maximum_depth, size_t *compact_length)
{
    hps_json_tokenizer tokenizer;
    hps_json_token token;
    size_t depth = 0U;
    size_t peak = 0U;
    size_t total = 0U;
    size_t length = 0U;
    uint8_t root_seen = 0U;
    uint8_t complete = 0U;
    int32_t status = hps_json_tokenizer_init(&tokenizer, input);
    while ((status == HPS_JSON_OK) && (complete == 0U)) {
        status = hps_json_tokenizer_next(&tokenizer, &token);
        if (status == HPS_JSON_OK) {
            if (token.kind == HPS_JSON_EOF) {
                complete = 1U;
                if ((depth != 0U) || (root_seen == 0U)) {
                    status = HPS_JSON_SYNTAX;
                }
            } else {
                if (depth > 0U) {
                    status = accept_nested(token.kind, frames, frame_capacity, &depth);
                } else if (root_seen == 1U) {
                    status = HPS_JSON_SYNTAX;
                } else {
                    root_seen = 1U;
                    status = accept_value(token.kind, frames, frame_capacity, &depth);
                }
                if ((status == HPS_JSON_OK) &&
                    ((total == SIZE_MAX) || (token.raw.length > input.length - length))) {
                    status = HPS_JSON_OUT_OF_RANGE;
                }
                if (status == HPS_JSON_OK) {
                    if (tokens != NULL) {
                        tokens[total] = token;
                    }
                    total += 1U;
                    length += token.raw.length;
                    if (depth > peak) {
                        peak = depth;
                    }
                }
            }
        }
    }
    if (status == HPS_JSON_OK) {
        *count = total;
        *maximum_depth = peak;
        *compact_length = length;
    }
    return status;
}

int32_t hps_json_read(const hps_string_view input,
                        hps_json_token *tokens, const size_t token_capacity,
                        hps_json_frame *frames, const size_t frame_capacity,
                        hps_json_document *document)
{
    int32_t status = HPS_JSON_INVALID_ARGUMENT;
    if ((document != NULL) && (token_capacity <= SIZE_MAX / sizeof(hps_json_token)) &&
        (frame_capacity <= SIZE_MAX / sizeof(hps_json_frame)) &&
        ((tokens != NULL) || (token_capacity == 0U)) &&
        ((frames != NULL) || (frame_capacity == 0U))) {
        size_t count = 0U;
        size_t peak = 0U;
        size_t length = 0U;
        status = hps_json_scan_document(input, frames, frame_capacity, NULL,
                                          &count, &peak, &length);
        if ((status == HPS_JSON_OK) && (count > token_capacity)) {
            status = HPS_JSON_NO_SPACE;
        }
        if (status == HPS_JSON_OK) {
            status = hps_json_scan_document(input, frames, frame_capacity, tokens,
                                              &count, &peak, &length);
            if (status == HPS_JSON_OK) {
                const hps_json_document result = {input, tokens, count, peak};
                *document = result;
            }
        }
    }
    return status;
}
