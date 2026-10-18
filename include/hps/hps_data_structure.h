#ifndef HPS_HPS_DATA_STRUCTURE_H
#define HPS_HPS_DATA_STRUCTURE_H

/**
 * @file hps/hps_data_structure.h
 * @brief Header-only typed container descriptors and explicit constructors.
 */

#include <stddef.h>
#include "hps/base/attributes.h"
#include "hps/base/expression.h"
#include "hps/span.h"

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

/* Mutable contiguous descriptors require length <= capacity. A NULL data pointer
 * requires zero capacity; a nonempty descriptor borrows initialized elements.
 * Storage covers capacity live objects. Construction does not resize or copy it.
 */
#define HPS_VECTOR_DEFINE(type_, name_) \
    typedef struct name_##_s { \
        type_ *data; \
        size_t length; \
        size_t capacity; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make( \
        type_ *data_, size_t length_, size_t capacity_) \
    { \
        name_##_t container_; \
        container_.data = data_; \
        container_.length = length_; \
        container_.capacity = capacity_; \
        return container_; \
    }

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

/* Const spans use the existing read-only span generator and its contract. */
#define HPS_CSPAN_DEFINE(type_, name_) HPS_SPAN_DEFINE(type_, name_)

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

/* In-place containers own their embedded array. capacity_ must be a positive
 * integer constant expression. Empty construction initializes every element to
 * its C zero-initialized value (including proper null pointers), then zeroes all
 * counters. It performs bounded O(capacity_) initialization/copying, not O(1).
 * Returning/copying an object copies its embedded array and invalidates no source
 * object, but pointers into one copy do not retarget themselves to another copy.
 */
#ifdef __cplusplus
# define HPS_DATA_STRUCTURE_EMPTY_INIT {}
#else
# define HPS_DATA_STRUCTURE_EMPTY_INIT {0}
#endif

#define HPS_SVECTOR_DEFINE(type_, name_, capacity_) \
    typedef size_t name_##_capacity_positive[((capacity_) > 0U) ? 1 : -1]; \
    typedef struct name_##_s { \
        type_ data[(capacity_)]; \
        size_t length; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make(void) \
    { \
        name_##_t container_ = HPS_DATA_STRUCTURE_EMPTY_INIT; \
        return container_; \
    }

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

/* Stack uses the same extent rules as vector; the logical top is length - 1
 * only when length is positive. No push/pop or allocation operation is added. */
#define HPS_STACK_DEFINE(type_, name_) \
    typedef struct name_##_s { \
        type_ *data; \
        size_t length; \
        size_t capacity; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make( \
        type_ *data_, size_t length_, size_t capacity_) \
    { \
        name_##_t container_; \
        container_.data = data_; \
        container_.length = length_; \
        container_.capacity = capacity_; \
        return container_; \
    }

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

