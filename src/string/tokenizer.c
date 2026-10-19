#include "hps/string/tokenizer.h"

#include "../internal/text.h"

/**
 * @file src/string/tokenizer.c
 * @brief Checked single-pass scanning of borrowed quoted/escaped token windows.
 */

/* Enabled syntax must survive delimiter skipping as well as token scanning:
 * discarding an opening quote/escape would change the following token bounds. */
static uint8_t tokenizer_is_separator(const uint8_t value,
                                     const hps_string_view delimiters,
                                     const uint8_t flags)
{
    size_t index = 0U;
    uint8_t found = 0U;
    if (((flags & HPS_STRING_TOKEN_ESCAPES) != 0U) &&
        (value == (uint8_t)'\\')) {
        /* An enabled escape begins token content even when in the set. */
    } else if (((flags & HPS_STRING_TOKEN_QUOTES) != 0U) &&
               ((value == (uint8_t)'\'') || (value == (uint8_t)'"'))) {
        /* An enabled quote begins token content even when in the set. */
    } else {
        while ((index < delimiters.length) && (found == 0U)) {
            if ((uint8_t)delimiters.data[index] == value) { found = 1U; }
            index += 1U;
        }
    }
    return found;
}

int32_t hps_string_tokenizer_next(const hps_string_view input,
                                   const hps_string_view delimiters,
                                   size_t *cursor, const uint8_t flags,
                                   hps_string_view *part, uint8_t *found)
{
    int32_t status = HPS_STRING_INVALID_ARGUMENT;
    if ((cursor != NULL) && (part != NULL) && (found != NULL) && (flags <= 7U)) {
        status = hps_string_check_view(input);
        if (status == HPS_STRING_OK) { status = hps_string_check_view(delimiters); }
        if (status == HPS_STRING_OK) {
            if ((*cursor > input.length) && (*cursor != HPS_STRING_NPOS)) {
                status = HPS_STRING_OUT_OF_RANGE;
            } else if (*cursor == HPS_STRING_NPOS) {
                *found = 0U;
            } else {
                size_t index = *cursor;
                while ((index < input.length) &&
                       (tokenizer_is_separator((uint8_t)input.data[index], delimiters, flags) == 1U)) {
                    index += 1U;
                }
                if (index == input.length) {
                    *cursor = HPS_STRING_NPOS;
                    *found = 0U;
                } else {
                    const size_t start = index;
                    uint8_t quote = 0U;
                    uint8_t complete = 0U;
                    while ((index < input.length) && (complete == 0U) &&
                           (status == HPS_STRING_OK)) {
                        const uint8_t value = (uint8_t)input.data[index];
                        if (((flags & HPS_STRING_TOKEN_ESCAPES) != 0U) && (value == (uint8_t)'\\')) {
                            /* The first subtraction is valid because index <
                             * length; prove a second character before +2. */
                            if ((input.length - index) < 2U) { status = HPS_STRING_INVALID_ARGUMENT; }
                            else { index += 2U; }
                        } else if (((flags & HPS_STRING_TOKEN_QUOTES) != 0U) &&
                                   ((value == (uint8_t)'\'') || (value == (uint8_t)'"'))) {
                            if (quote == 0U) { quote = value; }
                            else if (quote == value) { quote = 0U; }
                            else { /* The other quote is literal within a pair. */ }
                            index += 1U;
                        } else if ((quote == 0U) && (tokenizer_is_separator(value, delimiters, flags) == 1U)) {
                            complete = 1U;
                        } else { index += 1U; }
                    }
                    if ((status == HPS_STRING_OK) && (quote != 0U)) {
                        status = HPS_STRING_INVALID_ARGUMENT;
                    }
                    if (status == HPS_STRING_OK) {
                        hps_string_view token = {NULL, 0U};
                        status = hps_string_view_slice(input, start, index - start, &token);
                        if ((status == HPS_STRING_OK) && ((flags & HPS_STRING_TOKEN_TRIM) != 0U)) {
                            status = hps_string_view_trim_whitespace(token, &token);
                        }
                        while ((index < input.length) &&
                               (tokenizer_is_separator((uint8_t)input.data[index], delimiters, flags) == 1U)) {
                            index += 1U;
                        }
                        if (status == HPS_STRING_OK) {
                            *cursor = (index == input.length) ? HPS_STRING_NPOS : index;
                            *part = token;
                            *found = 1U;
                        }
                    }
                }
            }
        }
    }
    return status;
}
