#ifndef HPS_DATA_STRUCTURE_OPERATIONS_H
#define HPS_DATA_STRUCTURE_OPERATIONS_H

/**
 * @file hps/data_structure/operations.h
 * @brief Bounded typed operations and iterators for existing vector/list layouts.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/base/attributes.h"
#include "hps/data_structure/define.h"

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

/* Adds name_check, name_push, name_get and name_iterator_begin/next.
 * check validates length<=capacity, capacity*sizeof(type_) representability and
 * the NULL/zero-capacity rule; it cannot prove actual storage extent. The vector
 * borrows capacity live typed objects, of which length are initialized values.
 * push writes one element before increasing length; full capacity is an error.
 * get copies one initialized element to a non-NULL output disjoint from vector
 * metadata/storage. index==length is out of range, including empty vectors.
 *
 * begin initializes a disjoint iterator and captures the borrowed active range.
 * Until traversal ends or is abandoned, do not mutate the source descriptor,
 * elements or their storage/lifetime. Iterator copies are independent borrowed
 * cursors and do not extend lifetime. next needs non-NULL mutually disjoint
 * value/found outputs, disjoint from iterator and source storage. It writes
 * found=1 and a copied value on an element; at end it writes found=0 and preserves
 * value. Repeated end calls succeed. Failed calls preserve iterator and outputs.
 */
#define HPS_VECTOR_OPERATIONS_DEFINE(type_, name_) \
    typedef struct name_##_iterator_s { \
        const type_ *data; \
        size_t length; \
        size_t index; \
    } name_##_iterator_t; \
    static HPS_INLINE int32_t name_##_check(const name_##_t *container_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if (container_ != NULL) { \
            if ((container_->length > container_->capacity) || \
                ((container_->data == NULL) && \
                 (container_->capacity != 0U))) { \
                status_ = HPS_COLLECTION_INVALID_STATE; \
            } else if (container_->capacity > SIZE_MAX / sizeof(type_)) { \
                status_ = HPS_COLLECTION_SIZE_OVERFLOW; \
            } else { \
                status_ = HPS_COLLECTION_OK; \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_push(name_##_t *container_, type_ value_) \
    { \
        int32_t status_ = name_##_check(container_); \
        if (status_ == HPS_COLLECTION_OK) { \
            if (container_->length == container_->capacity) { \
                status_ = HPS_COLLECTION_CAPACITY; \
            } else { \
                container_->data[container_->length] = value_; \
                container_->length += 1U; \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_get( \
        const name_##_t *container_, size_t index_, type_ *value_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if (value_ != NULL) { \
            status_ = name_##_check(container_); \
            if (status_ == HPS_COLLECTION_OK) { \
                if (index_ >= container_->length) { \
                    status_ = HPS_COLLECTION_OUT_OF_RANGE; \
                } else { \
                    *value_ = container_->data[index_]; \
                } \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_iterator_begin( \
        const name_##_t *container_, name_##_iterator_t *iterator_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if (iterator_ != NULL) { \
            status_ = name_##_check(container_); \
            if (status_ == HPS_COLLECTION_OK) { \
                iterator_->data = container_->data; \
                iterator_->length = container_->length; \
                iterator_->index = 0U; \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_iterator_next( \
        name_##_iterator_t *iterator_, type_ *value_, uint8_t *found_) \
    { \
        int32_t status_ = HPS_COLLECTION_INVALID_ARGUMENT; \
        if ((iterator_ != NULL) && (value_ != NULL) && (found_ != NULL)) { \
            if ((iterator_->index > iterator_->length) || \
                ((iterator_->data == NULL) && (iterator_->length != 0U))) { \
                status_ = HPS_COLLECTION_INVALID_STATE; \
            } else if (iterator_->length > SIZE_MAX / sizeof(type_)) { \
                status_ = HPS_COLLECTION_SIZE_OVERFLOW; \
            } else { \
                if (iterator_->index == iterator_->length) { \
                    *found_ = UINT8_C(0); \
                } else { \
                    *value_ = iterator_->data[iterator_->index]; \
                    iterator_->index += 1U; \
                    *found_ = UINT8_C(1); \
                } \
                status_ = HPS_COLLECTION_OK; \
            } \
        } \
        return status_; \
    }

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

#endif /* HPS_DATA_STRUCTURE_OPERATIONS_H */