#define HPS_DLIST_DEFINE(type_, name_) \
    typedef struct name_##_node_s { \
        type_ value; \
        struct name_##_node_s *prev; \
        struct name_##_node_s *next; \
    } name_##_node_t; \
    typedef struct name_##_s { \
        name_##_node_t *head; \
        name_##_node_t *tail; \
        size_t length; \
    } name_##_t; \
    static HPS_INLINE name_##_node_t name_##_node_make( \
        type_ value_, name_##_node_t *prev_, name_##_node_t *next_) \
    { \
        name_##_node_t node_; \
        node_.value = value_; \
        node_.prev = prev_; \
        node_.next = next_; \
        return node_; \
    } \
    static HPS_INLINE name_##_t name_##_make( \
        name_##_node_t *head_, name_##_node_t *tail_, size_t length_) \
    { \
        name_##_t container_; \
        container_.head = head_; \
        container_.tail = tail_; \
        container_.length = length_; \
        return container_; \
    }

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
# define HPS_VECTOR_MAKE(name_, data_, length_, capacity_) \
    HPS_LVALUE(name_##_t, (data_), (length_), (capacity_))
# define HPS_MUTABLE_SPAN_MAKE(name_, data_, length_) \
    HPS_LVALUE(name_##_t, (data_), (length_))
# define HPS_ROBUFFER_MAKE(name_, data_, length_, capacity_) \
    HPS_LVALUE(name_##_t, (data_), (length_), (capacity_))
# define HPS_SVECTOR_MAKE(name_) \
    HPS_LVALUE(name_##_t, 0)
# define HPS_SRING_MAKE(name_) \
    HPS_LVALUE(name_##_t, 0)
# define HPS_RING_MAKE(name_, data_, head_, tail_, length_, capacity_) \
    HPS_LVALUE(name_##_t, (data_), (head_), (tail_), (length_), (capacity_))
# define HPS_STACK_MAKE(name_, data_, length_, capacity_) \
    HPS_LVALUE(name_##_t, (data_), (length_), (capacity_))
# define HPS_SLIST_MAKE(name_, head_, length_) \
    HPS_LVALUE(name_##_t, (head_), (length_))
# define HPS_DLIST_MAKE(name_, head_, tail_, length_) \
    HPS_LVALUE(name_##_t, (head_), (tail_), (length_))
# define HPS_BST_MAKE(name_, root_, length_) \
    HPS_LVALUE(name_##_t, (root_), (length_))
# define HPS_SLIST_NODE_MAKE(name_, value_, next_) \
    HPS_LVALUE(name_##_node_t, (value_), (next_))
# define HPS_DLIST_NODE_MAKE(name_, value_, prev_, next_) \
    HPS_LVALUE(name_##_node_t, (value_), (prev_), (next_))
# define HPS_BST_NODE_MAKE(name_, value_, left_, right_) \
    HPS_LVALUE(name_##_node_t, (value_), (left_), (right_))
#else
# define HPS_VECTOR_MAKE(name_, data_, length_, capacity_) \
    name_##_make((data_), (length_), (capacity_))
# define HPS_MUTABLE_SPAN_MAKE(name_, data_, length_) \
    name_##_make((data_), (length_))
# define HPS_ROBUFFER_MAKE(name_, data_, length_, capacity_) \
    name_##_make((data_), (length_), (capacity_))
# define HPS_SVECTOR_MAKE(name_) \
    name_##_make()
# define HPS_SRING_MAKE(name_) \
    name_##_make()
# define HPS_RING_MAKE(name_, data_, head_, tail_, length_, capacity_) \
    name_##_make((data_), (head_), (tail_), (length_), (capacity_))
# define HPS_STACK_MAKE(name_, data_, length_, capacity_) \
    name_##_make((data_), (length_), (capacity_))
# define HPS_SLIST_MAKE(name_, head_, length_) \
    name_##_make((head_), (length_))
# define HPS_DLIST_MAKE(name_, head_, tail_, length_) \
    name_##_make((head_), (tail_), (length_))
# define HPS_BST_MAKE(name_, root_, length_) \
    name_##_make((root_), (length_))
# define HPS_SLIST_NODE_MAKE(name_, value_, next_) \
    name_##_node_make((value_), (next_))
# define HPS_DLIST_NODE_MAKE(name_, value_, prev_, next_) \
    name_##_node_make((value_), (prev_), (next_))
# define HPS_BST_NODE_MAKE(name_, value_, left_, right_) \
    name_##_node_make((value_), (left_), (right_))
#endif

#define HPS_CSPAN_MAKE(name_, data_, length_) \
    HPS_SPAN_MAKE(name_, data_, length_)

#define HPS_VECTOR_STATIC_INIT(data_, length_, capacity_) \
    { (data_), (length_), (capacity_) }
#define HPS_MUTABLE_SPAN_STATIC_INIT(data_, length_) \
    { (data_), (length_) }
#define HPS_CSPAN_STATIC_INIT(data_, length_) \
    HPS_SPAN_STATIC_INIT(data_, length_)
#define HPS_ROBUFFER_STATIC_INIT(data_, length_, capacity_) \
    { (data_), (length_), (capacity_) }
#define HPS_SVECTOR_STATIC_INIT HPS_DATA_STRUCTURE_EMPTY_INIT
#define HPS_SRING_STATIC_INIT HPS_DATA_STRUCTURE_EMPTY_INIT
#define HPS_RING_STATIC_INIT(data_, head_, tail_, length_, capacity_) \
    { (data_), (head_), (tail_), (length_), (capacity_) }
#define HPS_STACK_STATIC_INIT(data_, length_, capacity_) \
    { (data_), (length_), (capacity_) }
#define HPS_SLIST_STATIC_INIT(head_, length_) \
    { (head_), (length_) }
#define HPS_DLIST_STATIC_INIT(head_, tail_, length_) \
    { (head_), (tail_), (length_) }
#define HPS_BST_STATIC_INIT(root_, length_) \
    { (root_), (length_) }
#define HPS_SLIST_NODE_STATIC_INIT(value_, next_) \
    { (value_), (next_) }
#define HPS_DLIST_NODE_STATIC_INIT(value_, prev_, next_) \
    { (value_), (prev_), (next_) }
#define HPS_BST_NODE_STATIC_INIT(value_, left_, right_) \
    { (value_), (left_), (right_) }

#endif /* HPS_HPS_DATA_STRUCTURE_H */
