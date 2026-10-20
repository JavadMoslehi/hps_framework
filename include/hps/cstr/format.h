#ifndef HPS_CSTR_FORMAT_H
#define HPS_CSTR_FORMAT_H

/**
 * @file hps/cstr/format.h
 * @brief Bounded formatting from typed arguments and exact fixed decimals.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/string/buffer.h"

#define HPS_CSTR_ARG_TEXT ((uint8_t)1U)
#define HPS_CSTR_ARG_I64 ((uint8_t)2U)
#define HPS_CSTR_ARG_U64 ((uint8_t)3U)
#define HPS_CSTR_ARG_BOOL ((uint8_t)4U)
#define HPS_CSTR_ARG_DECIMAL ((uint8_t)5U)
#define HPS_CSTR_DECIMAL_MAX_SCALE ((uint8_t)18U)

/* Initialize every field. The tag selects the field read; no union or hidden
 * native ABI promotion. DECIMAL is signed_value / 10^scale exactly. */
typedef struct {
    uint8_t type;
    hps_string_view text;
    int64_t signed_value;
    uint64_t unsigned_value;
    uint8_t flag;
    uint8_t scale;
} hps_cstr_format_arg;

#ifdef __cplusplus
extern "C" {
#endif

/* Format grammar: %% literal; %s TEXT (no embedded NUL); %c TEXT of length one;
 * %d/%i I64; %u U64; %x/%X U64 hex; %b BOOL(0/1); %f DECIMAL(scale0..18).
 * Optional l/ll before d/i/u/x/X is accepted, with the same exact-width field.
 * There are no arbitrary printf flags, width, precision, %n, native float/double,
 * varargs, locale or stdio dependencies. Every nonliteral token consumes exactly
 * one argument; missing/extra/mistagged arguments => INVALID_ARGUMENT.
 * Capacity includes NUL. Import format within format_capacity; NOT_TERMINATED
 * means no NUL within bound. Destination must be initialized. Invalid state is
 * checked first; format/args then capacity. Scratch must be live writable char
 * storage disjoint from destination, format, argument objects and text payloads.
 * Preflight sizes before writing scratch; error leaves destination, scratch and
 * all outputs unchanged. Source text may borrow destination because staging
 * completes before destination mutation. Metadata objects are disjoint.
 * No allocation/OS/global state; O(format length + emitted length + old length),
 * with each argument bounded by its declared extent, plus fixed numeric storage.
 * Output is complete or fails; no truncation. Callers serialize conflicting use. */
int32_t hps_cstr_format(hps_string_buffer *destination,
                        char *scratch, size_t scratch_capacity,
                        const char *format, size_t format_capacity,
                        const hps_cstr_format_arg *arguments,
                        size_t argument_count);
int32_t hps_cstr_append_format(hps_string_buffer *destination,
                               char *scratch, size_t scratch_capacity,
                               const char *format, size_t format_capacity,
                               const hps_cstr_format_arg *arguments,
                               size_t argument_count);

/* Render an exact fixed decimal, replacing destination. scale0..18; INT64_MIN
 * handled without signed negation overflow. Invalid scale/state/no space preserve
 * destination. No rounding, binary floating interpretation or locale. */
int32_t hps_cstr_render_decimal(hps_string_buffer *destination,
                                int64_t coefficient, uint8_t scale);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_CSTR_FORMAT_H */
