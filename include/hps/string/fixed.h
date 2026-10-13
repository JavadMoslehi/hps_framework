#ifndef HPS_STRING_FIXED_H
#define HPS_STRING_FIXED_H

/**
 * @file hps/string/fixed.h
 * @brief Typed fixed C-string storage with explicit buffer binding.
 */

#include <stddef.h>
#include "hps/base/attributes.h"
#include "hps/string/buffer.h"

/* Define at file scope; capacity_ is a positive integer constant INCLUDING NUL.
 * Storage owns exactly capacity_ character elements, no hidden pointer/heap.
 * bind empties storage and binds a separate descriptor; adopt validates existing
 * terminated text. Both reject NULL and preserve outputs on error. Descriptors
 * must be disjoint from storage and borrow it only during its lifetime. Copying
 * a storage object requires rebinding descriptors to its new address. No packed
 * layout, persistence/wire representation, ROM/stack/erasure or concurrency claim.
 * Generated functions are internal; capacities/stack budgets belong to caller.
 * Actual char storage has a proposed scoped project-policy disposition. */
#define HPS_STRING_FIXED_DEFINE(name_, capacity_) \
    typedef struct name_##_s { char data[(capacity_)]; } name_##_t; \
    typedef size_t name_##_capacity_positive[((capacity_) > 0U) ? 1 : -1]; \
    static HPS_INLINE size_t name_##_capacity(void) \
    { return (size_t)(capacity_); } \
    static HPS_INLINE hps_string_status name_##_bind(name_##_t *storage_, hps_string_buffer *buffer_) \
    { \
        return (storage_ == NULL) ? HPS_STRING_INVALID_ARGUMENT : \
            hps_string_buffer_init(buffer_, storage_->data, sizeof(storage_->data)); \
    } \
    static HPS_INLINE hps_string_status name_##_adopt(name_##_t *storage_, hps_string_buffer *buffer_) \
    { \
        return (storage_ == NULL) ? HPS_STRING_INVALID_ARGUMENT : \
            hps_string_buffer_init_cstr(buffer_, storage_->data, sizeof(storage_->data)); \
    }

#define HPS_STRING_FIXED_STATIC_INIT { { '\0' } }

#endif /* HPS_STRING_FIXED_H */
