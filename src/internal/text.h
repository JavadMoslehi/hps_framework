#ifndef HPS_INTERNAL_TEXT_H
#define HPS_INTERNAL_TEXT_H

/**
 * @file src/internal/text.h
 * @brief Shared shape and embedded-NUL validation for bounded text operations.
 */

#include "hps/base/attributes.h"
#include "hps/string/view.h"

static HPS_INLINE int32_t hps_string_check_view(hps_string_view view)
{
    return ((view.length != HPS_STRING_NPOS) &&
            ((view.data != NULL) || (view.length == 0U))) ?
        HPS_STRING_OK : HPS_STRING_INVALID_ARGUMENT;
}

/* Caller first proves the view shape and capacity; no pointer formation here. */
static HPS_INLINE uint8_t hps_string_has_nul(hps_string_view view)
{
    uint8_t found = 0U;
    size_t index = 0U;
    while ((index < view.length) && (found == 0U)) {
        if (view.data[index] == '\0') {
            found = 1U;
        }
        index += 1U;
    }
    return found;
}

#endif /* HPS_INTERNAL_TEXT_H */
