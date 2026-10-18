#ifndef HPS_VALIDATION_NATIONAL_CODE_H
#define HPS_VALIDATION_NATIONAL_CODE_H

/**
 * @file hps/validation/national_code.h
 * @brief Bounded legacy Iranian national-code checksum validation.
 */

#include <stddef.h>
#include <stdint.h>
#include "hps/string/view.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Read at most capacity characters including NUL. Successful result is 0/1:
 * exactly ten execution-set decimal digits, not all identical, and the legacy
 * modulo-11 checksum. This checks syntax/checksum only, not issuance, identity
 * or authorization. Invalid pointers/zero capacity => INVALID_ARGUMENT; missing
 * NUL => NOT_TERMINATED. Those errors preserve result. Wrong text => OK/0.
 * Input/result metadata are disjoint; caller proves initialized extent/lifetime.
 * No heap/OS/global state. Work O(capacity) for import, then at most ten digits. */
int32_t hps_iran_national_code_validate(const char *text,
                                        size_t capacity,
                                        uint8_t *result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_VALIDATION_NATIONAL_CODE_H */
