#ifndef HPS_MODULE_H
#define HPS_MODULE_H

/**
 * @file hps/module.h
 * @brief Template for a public HPS C module header.
 */

#include "hps/base/hps_stdint.h"

/* Local errors: meaningful only with this module's API. */
#define HPS_MODULE_OK ((int32_t)INT32_C(0))
#define HPS_MODULE_INVALID_ARGUMENT ((int32_t)-INT32_C(1))

#ifdef __cplusplus
extern "C" {
#endif

/* Template only: replace this prototype with the reviewed module contract.
 * Document initialization, inputs, failure outputs, lifetime and execution context.
 * Returns HPS_MODULE_OK or a documented local error; failure preserves outputs.
 * Keep return representation and numeric-code migration explicit for consumers.
 */
int32_t hps_module_operation(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MODULE_H */
