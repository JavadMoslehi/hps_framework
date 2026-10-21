#ifndef HPS_DATA_STRUCTURE_HPS_SLIST_H
#define HPS_DATA_STRUCTURE_HPS_SLIST_H

/**
 * @file hps/data_structure/hps_slist.h
 * @brief Typed singly linked nodes and borrowed list descriptors.
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

/* Linked descriptors borrow nodes with caller-managed storage and ownership.
 * Empty containers have NULL endpoints/root and length zero. Nonempty lists must
 * be acyclic and contain exactly length nodes; dlist endpoints and reciprocal
 * links must agree. A tree has exactly length distinct reachable nodes, no cycles
 * or shared children. Ordering/comparison is an application contract. Constructors
 * store pointers only and cannot establish these invariants or keep nodes alive.
 * Node makers copy value_ by value; embedded pointers keep their own lifetimes.
 */

#define HPS_SLIST_DEFINE(type_, name_) \
    typedef struct name_##_node_s { \
        type_ value; \
        struct name_##_node_s *next; \
    } name_##_node_t; \
    typedef struct name_##_s { \
        name_##_node_t *head; \
        size_t length; \
    } name_##_t; \
    static HPS_INLINE name_##_node_t name_##_node_make( \
        type_ value_, name_##_node_t *next_) \
    { \
        name_##_node_t node_; \
        node_.value = value_; \
        node_.next = next_; \
        return node_; \
    } \
    static HPS_INLINE name_##_t name_##_make(name_##_node_t *head_, size_t length_) \
    { \
        name_##_t container_; \
        container_.head = head_; \
        container_.length = length_; \
        return container_; \
    }

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_SLIST_MAKE(name_, head_, length_) \
    HPS_LVALUE(name_##_t, (head_), (length_))
# define HPS_SLIST_NODE_MAKE(name_, value_, next_) \
    HPS_LVALUE(name_##_node_t, (value_), (next_))

#else

# define HPS_SLIST_MAKE(name_, head_, length_) \
    name_##_make((head_), (length_))
# define HPS_SLIST_NODE_MAKE(name_, value_, next_) \
    name_##_node_make((value_), (next_))

#endif

#define HPS_SLIST_STATIC_INIT(head_, length_) \
    { (head_), (length_) }

#define HPS_SLIST_NODE_STATIC_INIT(value_, next_) \
    { (value_), (next_) }

#endif /* HPS_DATA_STRUCTURE_HPS_SLIST_H */
