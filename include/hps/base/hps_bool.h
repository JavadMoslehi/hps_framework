#ifndef HPS_BASE_HPS_BOOL_H
#define HPS_BASE_HPS_BOOL_H

/**
 * @file hps/base/hps_bool.h
 * @brief Standard bool spelling and bounded Boolean conversion helpers.
 */

#include "hps/base/hps_stdint.h"
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_attributes.h"

#if defined(__cplusplus)
/* C++ supplies bool, true and false as language keywords. */
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
/* C23 supplies bool, true and false as language keywords. */
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
/* C99 through C17 and draft C2x libraries supply their appropriate spelling. */
# include <stdbool.h>
#else
/* C89 compatibility only: this macro uses standard uint8_t storage. Unlike a
 * native Boolean type, assignment does not normalize arbitrary nonzero values.
 * Store true/false or use the conversion helpers to obtain normalized values.
 * This representation does not establish ABI compatibility with native bool.
 */
# ifndef bool
#  define bool uint8_t
# endif
# ifndef true
#  define true 1
# endif
# ifndef false
#  define false 0
# endif
# ifndef __bool_true_false_are_defined
#  define __bool_true_false_are_defined 1
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Every helper performs fixed work with no allocation, blocking or shared
 * mutable state. Scalar inputs are unrestricted; nonzero Boolean storage is
 * interpreted as true, including the C89 compatibility representation.
 */

/** Return a borrowed immutable, NUL-terminated literal with static lifetime.
 * The result is "true" (five bytes including NUL) or "false" (six bytes).
 */
static HPS_INLINE const char *hps_bool_to_cstr(bool value)
{
    return value ? "true" : "false";
}

/** Return the execution-character-set code for '1' or '0' in uint8_t storage.
 * Numeric character APIs use uint8_t; plain char remains for C-string pointers.
 */
static HPS_INLINE uint8_t hps_bool_to_char(bool value)
{
    return value ? (uint8_t)'1' : (uint8_t)'0';
}

/** Return exactly one for true and zero for false. */
static HPS_INLINE uint8_t hps_bool_to_u8(bool value)
{
    return value ? (uint8_t)1 : (uint8_t)0;
}

/** Return exactly one for true and zero for false. */
static HPS_INLINE int32_t hps_bool_to_i32(bool value)
{
    return value ? INT32_C(1) : INT32_C(0);
}

/** Normalize zero to false and every nonzero value to true. */
static HPS_INLINE bool hps_bool_from_u8(uint8_t value)
{
    return value != (uint8_t)0;
}

/** Normalize zero to false and every nonzero value, including negatives, to true. */
static HPS_INLINE bool hps_bool_from_i32(int32_t value)
{
    return value != INT32_C(0);
}

/** Accept exactly the execution-character-set codes for '0' and '1'. */
static HPS_INLINE bool hps_bool_is_valid_char(uint8_t value)
{
    return (value == (uint8_t)'0') || (value == (uint8_t)'1');
}

/** Return true for '1'; return false for every other code, including invalid input.
 * Use hps_bool_from_char_checked when invalid input must be distinguished.
 */
static HPS_INLINE bool hps_bool_from_char(uint8_t value)
{
    return value == (uint8_t)'1';
}

/** Validate a character and write its normalized Boolean value.
 * output_value is NULL or points to one writable bool object owned by the caller;
 * its previous value is not read. No pointer or ownership is retained.
 * Return false for NULL without a write. For invalid input, write false and
 * return false, preserving the supplied legacy helper's clearing behavior.
 * For '0'/'1', write false/true respectively and return true. Concurrent access
 * to the output object requires the caller's execution-context coordination.
 */
static HPS_INLINE bool hps_bool_from_char_checked(uint8_t value, bool *output_value)
{
    bool valid = false;

    if (output_value != NULL) {
        valid = hps_bool_is_valid_char(value);
        *output_value = valid ? hps_bool_from_char(value) : false;
    }

    return valid;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_BASE_HPS_BOOL_H */
