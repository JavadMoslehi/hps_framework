#ifndef HPS_JSON_HPS_TOKENIZER_H
#define HPS_JSON_HPS_TOKENIZER_H

/**
 * @file hps/json/hps_tokenizer.h
 * @brief Strict bounded JSON lexical tokens and counted scalar decoding.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/string/hps_view.h"

#define HPS_JSON_OK ((int32_t)INT32_C(0))
#define HPS_JSON_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_JSON_INVALID_STATE ((int32_t)-INT32_C(2))
#define HPS_JSON_SYNTAX ((int32_t)-INT32_C(3))
#define HPS_JSON_NO_SPACE ((int32_t)-INT32_C(4))
#define HPS_JSON_DEPTH_EXCEEDED ((int32_t)-INT32_C(5))
#define HPS_JSON_OUT_OF_RANGE ((int32_t)-INT32_C(6))

#define HPS_JSON_EOF ((uint8_t)0U)
#define HPS_JSON_OBJECT_BEGIN ((uint8_t)1U)
#define HPS_JSON_OBJECT_END ((uint8_t)2U)
#define HPS_JSON_ARRAY_BEGIN ((uint8_t)3U)
#define HPS_JSON_ARRAY_END ((uint8_t)4U)
#define HPS_JSON_COMMA ((uint8_t)5U)
#define HPS_JSON_COLON ((uint8_t)6U)
#define HPS_JSON_STRING ((uint8_t)7U)
#define HPS_JSON_NUMBER ((uint8_t)8U)
#define HPS_JSON_TRUE ((uint8_t)9U)
#define HPS_JSON_FALSE ((uint8_t)10U)
#define HPS_JSON_NULL ((uint8_t)11U)

typedef struct {
    uint8_t kind;
    hps_string_view raw;
    size_t offset;
    size_t line;
    size_t column;
} hps_json_token;

typedef struct {
    hps_string_view input;
    size_t position;
    size_t line;
    size_t column;
} hps_json_tokenizer;

#ifdef __cplusplus
extern "C" {
#endif

/* Input borrows length readable immutable characters, length < SIZE_MAX; NULL
 * only at zero length. ASCII-compatible execution charset and UTF-8 input are
 * required. Caller owns live input and serializes access. Metadata/payloads and
 * outputs are disjoint. No heap, callbacks, OS, locale, recursion or global state.
 * Token raw spans INCLUDE string quotes and escapes; EOF has {NULL,0}. Positions
 * are byte offsets (zero based), lines/columns one based; LF starts a new line.
 * next skips only JSON space/tab/CR/LF; it checks number grammar, literal
 * boundaries, UTF-8, escape spelling and paired UTF-16 surrogates. BOM, comments,
 * invalid UTF-8 and lone surrogates are rejected (interoperable JSON subset).
 * Document structure is checked by reader.h, not by this lexical API.
 * All failures preserve tokenizer, token and other outputs. Next is O(bytes
 * consumed); initialization O(1); fixed automatic scalar storage only.
 * Return priority: required arguments, state/shape, then lexical syntax. */
int32_t hps_json_tokenizer_init(hps_json_tokenizer *tokenizer,
                                 hps_string_view input);
int32_t hps_json_tokenizer_next(hps_json_tokenizer *tokenizer,
                                 hps_json_token *token);

/* Revalidate the entire raw token; kind must match. Decode JSON string to UTF-8
 * bytes, including embedded U+0000; NO terminator is written. NULL output allowed
 * only at zero capacity. Input/output metadata and payloads must be disjoint.
 * Preflight validates before capacity, then writes; all failures preserve output
 * bytes/count. O(raw.length), three scans, constant automatic storage. */
int32_t hps_json_token_decode_string(const hps_json_token *token,
                                      uint8_t *output, size_t capacity,
                                      size_t *written);

/* Entire NUMBER token must be an integer spelling; decimal/exponent spellings
 * return SYNTAX, out-of-int64 range returns OUT_OF_RANGE. No floating conversion.
 * Preserves result on failure; O(raw.length), fixed scalar storage. */
int32_t hps_json_token_i64(const hps_json_token *token, int64_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_JSON_HPS_TOKENIZER_H */
