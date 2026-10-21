#ifndef HPS_DATA_STRUCTURE_HPS_MUTABLE_SPAN_H
#define HPS_DATA_STRUCTURE_HPS_MUTABLE_SPAN_H

/**
 * @file hps/data_structure/hps_mutable_span.h
 * @brief Mutable typed spans and construction helpers.
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

/* Mutable spans borrow length initialized elements. NULL requires length zero.
 * This is distinct from HPS_SPAN_DEFINE's read-only element access. */

#define HPS_MUTABLE_SPAN_DEFINE(type_, name_) \
    typedef struct name_##_s { \
        type_ *data; \
        size_t length; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make(type_ *data_, size_t length_) \
    { \
        name_##_t container_; \
        container_.data = data_; \
        container_.length = length_; \
        return container_; \
    }

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_MUTABLE_SPAN_MAKE(name_, data_, length_) \
    HPS_LVALUE(name_##_t, (data_), (length_))

#else

# define HPS_MUTABLE_SPAN_MAKE(name_, data_, length_) \
    name_##_make((data_), (length_))

#endif

#define HPS_MUTABLE_SPAN_STATIC_INIT(data_, length_) \
    { (data_), (length_) }

#endif /* HPS_DATA_STRUCTURE_HPS_MUTABLE_SPAN_H */
