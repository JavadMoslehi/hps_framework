#ifndef HPS_DATA_STRUCTURE_HPS_CSPAN_H
#define HPS_DATA_STRUCTURE_HPS_CSPAN_H

/**
 * @file hps/data_structure/hps_cspan.h
 * @brief Const-span generator using the read-only span contract.
 */

#include "hps/data_structure/hps_span.h"

/* Const spans use the existing read-only span generator and its contract. */
#define HPS_CSPAN_DEFINE(type_, name_) HPS_SPAN_DEFINE(type_, name_)

#define HPS_CSPAN_MAKE(name_, data_, length_) \
    HPS_SPAN_MAKE(name_, data_, length_)

#define HPS_CSPAN_STATIC_INIT(data_, length_) \
    HPS_SPAN_STATIC_INIT(data_, length_)

#endif /* HPS_DATA_STRUCTURE_HPS_CSPAN_H */
