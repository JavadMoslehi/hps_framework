#ifndef HPS_DATA_STRUCTURE_OPTIONAL_H
#define HPS_DATA_STRUCTURE_OPTIONAL_H

/**
 * @file hps/data_structure/optional.h
 * @brief Typed optional values with checked access and explicit copy semantics.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/base/attributes.h"
#include "hps/data_structure/define.h"

#define HPS_OPTIONAL_OK ((int32_t)INT32_C(0))
#define HPS_OPTIONAL_INVALID_ARGUMENT ((int32_t)-INT32_C(1))
#define HPS_OPTIONAL_INVALID_STATE ((int32_t)-INT32_C(2))
#define HPS_OPTIONAL_EMPTY ((int32_t)-INT32_C(3))

/* Expand once at file scope with a unique hps_-prefixed name_ and a complete,
 * assignable, copyable object type allowed by the numerical/storage policy.
 * Use a typedef for pointer elements. C++ elements must be trivial C-compatible
 * objects. Generates name_t with uint8_t has_value and an actual type_ value,
 * plus internal functions name_none/some/check/has/get/get_or/set_none/set_some.
 *
 * none constructs flag zero and a properly typed zero-initialized payload,
 * including real null pointers; it never treats memset as object initialization.
 * some constructs flag one and copies its value. Initialize via a constructor
 * before checked operations. Only flags zero and one are valid. set_none clears
 * the flag and retains the initialized payload without erasure/destruction;
 * set_some copies the new value before publishing flag one. some(NULL) is a
 * present pointer value and differs from none. No heap, ownership transfer,
 * retain/release, cleanup callback, union or implicit scope-exit action occurs.
 * Copy/overwrite/discard must be permitted by the element's own lifetime and
 * ownership contract. Caller owns live typed storage and serializes accesses.
 *
 * Ordinary function arguments are evaluated once, with no promised ordering.
 * In particular get_or EAGERLY evaluates its fallback even for a present value,
 * unlike the legacy conditional macro's lazy branch. Use explicit caller flow
 * when fallback work must be conditional. There is no unsafe unwrap, anonymous
 * struct type language, void-pointer flag access or callback mapping engine.
 *
 * check validates only pointer nullness and the initialized flag. It cannot
 * prove arbitrary pointer validity, lifetime or correct payload ownership.
 * has/get/get_or require a writable output disjoint from the optional and its
 * referenced payloads. NULL required arguments take priority over INVALID_STATE;
 * get returns EMPTY only after valid state. has writes exactly zero or one;
 * get_or succeeds on either valid flag. All errors preserve state and outputs.
 * Per object storage is sizeof(type_) plus one uint8_t and native padding. Work
 * and automatic storage are bounded by the fixed generated type size; checking
 * and flag changes use O(1) scalar metadata, and value operations copy at most
 * one element plus the returned wrapper. Target ABI/stack/padding need review.
 */
#define HPS_OPTIONAL_DEFINE(type_, name_) \
    typedef struct name_##_s { \
        uint8_t has_value; \
        type_ value; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_none(void) \
    { \
        name_##_t optional_ = HPS_DATA_STRUCTURE_EMPTY_INIT; \
        return optional_; \
    } \
    static HPS_INLINE name_##_t name_##_some(type_ value_) \
    { \
        name_##_t optional_ = {UINT8_C(1), value_}; \
        return optional_; \
    } \
    static HPS_INLINE int32_t name_##_check(const name_##_t *optional_) \
    { \
        int32_t status_ = HPS_OPTIONAL_INVALID_ARGUMENT; \
        if (optional_ != NULL) { \
            status_ = (optional_->has_value <= UINT8_C(1)) ? \
                HPS_OPTIONAL_OK : HPS_OPTIONAL_INVALID_STATE; \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_has( \
        const name_##_t *optional_, uint8_t *result_) \
    { \
        int32_t status_ = HPS_OPTIONAL_INVALID_ARGUMENT; \
        if (result_ != NULL) { \
            status_ = name_##_check(optional_); \
            if (status_ == HPS_OPTIONAL_OK) { \
                *result_ = optional_->has_value; \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_get( \
        const name_##_t *optional_, type_ *result_) \
    { \
        int32_t status_ = HPS_OPTIONAL_INVALID_ARGUMENT; \
        if (result_ != NULL) { \
            status_ = name_##_check(optional_); \
            if (status_ == HPS_OPTIONAL_OK) { \
                if (optional_->has_value == UINT8_C(0)) { \
                    status_ = HPS_OPTIONAL_EMPTY; \
                } else { \
                    *result_ = optional_->value; \
                } \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_get_or( \
        const name_##_t *optional_, type_ fallback_, type_ *result_) \
    { \
        int32_t status_ = HPS_OPTIONAL_INVALID_ARGUMENT; \
        if (result_ != NULL) { \
            status_ = name_##_check(optional_); \
            if (status_ == HPS_OPTIONAL_OK) { \
                *result_ = (optional_->has_value == UINT8_C(1)) ? \
                    optional_->value : fallback_; \
            } \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_set_none(name_##_t *optional_) \
    { \
        int32_t status_ = name_##_check(optional_); \
        if (status_ == HPS_OPTIONAL_OK) { \
            optional_->has_value = UINT8_C(0); \
        } \
        return status_; \
    } \
    static HPS_INLINE int32_t name_##_set_some( \
        name_##_t *optional_, type_ value_) \
    { \
        int32_t status_ = name_##_check(optional_); \
        if (status_ == HPS_OPTIONAL_OK) { \
            optional_->value = value_; \
            optional_->has_value = UINT8_C(1); \
        } \
        return status_; \
    }

#endif /* HPS_DATA_STRUCTURE_OPTIONAL_H */
