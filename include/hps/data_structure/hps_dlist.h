#ifndef HPS_DATA_STRUCTURE_HPS_DLIST_H
#define HPS_DATA_STRUCTURE_HPS_DLIST_H

/**
 * @file hps/data_structure/hps_dlist.h
 * @brief Typed doubly linked nodes, bounded list operations and iterators.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/hps_attributes.h"
#include "hps/base/hps_expression.h"
#include "hps/data_structure/hps_collection.h"

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

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_DLIST_MAKE(name_, head_, tail_, length_) \
    HPS_LVALUE(name_##_t, (head_), (tail_), (length_))
# define HPS_DLIST_NODE_MAKE(name_, value_, prev_, next_) \
    HPS_LVALUE(name_##_node_t, (value_), (prev_), (next_))

#else

# define HPS_DLIST_MAKE(name_, head_, tail_, length_) \
    name_##_make((head_), (tail_), (length_))
# define HPS_DLIST_NODE_MAKE(name_, value_, prev_, next_) \
    name_##_node_make((value_), (prev_), (next_))

#endif

#define HPS_DLIST_STATIC_INIT(head_, tail_, length_) \
    { (head_), (tail_), (length_) }

#define HPS_DLIST_NODE_STATIC_INIT(value_, prev_, next_) \
    { (value_), (prev_), (next_) }

/* Adds name_check, name_push_back and name_iterator_begin/next.
 * The caller proves the list is acyclic, has exactly length distinct live nodes
 * and has reciprocal interior links. check examines only length/endpoints and
 * their exterior links, keeping append O(1). A positive length requires live
 * endpoints; all nodes and metadata are disjoint.
 *
 * push_back requires an exclusively owned detached initialized node with NULL
 * prev/next, not a member of this or another list. It rejects either endpoint as
 * the candidate, preventing a singleton self-cycle. Membership/interior validity
 * remains a caller proof; no search or node allocation occurs. It preserves the
 * candidate value and links it after the tail, then increments checked length.
 * SIZE_MAX length returns SIZE_OVERFLOW after descriptor/candidate validation.
 *
 * begin requires maximum_nodes: the caller's traversal budget. length exceeding
 * this bound returns CAPACITY and preserves the iterator. Zero budget permits
 * empty lists. The initialized iterator captures length, head and a NULL previous
 * node; lifetime/mutation/output rules match vector iteration. next validates
 * reciprocal predecessor and final-node termination while advancing at most one
 * node. It never follows more than the captured length. Invalid live-node links
 * return INVALID_STATE with iterator/outputs preserved; there is no recovery or
 * arbitrary-memory validation guarantee. At end found=0 and value is preserved.
 */
#define HPS_DLIST_OPERATIONS_DEFINE(type_, name_) \
    typedef struct name_##_iterator_s { \
        const name_##_node_t *node; \
        const name_##_node_t *previous; \
        size_t remaining; \
    } name_##_iterator_t; \
    static HPS_INLINE int32_t name_##_check(const name_##_t *container_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if (container_ != NULL) { \
            status_ = HPS_COLLECTION_INVALID_STATE; \
            if (container_->length == 0U) { \
                if ((container_->head == NULL) && (container_->tail == NULL)) { \
                    status_ = HPS_COLLECTION_OK; \
                } \
            } else if ((container_->head != NULL) && \
                       (container_->tail != NULL)) { \
                if ((container_->head->prev == NULL) && \
                    (container_->tail->next == NULL)) { \
                    if (((container_->length == 1U) && \
                         (container_->head == container_->tail)) || \
                        ((container_->length > 1U) && \
                         (container_->head != container_->tail))) { \
                        status_ = HPS_COLLECTION_OK; \
                    } \
                } \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_push_back( \
        name_##_t *container_, name_##_node_t *node_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if (node_ != NULL) { \
            status_ = name_##_check(container_); \
            if (status_ == HPS_COLLECTION_OK) { \
                if ((node_ == container_->head) || \
                    (node_ == container_->tail) || \
                    (node_->prev != NULL) || (node_->next != NULL)) { \
                    status_ = HPS_COLLECTION_INVALID_STATE; \
                } else if (container_->length == SIZE_MAX) { \
                    status_ = HPS_COLLECTION_SIZE_OVERFLOW; \
                } else { \
                    node_->prev = container_->tail; \
                    if (container_->tail != NULL) { \
                        container_->tail->next = node_; \
                    } else { \
                        container_->head = node_; \
                    } \
                    container_->tail = node_; \
                    container_->length += 1U; \
                } \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_iterator_begin( \
        const name_##_t *container_, size_t maximum_nodes_, \
        name_##_iterator_t *iterator_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if (iterator_ != NULL) { \
            status_ = name_##_check(container_); \
            if (status_ == HPS_COLLECTION_OK) { \
                if (container_->length > maximum_nodes_) { \
                    status_ = HPS_COLLECTION_CAPACITY; \
                } else { \
                    iterator_->node = container_->head; \
                    iterator_->previous = NULL; \
                    iterator_->remaining = container_->length; \
                } \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_iterator_next( \
        name_##_iterator_t *iterator_, type_ *value_, uint8_t *found_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if ((iterator_ != NULL) && (value_ != NULL) && (found_ != NULL)) { \
            status_ = HPS_COLLECTION_INVALID_STATE; \
            if (iterator_->remaining == 0U) { \
                if (iterator_->node == NULL) { \
                    *found_ = UINT8_C(0); \
                    status_ = HPS_COLLECTION_OK; \
                } \
            } else if (iterator_->node != NULL) { \
                if (iterator_->node->prev == iterator_->previous) { \
                    if (((iterator_->remaining == 1U) && \
                         (iterator_->node->next == NULL)) || \
                        ((iterator_->remaining > 1U) && \
                         (iterator_->node->next != NULL))) { \
                        *value_ = iterator_->node->value; \
                        iterator_->previous = iterator_->node; \
                        iterator_->node = iterator_->node->next; \
                        iterator_->remaining -= 1U; \
                        *found_ = UINT8_C(1); \
                        status_ = HPS_COLLECTION_OK; \
                    } \
                } \
            } \
        } \
        return status_; \
    }

#endif /* HPS_DATA_STRUCTURE_HPS_DLIST_H */
