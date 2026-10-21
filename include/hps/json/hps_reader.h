#ifndef HPS_JSON_HPS_READER_H
#define HPS_JSON_HPS_READER_H

/**
 * @file hps/json/hps_reader.h
 * @brief Iterative JSON grammar validation with caller token and depth budgets.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/json/hps_tokenizer.h"

/* Scratch frame fields are implementation state, not an application format. */
typedef struct {
    uint8_t kind;
    uint8_t state;
} hps_json_frame;

typedef struct {
    hps_string_view input;
    const hps_json_token *tokens;
    size_t token_count;
    size_t maximum_depth;
} hps_json_document;

#ifdef __cplusplus
extern "C" {
#endif

/* Strict single JSON value followed only by whitespace. Caller supplies exactly
 * token_capacity token objects and frame_capacity scratch frames; zero allows
 * NULL. Capacities above SIZE_MAX/sizeof(element) are INVALID_ARGUMENT. Frames
 * budget simultaneously open arrays/objects (scalar needs zero).
 * Tokens retain ALL lexical tokens, including punctuation, excluding EOF; they
 * borrow input. Object members retain order and duplicate names; no lookup,
 * implicit duplicate resolution, heap graph or numeric conversion is performed.
 * Comments/trailing commas are rejected. Shared tokenizer Unicode rules apply.
 * All arrays, input and metadata must be disjoint/live and input immutable for
 * the call. Token/document outputs are preserved on every failure; frames may
 * change. First pass validates grammar and both budgets before second-pass
 * writes. Invalid arguments first, then first encountered syntax/depth error;
 * token NO_SPACE is reported only after full grammar/depth validation.
 * O(input.length), two scans; caller storage is token_capacity*sizeof(token)
 * plus frame_capacity*sizeof(frame); constant automatic storage, no recursion.
 * Document remains valid only while input/token storage stays unchanged/live.
 * No allocation, blocking, OS or synchronization; caller serializes use. */
int32_t hps_json_read(hps_string_view input,
                        hps_json_token *tokens, size_t token_capacity,
                        hps_json_frame *frames, size_t frame_capacity,
                        hps_json_document *document);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_JSON_HPS_READER_H */
