#ifndef HPS_HPS_ALLOCATOR_H
#define HPS_HPS_ALLOCATOR_H

/**
 * @file hps/hps_allocator.h
 * @brief Compatibility include for the optional allocator development proposal.
 */

/* Prefer hps/memory/allocator.h. This forwards the include path, not the draft's
 * conflicting malloc-family names, callback member names or numerical ABI.
 * The exact original draft is retained in the migration source archive. */
#include "hps/memory/allocator.h"

#endif /* HPS_HPS_ALLOCATOR_H */
