#ifndef HPS_STRING_TOKENIZER_H
#define HPS_STRING_TOKENIZER_H

/**
 * @file hps/string/tokenizer.h
 * @brief Borrowed delimiter-set tokens with optional quotes, escapes and trim.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/string/view.h"

#define HPS_STRING_TOKEN_TRIM ((uint8_t)UINT8_C(1))
#define HPS_STRING_TOKEN_QUOTES ((uint8_t)UINT8_C(2))
#define HPS_STRING_TOKEN_ESCAPES ((uint8_t)UINT8_C(4))

#ifdef __cplusplus
extern "C" {
#endif

/* Input and delimiters follow the counted-view extent/lifetime contract. A
 * delimiter is any member of the set, not a substring. Runs are skipped, so no
 * empty fields are emitted. An empty set selects the entire remaining input.
 * cursor starts at zero; successful exhaustion sets it to NPOS and found to 0,
 * leaving part unchanged. Otherwise found is 1 and part borrows the next token.
 * Metadata outputs are writable, mutually disjoint and disjoint from payloads.
 * Caller owns live storage and serializes mutation/release and cursor access.
 *
 * flags combines only the three constants above. QUOTES shields delimiters
 * inside matching single/double quotes. ESCAPES shields the next character,
 * inside or outside quotes. Enabled quote/escape syntax takes precedence over
 * delimiter-set membership everywhere, including leading/trailing delimiter
 * skipping. With that flag disabled the same characters are ordinary set
 * members. Quotes/backslashes remain literal token content;
 * no decoding or unquoting occurs. TRIM removes the six basic whitespace
 * characters from the token edges after scanning. A whitespace-only token can
 * therefore produce an empty normalized view with found == 1.
 * Unmatched quotes and a trailing escape return INVALID_ARGUMENT, correcting
 * the legacy scanner's silent malformed input and out-of-bounds escape step.
 * Unknown flags, malformed views or NULL outputs are INVALID_ARGUMENT; cursor
 * above input.length except NPOS is OUT_OF_RANGE. All failures preserve cursor,
 * part and found. Empty tokens normalize to {NULL,0}; embedded NUL is counted
 * data. Work O(input.length * (delimiters.length + 1)) per call, fixed automatic
 * metadata, no allocation, I/O, locale, blocking or synchronization. */
int32_t hps_string_tokenizer_next(hps_string_view input,
                                   hps_string_view delimiters, size_t *cursor,
                                   uint8_t flags, hps_string_view *part,
                                   uint8_t *found);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STRING_TOKENIZER_H */
