#ifndef HPS_STRING_POLICY_H
#define HPS_STRING_POLICY_H

/**
 * @file hps/string/policy.h
 * @brief Single-evaluation ASCII alphabet policies migrated from fixed strings.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/string/buffer.h"

#define HPS_STRING_POLICY_NONE ((uint8_t)0U)
#define HPS_STRING_POLICY_ANY ((uint8_t)1U)
#define HPS_STRING_POLICY_ASCII ((uint8_t)2U)
#define HPS_STRING_POLICY_ASCII_NON_NUL ((uint8_t)3U)
#define HPS_STRING_POLICY_CONTROL_ASCII ((uint8_t)4U)
#define HPS_STRING_POLICY_PRINTABLE_ASCII ((uint8_t)5U)
#define HPS_STRING_POLICY_GRAPHICAL_ASCII ((uint8_t)6U)
#define HPS_STRING_POLICY_SPACE ((uint8_t)7U)
#define HPS_STRING_POLICY_HORIZONTAL_SPACE ((uint8_t)8U)
#define HPS_STRING_POLICY_LINE_BREAK ((uint8_t)9U)
#define HPS_STRING_POLICY_PUNCTUATION_ASCII ((uint8_t)10U)
#define HPS_STRING_POLICY_BINARY ((uint8_t)11U)
#define HPS_STRING_POLICY_OCTAL ((uint8_t)12U)
#define HPS_STRING_POLICY_DIGIT ((uint8_t)13U)
#define HPS_STRING_POLICY_DECIMAL_NUMBER ((uint8_t)14U)
#define HPS_STRING_POLICY_LOWER_ALPHA ((uint8_t)15U)
#define HPS_STRING_POLICY_UPPER_ALPHA ((uint8_t)16U)
#define HPS_STRING_POLICY_ALPHA ((uint8_t)17U)
#define HPS_STRING_POLICY_ALNUM ((uint8_t)18U)
#define HPS_STRING_POLICY_ALNUM_SPACE ((uint8_t)19U)
#define HPS_STRING_POLICY_HEX_LOWER ((uint8_t)20U)
#define HPS_STRING_POLICY_HEX_UPPER ((uint8_t)21U)
#define HPS_STRING_POLICY_HEX ((uint8_t)22U)
#define HPS_STRING_POLICY_SIGN ((uint8_t)23U)
#define HPS_STRING_POLICY_SIGNED_INTEGER_CHAR ((uint8_t)24U)
#define HPS_STRING_POLICY_DECIMAL_CHAR ((uint8_t)25U)
#define HPS_STRING_POLICY_SIGNED_DECIMAL_CHAR ((uint8_t)26U)
#define HPS_STRING_POLICY_SIGNED_NUMBER ((uint8_t)27U)
#define HPS_STRING_POLICY_AMOUNT ((uint8_t)28U)
#define HPS_STRING_POLICY_AMOUNT_DECIMAL ((uint8_t)29U)
#define HPS_STRING_POLICY_AMOUNT_DECIMAL_COMMA ((uint8_t)30U)
#define HPS_STRING_POLICY_IDENTIFIER_FIRST ((uint8_t)31U)
#define HPS_STRING_POLICY_IDENTIFIER_CONTINUE ((uint8_t)32U)
#define HPS_STRING_POLICY_SLUG ((uint8_t)33U)
#define HPS_STRING_POLICY_FILENAME_PORTABLE ((uint8_t)34U)
#define HPS_STRING_POLICY_PATH_PORTABLE ((uint8_t)35U)
#define HPS_STRING_POLICY_HOSTNAME ((uint8_t)36U)
#define HPS_STRING_POLICY_URI_UNRESERVED ((uint8_t)37U)
#define HPS_STRING_POLICY_EMAIL_CHAR ((uint8_t)38U)
#define HPS_STRING_POLICY_IPV4_CHAR ((uint8_t)39U)
#define HPS_STRING_POLICY_IPV6_CHAR ((uint8_t)40U)
#define HPS_STRING_POLICY_MAC_ADDRESS_CHAR ((uint8_t)41U)
#define HPS_STRING_POLICY_PORT_CHAR ((uint8_t)42U)
#define HPS_STRING_POLICY_PAN ((uint8_t)43U)
#define HPS_STRING_POLICY_PIN ((uint8_t)44U)
#define HPS_STRING_POLICY_PHONE_CHAR ((uint8_t)45U)
#define HPS_STRING_POLICY_TRACK1 ((uint8_t)46U)
#define HPS_STRING_POLICY_TRACK2 ((uint8_t)47U)
#define HPS_STRING_POLICY_TRACK3 ((uint8_t)48U)
#define HPS_STRING_POLICY_ISO8583_NUMERIC ((uint8_t)49U)
#define HPS_STRING_POLICY_ISO8583_ALPHA ((uint8_t)50U)
#define HPS_STRING_POLICY_ISO8583_ALNUM ((uint8_t)51U)
#define HPS_STRING_POLICY_ISO8583_ANS ((uint8_t)52U)
#define HPS_STRING_POLICY_DATE_CHAR ((uint8_t)53U)
#define HPS_STRING_POLICY_TIME_CHAR ((uint8_t)54U)
#define HPS_STRING_POLICY_DATETIME_CHAR ((uint8_t)55U)
#define HPS_STRING_POLICY_BASE32 ((uint8_t)56U)
#define HPS_STRING_POLICY_BASE64 ((uint8_t)57U)
#define HPS_STRING_POLICY_BASE64_URL ((uint8_t)58U)

#ifdef __cplusplus
extern "C" {
#endif

/* Alphabet membership only, never full email/IP/date/payment grammar validation.
 * value is an ASCII octet; policy is one of the constants. Invalid policy returns
 * flag zero. Inputs evaluated once, O(1), no state/allocation/locale/Unicode. */
uint8_t hps_string_policy_accept(uint8_t policy, uint8_t value);

/* Validate non-NUL text before a complete assign/append. Every failure preserves
 * buffer and outputs; source payload may overlap buffer. Unknown policy/negative
 * membership => INVALID_ARGUMENT. State, source shape, policy then capacity are
 * checked before mutation. O(old length + source length). */
int32_t hps_string_policy_assign(hps_string_buffer *buffer,
                                 hps_string_view source, uint8_t policy);
int32_t hps_string_policy_append(hps_string_buffer *buffer,
                                 hps_string_view source, uint8_t policy);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HPS_STRING_POLICY_H */
