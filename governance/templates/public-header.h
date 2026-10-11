#ifndef HPS_MODULE_H
#define HPS_MODULE_H

/**
 * @file hps/module.h
 * @brief Template for a public HPS C module header.
 */

#include <stdint.h>

typedef uint8_t hps_module_status;

#ifdef __cplusplus
extern "C" {
#endif

/* Template only: replace this prototype with the reviewed module contract.
 * Document initialization, inputs, failure outputs, lifetime and execution context.
 */
hps_module_status hps_module_operation(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_MODULE_H */
