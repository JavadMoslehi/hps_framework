#ifndef HPS_DATA_STRUCTURE_HPS_SPAN_H
#define HPS_DATA_STRUCTURE_HPS_SPAN_H

/**
 * @file hps/data_structure/hps_span.h
 * @brief Read-only typed spans and construction helpers.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_attributes.h"
#include "hps/base/hps_expression.h"

/* Expand at file scope with an object type and a unique prefix. length counts
 * elements, not bytes. NULL requires length zero; nonempty spans borrow that
 * many initialized objects of type_ with a live backing array. Construction is
 * unchecked (as aggregate initialization is), copies no payload and owns nothing.
 * Caller proves extent/lifetime and serializes conflicting access/release.
 * Make arguments are evaluated once; STATIC_INIT requires constant expressions
 * for static storage. Do not infer C89 integer-header or target ABI support from
 * the generator's C89 declaration syntax. Functions have internal linkage. */
#define HPS_SPAN_DEFINE(type_, name_) \
    typedef struct name_##_s { \
        const type_ *data; \
        size_t length; \
    } name_##_t; \
    static HPS_INLINE name_##_t name_##_make(const type_ *data_, size_t length_) \
    { \
        name_##_t span_; \
        span_.data = data_; \
        span_.length = length_; \
        return span_; \
    }

/* C99 compound literals and portable constructor fallbacks evaluate each input
 * once. Both return values; neither adds validation or resource management. */

#if defined(HPS_LVALUE)

# define HPS_SPAN_MAKE(name_, data_, length_) \
    HPS_LVALUE(name_##_t, (data_), (length_))

#else

# define HPS_SPAN_MAKE(name_, data_, length_) \
    name_##_make((data_), (length_))

#endif

#define HPS_SPAN_STATIC_INIT(data_, length_) { (data_), (length_) }

#endif /* HPS_DATA_STRUCTURE_HPS_SPAN_H */
