#ifndef HPS_MEMORY_MEMORY_H
#define HPS_MEMORY_MEMORY_H

/**
 * @file hps/memory/memory.h
 * @brief Public umbrella for HPS memory APIs.
 */

/* Includes the optional ownership/allocator development proposals. Including
 * declarations selects no provider and performs no allocation. See the Memory
 * contract before using those APIs; HPS-MEM-001 disposition remains pending. */
#include "hps/memory/hps_allocated_array.h"
#include "hps/memory/hps_allocator.h"
#include "hps/memory/hps_array.h"
#include "hps/memory/hps_clone.h"
#include "hps/memory/hps_operations.h"
#include "hps/memory/hps_owner.h"
#include "hps/memory/hps_pool.h"
#include "hps/memory/hps_refcount.h"

#endif /* HPS_MEMORY_MEMORY_H */
