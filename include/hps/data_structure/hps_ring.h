#ifndef HPS_DATA_STRUCTURE_HPS_RING_H
#define HPS_DATA_STRUCTURE_HPS_RING_H

/**
 * @file hps/data_structure/hps_ring.h
 * @brief Borrowed typed ring descriptors and construction helpers.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_attributes.h"
#include "hps/base/hps_expression.h"

/* Expand DEFINE at file scope with a complete element type and a unique prefix.
 * Use an element typedef for pointer/array declarators; node values must also be
 * assignable by value. C++ consumers use trivial C-compatible element types.
 * Counts and capacities measure elements, never bytes.
 * Constructors are unchecked, evaluate each argument once and copy metadata
 * (plus one value for a node). No evaluation order between arguments is promised.
 * Caller proves extents, initialization, lifetime, bounds and link consistency.
 * No constructor allocates, releases, transfers storage ownership or traverses
 * nodes. Pointer descriptors use caller-selected storage; exclusive ownership,
 * if desired, is an application contract. Views always borrow their payload.
 * Caller serializes conflicting access/release. No ISR/concurrency guarantee.
 * All generated functions have internal linkage. STATIC_INIT requires values
 * allowed in a constant initializer when used for static storage.
 */

/* A ring borrows capacity live elements. head is the next read position and tail
 * the next write position; both are below positive capacity, length <= capacity,
 * and tail is head advanced by length with wrap. A zero-capacity ring is empty
 * with zero head/tail. Only the logical active elements need be initialized.
 * The in-place SRING uses the same index/count rules with its embedded capacity.
 */

#define HPS_RING_DEFINE(type_, name_) \
    typedef struct name_##_s { \
        type_ *data; \
        size_t head; \
        size_t tail; \
        size_t length; \
        size_t capacity; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make( \
        type_ *data_, size_t head_, size_t tail_, size_t length_, size_t capacity_) \
    { \
        name_##_t container_; \
        container_.data = data_; \
        container_.head = head_; \
        container_.tail = tail_; \
        container_.length = length_; \
        container_.capacity = capacity_; \
        return container_; \
    }

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_RING_MAKE(name_, data_, head_, tail_, length_, capacity_) \
    HPS_LVALUE(name_##_t, (data_), (head_), (tail_), (length_), (capacity_))

#else

# define HPS_RING_MAKE(name_, data_, head_, tail_, length_, capacity_) \
    name_##_make((data_), (head_), (tail_), (length_), (capacity_))

#endif

#define HPS_RING_STATIC_INIT(data_, head_, tail_, length_, capacity_) \
    { (data_), (head_), (tail_), (length_), (capacity_) }

#endif /* HPS_DATA_STRUCTURE_HPS_RING_H */
