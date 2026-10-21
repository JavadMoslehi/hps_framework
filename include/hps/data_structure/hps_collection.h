#ifndef HPS_DATA_STRUCTURE_HPS_COLLECTION_H
#define HPS_DATA_STRUCTURE_HPS_COLLECTION_H

/**
 * @file hps/data_structure/hps_collection.h
 * @brief Local typed collection results and shared operation contracts.
 */

#include "hps/base/hps_stdint.h"

#define HPS_COLLECTION_OK ((int32_t)INT32_C(0))
#define HPS_COLLECTION_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_COLLECTION_INVALID_STATE ((int32_t)-INT32_C(2))
#define HPS_COLLECTION_CAPACITY ((int32_t)-INT32_C(3))
#define HPS_COLLECTION_OUT_OF_RANGE ((int32_t)-INT32_C(4))
#define HPS_COLLECTION_SIZE_OVERFLOW ((int32_t)-INT32_C(5))

/* Expand once at file scope after the matching HPS_VECTOR_DEFINE or
 * HPS_DLIST_DEFINE, with the identical type_ and unique hps_-prefixed name_.
 * Use a typedef for pointer elements; elements must be assignable by value.
 * C++ elements must be trivial C-compatible objects. Functions have internal
 * linkage and arguments are evaluated once by ordinary function calls.
 *
 * These operations borrow actual typed caller-owned storage. They do not grow,
 * allocate, retain, release, destruct, or transfer ownership of element values.
 * Copying a value must be permitted by its own ownership contract. Descriptor,
 * node and iterator metadata must be initialized and remain live. The caller
 * proves actual array extents, node validity, interior links and lifetime;
 * metadata checks cannot validate arbitrary pointers or find every corruption.
 * Caller serializes all conflicting accesses. No concurrency/ISR claim is made.
 *
 * Check/operation results use the local HPS_COLLECTION_* domain. NULL arguments
 * precede invalid metadata; valid metadata precedes operation-specific bounds.
 * All failing calls preserve metadata, payload and output values. Each function
 * has O(1) work/automatic storage and uses no payload temporary array. A vector
 * budgets capacity*sizeof(type_) backing storage; a list budgets all caller-owned
 * nodes plus its descriptor. Each iterator requires only the fields declared
 * below, with ordinary target padding. Total iteration is O(captured length).
 */

#endif /* HPS_DATA_STRUCTURE_HPS_COLLECTION_H */
