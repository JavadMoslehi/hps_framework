#ifndef HPS_DATA_STRUCTURE_HPS_SRING_H
#define HPS_DATA_STRUCTURE_HPS_SRING_H

/**
 * @file hps/data_structure/hps_sring.h
 * @brief In-place typed ring descriptors with fixed capacity.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_attributes.h"
#include "hps/base/hps_expression.h"
#include "hps/data_structure/hps_static_init.h"

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

#define HPS_SRING_DEFINE(type_, name_, capacity_) \
    typedef size_t name_##_capacity_positive[((capacity_) > 0U) ? 1 : -1]; \
    typedef struct name_##_s { \
        type_ data[(capacity_)]; \
        size_t head; \
        size_t tail; \
        size_t length; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make(void) \
    { \
        name_##_t container_ = HPS_DATA_STRUCTURE_EMPTY_INIT; \
        return container_; \
    }

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_SRING_MAKE(name_) \
    HPS_LVALUE(name_##_t, 0)

#else

# define HPS_SRING_MAKE(name_) \
    name_##_make()

#endif

#define HPS_SRING_STATIC_INIT HPS_DATA_STRUCTURE_EMPTY_INIT

#endif /* HPS_DATA_STRUCTURE_HPS_SRING_H */
