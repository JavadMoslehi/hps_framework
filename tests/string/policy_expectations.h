#ifndef HPS_TESTS_STRING_POLICY_EXPECTATIONS_H
#define HPS_TESTS_STRING_POLICY_EXPECTATIONS_H

#include <stdint.h>

/* Differential fixtures from the inventoried CSEP policy expression AST.
 * Original source was read as data, without compilation/execution or edits.
 * 59 policies x256 ASCII/octet values; each row stores 256 membership bits.
 * Fixture parity does not establish grammar validation or MISRA compliance. */
static const uint8_t policy_expected[59][32] = {
    /* NONE */
    {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ANY */
    {255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U},
    /* ASCII */
    {255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ASCII_NON_NUL */
    {254U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* CONTROL_ASCII */
    {255U, 255U, 255U, 255U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 128U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* PRINTABLE_ASCII */
    {0U, 0U, 0U, 0U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 127U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* GRAPHICAL_ASCII */
    {0U, 0U, 0U, 0U, 254U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 127U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* SPACE */
    {0U, 62U, 0U, 0U, 1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* HORIZONTAL_SPACE */
    {0U, 2U, 0U, 0U, 1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* LINE_BREAK */
    {0U, 36U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* PUNCTUATION_ASCII */
    {0U, 0U, 0U, 0U, 254U, 255U, 0U, 252U, 1U, 0U, 0U, 248U, 1U, 0U, 0U, 120U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* BINARY */
    {0U, 0U, 0U, 0U, 0U, 0U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* OCTAL */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* DIGIT */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* DECIMAL_NUMBER */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* LOWER_ALPHA */
    {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* UPPER_ALPHA */
    {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ALPHA */
    {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 254U, 255U, 255U, 7U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ALNUM */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 254U, 255U, 255U, 7U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ALNUM_SPACE */
    {0U, 0U, 0U, 0U, 1U, 0U, 255U, 3U, 254U, 255U, 255U, 7U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* HEX_LOWER */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 126U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* HEX_UPPER */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 126U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* HEX */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 126U, 0U, 0U, 0U, 126U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* SIGN */
    {0U, 0U, 0U, 0U, 0U, 40U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* SIGNED_INTEGER_CHAR */
    {0U, 0U, 0U, 0U, 0U, 40U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* DECIMAL_CHAR */
    {0U, 0U, 0U, 0U, 0U, 64U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* SIGNED_DECIMAL_CHAR */
    {0U, 0U, 0U, 0U, 0U, 104U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* SIGNED_NUMBER */
    {0U, 0U, 0U, 0U, 0U, 40U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* AMOUNT */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* AMOUNT_DECIMAL */
    {0U, 0U, 0U, 0U, 0U, 64U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* AMOUNT_DECIMAL_COMMA */
    {0U, 0U, 0U, 0U, 0U, 80U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* IDENTIFIER_FIRST */
    {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 254U, 255U, 255U, 135U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* IDENTIFIER_CONTINUE */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 254U, 255U, 255U, 135U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* SLUG */
    {0U, 0U, 0U, 0U, 0U, 32U, 255U, 3U, 254U, 255U, 255U, 135U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* FILENAME_PORTABLE */
    {0U, 0U, 0U, 0U, 0U, 96U, 255U, 3U, 254U, 255U, 255U, 135U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* PATH_PORTABLE */
    {0U, 0U, 0U, 0U, 0U, 224U, 255U, 7U, 254U, 255U, 255U, 151U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* HOSTNAME */
    {0U, 0U, 0U, 0U, 0U, 96U, 255U, 3U, 254U, 255U, 255U, 7U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* URI_UNRESERVED */
    {0U, 0U, 0U, 0U, 0U, 96U, 255U, 3U, 254U, 255U, 255U, 135U, 254U, 255U, 255U, 71U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* EMAIL_CHAR */
    {0U, 0U, 0U, 0U, 250U, 236U, 255U, 163U, 255U, 255U, 255U, 199U, 255U, 255U, 255U, 127U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* IPV4_CHAR */
    {0U, 0U, 0U, 0U, 0U, 64U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* IPV6_CHAR */
    {0U, 0U, 0U, 0U, 0U, 64U, 255U, 7U, 126U, 0U, 0U, 0U, 126U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* MAC_ADDRESS_CHAR */
    {0U, 0U, 0U, 0U, 0U, 96U, 255U, 7U, 126U, 0U, 0U, 0U, 126U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* PORT_CHAR */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* PAN */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* PIN */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* PHONE_CHAR */
    {0U, 0U, 0U, 0U, 8U, 28U, 255U, 3U, 0U, 0U, 129U, 0U, 0U, 0U, 129U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* TRACK1 */
    {0U, 0U, 0U, 0U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 127U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* TRACK2 */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 35U, 16U, 0U, 0U, 0U, 16U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* TRACK3 */
    {0U, 0U, 0U, 0U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 127U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ISO8583_NUMERIC */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ISO8583_ALPHA */
    {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 254U, 255U, 255U, 7U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ISO8583_ALNUM */
    {0U, 0U, 0U, 0U, 0U, 0U, 255U, 3U, 254U, 255U, 255U, 7U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* ISO8583_ANS */
    {0U, 0U, 0U, 0U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U, 127U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* DATE_CHAR */
    {0U, 0U, 0U, 0U, 0U, 160U, 255U, 3U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* TIME_CHAR */
    {0U, 0U, 0U, 0U, 0U, 64U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* DATETIME_CHAR */
    {0U, 0U, 0U, 0U, 1U, 224U, 255U, 7U, 0U, 0U, 16U, 4U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* BASE32 */
    {0U, 0U, 0U, 0U, 0U, 0U, 252U, 32U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* BASE64 */
    {0U, 0U, 0U, 0U, 0U, 136U, 255U, 35U, 254U, 255U, 255U, 7U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    /* BASE64_URL */
    {0U, 0U, 0U, 0U, 0U, 32U, 255U, 35U, 254U, 255U, 255U, 135U, 254U, 255U, 255U, 7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
};

#endif /* HPS_TESTS_STRING_POLICY_EXPECTATIONS_H */
