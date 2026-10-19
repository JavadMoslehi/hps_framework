#ifndef HPS_JSON_INTERNAL_H
#define HPS_JSON_INTERNAL_H

/**
 * @file src/json/internal.h
 * @brief Shared bounded JSON scanning and grammar preflight contracts.
 */

#include "hps/json/reader.h"

int32_t hps_json_scan_string(hps_string_view input, size_t start,
                                size_t *end, uint8_t *output, size_t *written);
int32_t hps_json_scan_document(hps_string_view input,
                                  hps_json_frame *frames, size_t frame_capacity,
                                  hps_json_token *tokens, size_t *count,
                                  size_t *maximum_depth, size_t *compact_length);

#endif /* HPS_JSON_INTERNAL_H */
