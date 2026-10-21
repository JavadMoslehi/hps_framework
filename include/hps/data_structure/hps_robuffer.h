#ifndef HPS_DATA_STRUCTURE_HPS_ROBUFFER_H
#define HPS_DATA_STRUCTURE_HPS_ROBUFFER_H

/**
 * @file hps/data_structure/hps_robuffer.h
 * @brief Read-only typed buffer descriptors and construction helpers.
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

/* Read-only window: the same extent rules as vector, with const element access.
 * const access does not prevent mutation through another alias. */

#define HPS_ROBUFFER_DEFINE(type_, name_) \
    typedef struct name_##_s { \
        const type_ *data; \
        size_t length; \
        size_t capacity; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make( \
        const type_ *data_, size_t length_, size_t capacity_) \
    { \
        name_##_t container_; \
        container_.data = data_; \
        container_.length = length_; \
        container_.capacity = capacity_; \
        return container_; \
    }

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_ROBUFFER_MAKE(name_, data_, length_, capacity_) \
    HPS_LVALUE(name_##_t, (data_), (length_), (capacity_))

#else

# define HPS_ROBUFFER_MAKE(name_, data_, length_, capacity_) \
    name_##_make((data_), (length_), (capacity_))

#endif

#define HPS_ROBUFFER_STATIC_INIT(data_, length_, capacity_) \
    { (data_), (length_), (capacity_) }

#endif /* HPS_DATA_STRUCTURE_HPS_ROBUFFER_H */
