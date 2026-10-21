#ifndef HPS_JSON_HPS_WRITER_H
#define HPS_JSON_HPS_WRITER_H

/**
 * @file hps/json/hps_writer.h
 * @brief Validate and emit compact JSON into fixed caller-owned text storage.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/json/hps_reader.h"
#include "hps/string/hps_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Replace initialized destination with input's JSON lexical spans joined without
 * inter-token whitespace. String contents/escapes, number spellings, member order
 * and duplicate keys are preserved; this is compact output, not canonical JSON.
 * Input is independently validated; no mutable document/token cache is trusted.
 * Same strict grammar/Unicode/depth rules as hps_json_read. Frame storage may
 * change on error. Destination metadata/payload are preserved on every error;
 * capacity includes NUL, no truncation. Input, frames, destination metadata and
 * payload must be mutually disjoint; overlapping input/output is unsupported.
 * Validate destination first, arguments next, grammar/depth next, capacity last.
 * Frame capacity above SIZE_MAX/sizeof(frame) is INVALID_ARGUMENT. Error results
 * use the JSON domain, including INVALID_STATE for broken output.
 * O(input.length + old output.length), two scans; caller frame budget, constant
 * automatic storage. No heap, recursion, callbacks, global state, OS or locks.
 * Build new values with existing string builder/typed cstr formatting; this API
 * deliberately avoids duplicate generic text writers or owning object graphs. */
int32_t hps_json_write_compact(hps_string_buffer *destination,
                                 hps_string_view input,
                                 hps_json_frame *frames, size_t frame_capacity);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_JSON_HPS_WRITER_H */
