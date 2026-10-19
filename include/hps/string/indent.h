#ifndef HPS_STRING_INDENT_H
#define HPS_STRING_INDENT_H

/**
 * @file hps/string/indent.h
 * @brief Persistent four-space indentation over a caller-owned string builder.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/string/builder.h"

#define HPS_STRING_INDENT_WIDTH ((size_t)4U)

typedef struct {
    hps_string_builder *builder;
    size_t level;
    uint8_t at_line_start;
} hps_string_indent;

#ifdef __cplusplus
extern "C" {
#endif

/* Bind one live initialized builder; this context borrows its descriptor and
 * storage. Do not mutate either outside this context while bound. Context,
 * builder and output metadata are disjoint from one another and all payloads.
 * Caller serializes access and proves lifetimes/extents. No allocation/growth,
 * ownership transfer, I/O, mutable globals or concurrency guarantee.
 * init leaves text unchanged, starts at level zero and infers line start from
 * empty text or a final LF. clear empties the builder and resets both fields.
 * set_level accepts at most SIZE_MAX / WIDTH, otherwise OUT_OF_RANGE.
 * All calls return the HPS_STRING_* local error domain. NULL context or an init
 * with NULL builder is INVALID_ARGUMENT; a malformed bound context or builder
 * is INVALID_STATE. Every
 * failure preserves context, builder text/descriptor, outputs AND scratch.
 * Work includes the existing O(builder.length) invariant validation; fixed
 * automatic metadata only. No target stack or timing guarantee is inferred. */
int32_t hps_string_indent_init(hps_string_indent *context,
                                hps_string_builder *builder);
int32_t hps_string_indent_clear(hps_string_indent *context);
int32_t hps_string_indent_set_level(hps_string_indent *context, size_t level);

/* append_indent emits WIDTH*level spaces explicitly without changing line-start
 * state, matching the old manual indent helper. append_newline emits one LF and
 * marks line start. Both use existing builder operations and require no scratch.
 * A successful explicit indent followed by append_view at line start therefore
 * emits indentation twice, as in the legacy builder. */
int32_t hps_string_indent_append_indent(hps_string_indent *context);
int32_t hps_string_indent_append_newline(hps_string_indent *context);

/* append_view emits indentation once if at_line_start, then the entire source;
 * even empty source changes that flag to zero after any required indentation.
 * append_line additionally emits LF and sets the flag to one. Source contents
 * are not parsed for embedded LF, preserving the legacy explicit-newline model.
 * Source is a valid counted view without NUL and may borrow the current builder.
 * Scratch is a separate live writable array, disjoint from both descriptors,
 * context, builder storage and source. It must hold the COMPLETE resulting text
 * including NUL. Capacity and source NUL checks precede all writes; insufficient
 * builder/scratch space gives NO_SPACE, with no partial indentation or line.
 * Source shape and scratch NULL are INVALID_ARGUMENT. Work is O(old length +
 * indentation + source.length), bounded by builder and scratch capacities. */
int32_t hps_string_indent_append_view(hps_string_indent *context,
                                       hps_string_view source,
                                       char *scratch, size_t scratch_capacity);
int32_t hps_string_indent_append_line(hps_string_indent *context,
                                       hps_string_view source,
                                       char *scratch, size_t scratch_capacity);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STRING_INDENT_H */
