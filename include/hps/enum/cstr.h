#ifndef HPS_ENUM_CSTR_H
#define HPS_ENUM_CSTR_H

/**
 * @file hps/enum/cstr.h
 * @brief X-macro string keys mapped to native enum values for switch statements.
 */

#include <stddef.h>
#include <string.h>
#include "hps/base/attributes.h"

/* table_(ENTRY, PREFIX) expands ENTRY(PREFIX, ID, "literal") for each key.
 * IDs are unique; keys must be immutable string literals without embedded NUL.
 * Duplicate text maps to its first entry. Expand at file scope using unique
 * type/prefix names. Generated functions have internal linkage; no external ABI
 * or linkage block is needed. from_cstr requires a real readable capacity including
 * NUL and maps NULL, missing termination and unmatched keys to UNKNOWN. Input
 * lifetime/extent and serialization are caller preconditions. Work is O(capacity
 * + table text), no allocation/OS/global mutable state. to_cstr borrows immutable
 * program-lifetime text and maps invalid/UNKNOWN/COUNT values to "unknown".
 * This deliberately retains the user's native-enum switch interface; its scoped
 * numerical-policy disposition remains proposed, not MISRA compliance evidence.
 */
#define HPS_CSTR_ENUM_DEFINE_STATIC(type_name_, prefix_, table_) \
    typedef enum { \
        prefix_##_UNKNOWN = 0, \
        table_(HPS_DETAIL_CSTR_ENUM_ENTRY, prefix_) \
        prefix_##_COUNT \
    } type_name_; \
    static HPS_INLINE type_name_ type_name_##_from_cstr(const char *text_, size_t capacity_) \
    { \
        size_t length_; \
        if ((text_ == NULL) || (capacity_ == 0U)) { \
            return prefix_##_UNKNOWN; \
        } \
        length_ = 0U; \
        while ((length_ < capacity_) && (text_[length_] != '\0')) { \
            length_ += 1U; \
        } \
        if (length_ == capacity_) { \
            return prefix_##_UNKNOWN; \
        } \
        table_(HPS_DETAIL_CSTR_ENUM_MATCH, prefix_) \
        return prefix_##_UNKNOWN; \
    } \
    static HPS_INLINE const char *type_name_##_to_cstr(type_name_ value_) \
    { \
        switch (value_) { \
            table_(HPS_DETAIL_CSTR_ENUM_CASE, prefix_) \
            case prefix_##_UNKNOWN: \
            case prefix_##_COUNT: \
            default: \
                return "unknown"; \
        } \
    }

#define HPS_DETAIL_CSTR_ENUM_ENTRY(prefix_, id_, text_) prefix_##_##id_,
#define HPS_DETAIL_CSTR_ENUM_MATCH(prefix_, id_, literal_) \
    if ((length_ == (sizeof(literal_) - 1U)) && \
        (memcmp(text_, (literal_), length_) == 0)) { return prefix_##_##id_; }
#define HPS_DETAIL_CSTR_ENUM_CASE(prefix_, id_, literal_) \
    case prefix_##_##id_: return (literal_);

#endif /* HPS_ENUM_CSTR_H */
