#ifndef HPS_DATA_STRUCTURE_HPS_VECTOR_H
#define HPS_DATA_STRUCTURE_HPS_VECTOR_H

/**
 * @file hps/data_structure/hps_vector.h
 * @brief Typed vector descriptors, bounded operations and iterators.
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

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_VECTOR_MAKE(name_, data_, length_, capacity_) \
    HPS_LVALUE(name_##_t, (data_), (length_), (capacity_))

#else

# define HPS_VECTOR_MAKE(name_, data_, length_, capacity_) \
    name_##_make((data_), (length_), (capacity_))

#endif

#define HPS_VECTOR_STATIC_INIT(data_, length_, capacity_) \
    { (data_), (length_), (capacity_) }

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

#endif /* HPS_DATA_STRUCTURE_HPS_VECTOR_H */
