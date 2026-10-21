#ifndef HPS_BASE_HPS_MACROS_H
#define HPS_BASE_HPS_MACROS_H

/**
 * @file hps/base/hps_macros.h
 * @brief Array extent and explicit type-spelling helpers.
 */

#include "hps/base/hps_stddef.h"

/* The argument must name an actual complete, non-VLA array, never a pointer or
 * a function array parameter. The result has type size_t. sizeof does not
 * evaluate such an argument; this portable form cannot diagnose pointer misuse.
 * No ownership, storage, runtime state or allocation is introduced. */
#define HPS_COUNT_OF(array_) (sizeof(array_) / sizeof((array_)[0]))
#define HPS_ARRAY_SIZE(array_) HPS_COUNT_OF(array_)
#define HPS_ARRAY_LENGTH(array_) HPS_COUNT_OF(array_)

/* RAW preserves the argument token spelling without macro expansion, matching
 * the legacy TO_STRING helper. STRINGIFY expands macros first. Both produce an
 * immutable C-string literal; they evaluate no expression or runtime state. */
#define HPS_STRINGIFY_RAW(tokens_) #tokens_
#define HPS_STRINGIFY(tokens_) HPS_STRINGIFY_RAW(tokens_)

/* These spell types, not runtime operations or ownership/reference semantics.
 * Use a simple type name or typedef compatible with this declaration syntax.
 * Direct const qualifiers and pointer declarators remain equally valid. */
#define HPS_VALUE(type_) type_
#define HPS_CONST_VALUE(type_) const type_
#define HPS_POINTER(type_) type_ *
#define HPS_POINTER_TO_CONST(type_) const type_ *
#define HPS_CONST_POINTER(type_) type_ * const
#define HPS_CONST_POINTER_TO_CONST(type_) const type_ * const

#endif /* HPS_BASE_HPS_MACROS_H */
