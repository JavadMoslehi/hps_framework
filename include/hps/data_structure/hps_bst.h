#ifndef HPS_DATA_STRUCTURE_HPS_BST_H
#define HPS_DATA_STRUCTURE_HPS_BST_H

/**
 * @file hps/data_structure/hps_bst.h
 * @brief Typed binary-tree nodes and borrowed tree descriptors.
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

#define HPS_BST_DEFINE(type_, name_) \
    typedef struct name_##_node_s { \
        type_ value; \
        struct name_##_node_s *left; \
        struct name_##_node_s *right; \
    } name_##_node_t; \
    typedef struct name_##_s { \
        name_##_node_t *root; \
        size_t length; \
    } name_##_t; \
    static HPS_INLINE name_##_node_t name_##_node_make( \
        type_ value_, name_##_node_t *left_, name_##_node_t *right_) \
    { \
        name_##_node_t node_; \
        node_.value = value_; \
        node_.left = left_; \
        node_.right = right_; \
        return node_; \
    } \
    static HPS_INLINE name_##_t name_##_make(name_##_node_t *root_, size_t length_) \
    { \
        name_##_t container_; \
        container_.root = root_; \
        container_.length = length_; \
        return container_; \
    }

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_BST_MAKE(name_, root_, length_) \
    HPS_LVALUE(name_##_t, (root_), (length_))
# define HPS_BST_NODE_MAKE(name_, value_, left_, right_) \
    HPS_LVALUE(name_##_node_t, (value_), (left_), (right_))

#else

# define HPS_BST_MAKE(name_, root_, length_) \
    name_##_make((root_), (length_))
# define HPS_BST_NODE_MAKE(name_, value_, left_, right_) \
    name_##_node_make((value_), (left_), (right_))

#endif

#define HPS_BST_STATIC_INIT(root_, length_) \
    { (root_), (length_) }

#define HPS_BST_NODE_STATIC_INIT(value_, left_, right_) \
    { (value_), (left_), (right_) }

#endif /* HPS_DATA_STRUCTURE_HPS_BST_H */
