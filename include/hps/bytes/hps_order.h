
#ifndef HPS_BYTES_HPS_ORDER_H
#define HPS_BYTES_HPS_ORDER_H

/**
 * @file hps/bytes/hps_order.h
 * @brief Exact-width byte-order selector for integer codecs.
 */

#include "hps/base/hps_stdint.h"

/* Only these two selector values are supported by the codec API. Base's
 * HPS_ENDIAN_* target-detection identifiers have a different numeric domain;
 * do not pass them as codec order selectors. */
typedef uint8_t hps_byte_order;
#define HPS_BYTE_ORDER_LITTLE_ENDIAN ((hps_byte_order)UINT8_C(0))
#define HPS_BYTE_ORDER_BIG_ENDIAN ((hps_byte_order)UINT8_C(1))

#endif /* HPS_BYTES_HPS_ORDER_H */
