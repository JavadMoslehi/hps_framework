#include "hps/string/hps_policy.h"
#include "../internal/text.h"

/**
 * @file src/string/policy.c
 * @brief Explicit single-character ASCII policies and checked string copies.
 */

#if ('A' != 65) || ('a' != 97) || ('0' != 48)
#error "HPS string policies require an ASCII-compatible execution character set"
#endif

static uint8_t policy_none(uint8_t value);
static uint8_t policy_any(uint8_t value);
static uint8_t policy_ascii(uint8_t value);
static uint8_t policy_ascii_non_nul(uint8_t value);
static uint8_t policy_control_ascii(uint8_t value);
static uint8_t policy_printable_ascii(uint8_t value);
static uint8_t policy_graphical_ascii(uint8_t value);
static uint8_t policy_space(uint8_t value);
static uint8_t policy_horizontal_space(uint8_t value);
static uint8_t policy_line_break(uint8_t value);
static uint8_t policy_punctuation_ascii(uint8_t value);
static uint8_t policy_binary(uint8_t value);
static uint8_t policy_octal(uint8_t value);
static uint8_t policy_digit(uint8_t value);
static uint8_t policy_decimal_number(uint8_t value);
static uint8_t policy_lower_alpha(uint8_t value);
static uint8_t policy_upper_alpha(uint8_t value);
static uint8_t policy_alpha(uint8_t value);
static uint8_t policy_alnum(uint8_t value);
static uint8_t policy_alnum_space(uint8_t value);
static uint8_t policy_hex_lower(uint8_t value);
static uint8_t policy_hex_upper(uint8_t value);
static uint8_t policy_hex(uint8_t value);
static uint8_t policy_sign(uint8_t value);
static uint8_t policy_signed_integer_char(uint8_t value);
static uint8_t policy_decimal_char(uint8_t value);
static uint8_t policy_signed_decimal_char(uint8_t value);
static uint8_t policy_signed_number(uint8_t value);
static uint8_t policy_amount(uint8_t value);
static uint8_t policy_amount_decimal(uint8_t value);
static uint8_t policy_amount_decimal_comma(uint8_t value);
static uint8_t policy_identifier_first(uint8_t value);
static uint8_t policy_identifier_continue(uint8_t value);
static uint8_t policy_slug(uint8_t value);
static uint8_t policy_filename_portable(uint8_t value);
static uint8_t policy_path_portable(uint8_t value);
static uint8_t policy_hostname(uint8_t value);
static uint8_t policy_uri_unreserved(uint8_t value);
static uint8_t policy_email_char(uint8_t value);
static uint8_t policy_ipv4_char(uint8_t value);
static uint8_t policy_ipv6_char(uint8_t value);
static uint8_t policy_mac_address_char(uint8_t value);
static uint8_t policy_port_char(uint8_t value);
static uint8_t policy_pan(uint8_t value);
static uint8_t policy_pin(uint8_t value);
static uint8_t policy_phone_char(uint8_t value);
static uint8_t policy_track1(uint8_t value);
static uint8_t policy_track2(uint8_t value);
static uint8_t policy_track3(uint8_t value);
static uint8_t policy_iso8583_numeric(uint8_t value);
static uint8_t policy_iso8583_alpha(uint8_t value);
static uint8_t policy_iso8583_alnum(uint8_t value);
static uint8_t policy_iso8583_ans(uint8_t value);
static uint8_t policy_date_char(uint8_t value);
static uint8_t policy_time_char(uint8_t value);
static uint8_t policy_datetime_char(uint8_t value);
static uint8_t policy_base32(uint8_t value);
static uint8_t policy_base64(uint8_t value);
static uint8_t policy_base64_url(uint8_t value);

static uint8_t policy_none(const uint8_t value)
{
    (void)value;
    return 0U;
}

static uint8_t policy_any(const uint8_t value)
{
    (void)value;
    return 1U;
}

static uint8_t policy_ascii(const uint8_t value)
{
    return (uint8_t)((value <= 0x7Fu));
}

static uint8_t policy_ascii_non_nul(const uint8_t value)
{
    return (uint8_t)(((value > 0u) &&       policy_ascii(value)));
}

static uint8_t policy_control_ascii(const uint8_t value)
{
    return (uint8_t)(((value < 0x20u) ||       (value == 0x7Fu)));
}

static uint8_t policy_printable_ascii(const uint8_t value)
{
    return (uint8_t)(((value >= 0x20u) &&       (value <= 0x7Eu)));
}

static uint8_t policy_graphical_ascii(const uint8_t value)
{
    return (uint8_t)(((value >= 0x21u) &&       (value <= 0x7Eu)));
}

static uint8_t policy_space(const uint8_t value)
{
    return (uint8_t)((((value) == ' ')  || ((value) == '\t') ||       ((value) == '\r') || ((value) == '\n') ||       ((value) == '\v') || ((value) == '\f')));
}

static uint8_t policy_horizontal_space(const uint8_t value)
{
    return (uint8_t)((((value) == ' ') || ((value) == '\t')));
}

static uint8_t policy_line_break(const uint8_t value)
{
    return (uint8_t)((((value) == '\r') || ((value) == '\n')));
}

static uint8_t policy_punctuation_ascii(const uint8_t value)
{
    return (uint8_t)((policy_graphical_ascii(value) &&       !policy_alnum(value)));
}

static uint8_t policy_binary(const uint8_t value)
{
    return (uint8_t)((((value) == '0') || ((value) == '1')));
}

static uint8_t policy_octal(const uint8_t value)
{
    return (uint8_t)((((value) >= '0') && ((value) <= '7')));
}

static uint8_t policy_digit(const uint8_t value)
{
    return (uint8_t)((((value) >= '0') && ((value) <= '9')));
}

static uint8_t policy_decimal_number(const uint8_t value)
{
    return (uint8_t)(policy_digit(value));
}

static uint8_t policy_lower_alpha(const uint8_t value)
{
    return (uint8_t)((((value) >= 'a') && ((value) <= 'z')));
}

static uint8_t policy_upper_alpha(const uint8_t value)
{
    return (uint8_t)((((value) >= 'A') && ((value) <= 'Z')));
}

static uint8_t policy_alpha(const uint8_t value)
{
    return (uint8_t)((policy_lower_alpha(value) ||       policy_upper_alpha(value)));
}

static uint8_t policy_alnum(const uint8_t value)
{
    return (uint8_t)((policy_alpha(value) ||       policy_digit(value)));
}

static uint8_t policy_alnum_space(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) || ((value) == ' ')));
}

static uint8_t policy_hex_lower(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) ||       (((value) >= 'a') && ((value) <= 'f'))));
}

static uint8_t policy_hex_upper(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) ||       (((value) >= 'A') && ((value) <= 'F'))));
}

static uint8_t policy_hex(const uint8_t value)
{
    return (uint8_t)((policy_hex_lower(value) ||       policy_hex_upper(value)));
}

static uint8_t policy_sign(const uint8_t value)
{
    return (uint8_t)((((value) == '+') || ((value) == '-')));
}

static uint8_t policy_signed_integer_char(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) ||       policy_sign(value)));
}

static uint8_t policy_decimal_char(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) || ((value) == '.')));
}

static uint8_t policy_signed_decimal_char(const uint8_t value)
{
    return (uint8_t)((policy_decimal_char(value) ||       policy_sign(value)));
}

static uint8_t policy_signed_number(const uint8_t value)
{
    return (uint8_t)(policy_signed_integer_char(value));
}

static uint8_t policy_amount(const uint8_t value)
{
    return (uint8_t)(policy_digit(value));
}

static uint8_t policy_amount_decimal(const uint8_t value)
{
    return (uint8_t)(policy_decimal_char(value));
}

static uint8_t policy_amount_decimal_comma(const uint8_t value)
{
    return (uint8_t)((policy_decimal_char(value) || ((value) == ',')));
}

static uint8_t policy_identifier_first(const uint8_t value)
{
    return (uint8_t)((policy_alpha(value) || ((value) == '_')));
}

static uint8_t policy_identifier_continue(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) || ((value) == '_')));
}

static uint8_t policy_slug(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) ||       ((value) == '-') || ((value) == '_')));
}

static uint8_t policy_filename_portable(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) ||       ((value) == '-') || ((value) == '_') || ((value) == '.')));
}

static uint8_t policy_path_portable(const uint8_t value)
{
    return (uint8_t)((policy_filename_portable(value) ||       ((value) == '/') || ((value) == '\\') || ((value) == ':')));
}

static uint8_t policy_hostname(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) ||       ((value) == '-') || ((value) == '.')));
}

static uint8_t policy_uri_unreserved(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) ||       ((value) == '-') || ((value) == '.') || ((value) == '_') || ((value) == '~')));
}

static uint8_t policy_email_char(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) ||       ((value) == '!') || ((value) == '#') || ((value) == '$') ||       ((value) == '%') || ((value) == '&') || ((value) == '\'') ||       ((value) == '*') || ((value) == '+') || ((value) == '-') ||       ((value) == '/') || ((value) == '=') || ((value) == '?') ||       ((value) == '^') || ((value) == '_') || ((value) == '`') ||       ((value) == '{') || ((value) == '|') || ((value) == '}') ||       ((value) == '~') || ((value) == '.') || ((value) == '@')));
}

static uint8_t policy_ipv4_char(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) || ((value) == '.')));
}

static uint8_t policy_ipv6_char(const uint8_t value)
{
    return (uint8_t)((policy_hex(value) ||       ((value) == ':') || ((value) == '.')));
}

static uint8_t policy_mac_address_char(const uint8_t value)
{
    return (uint8_t)((policy_hex(value) ||       ((value) == ':') || ((value) == '-') || ((value) == '.')));
}

static uint8_t policy_port_char(const uint8_t value)
{
    return (uint8_t)(policy_digit(value));
}

static uint8_t policy_pan(const uint8_t value)
{
    return (uint8_t)(policy_digit(value));
}

static uint8_t policy_pin(const uint8_t value)
{
    return (uint8_t)(policy_digit(value));
}

static uint8_t policy_phone_char(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) ||       ((value) == '+') || ((value) == '#') || ((value) == '*') ||       ((value) == ',') || ((value) == 'p') || ((value) == 'P') ||       ((value) == 'w') || ((value) == 'W')));
}

static uint8_t policy_track1(const uint8_t value)
{
    return (uint8_t)(policy_printable_ascii(value));
}

static uint8_t policy_track2(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) ||       ((value) == '=') || ((value) == 'D') || ((value) == 'd')));
}

static uint8_t policy_track3(const uint8_t value)
{
    return (uint8_t)(policy_printable_ascii(value));
}

static uint8_t policy_iso8583_numeric(const uint8_t value)
{
    return (uint8_t)(policy_digit(value));
}

static uint8_t policy_iso8583_alpha(const uint8_t value)
{
    return (uint8_t)(policy_alpha(value));
}

static uint8_t policy_iso8583_alnum(const uint8_t value)
{
    return (uint8_t)(policy_alnum(value));
}

static uint8_t policy_iso8583_ans(const uint8_t value)
{
    return (uint8_t)(policy_printable_ascii(value));
}

static uint8_t policy_date_char(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) ||       ((value) == '-') || ((value) == '/')));
}

static uint8_t policy_time_char(const uint8_t value)
{
    return (uint8_t)((policy_digit(value) ||       ((value) == ':') || ((value) == '.')));
}

static uint8_t policy_datetime_char(const uint8_t value)
{
    return (uint8_t)((policy_date_char(value) ||       policy_time_char(value) ||       ((value) == 'T') || ((value) == 'Z') || ((value) == ' ')));
}

static uint8_t policy_base32(const uint8_t value)
{
    return (uint8_t)(((((value) >= 'A') && ((value) <= 'Z')) ||       (((value) >= '2') && ((value) <= '7')) || ((value) == '=')));
}

static uint8_t policy_base64(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) ||       ((value) == '+') || ((value) == '/') || ((value) == '=')));
}

static uint8_t policy_base64_url(const uint8_t value)
{
    return (uint8_t)((policy_alnum(value) ||       ((value) == '-') || ((value) == '_') || ((value) == '=')));
}

uint8_t hps_string_policy_accept(const uint8_t policy, const uint8_t value)
{
    uint8_t result = 0U;
    switch (policy) {
        case HPS_STRING_POLICY_NONE:
            result = policy_none(value);
            break;
        case HPS_STRING_POLICY_ANY:
            result = policy_any(value);
            break;
        case HPS_STRING_POLICY_ASCII:
            result = policy_ascii(value);
            break;
        case HPS_STRING_POLICY_ASCII_NON_NUL:
            result = policy_ascii_non_nul(value);
            break;
        case HPS_STRING_POLICY_CONTROL_ASCII:
            result = policy_control_ascii(value);
            break;
        case HPS_STRING_POLICY_PRINTABLE_ASCII:
            result = policy_printable_ascii(value);
            break;
        case HPS_STRING_POLICY_GRAPHICAL_ASCII:
            result = policy_graphical_ascii(value);
            break;
        case HPS_STRING_POLICY_SPACE:
            result = policy_space(value);
            break;
        case HPS_STRING_POLICY_HORIZONTAL_SPACE:
            result = policy_horizontal_space(value);
            break;
        case HPS_STRING_POLICY_LINE_BREAK:
            result = policy_line_break(value);
            break;
        case HPS_STRING_POLICY_PUNCTUATION_ASCII:
            result = policy_punctuation_ascii(value);
            break;
        case HPS_STRING_POLICY_BINARY:
            result = policy_binary(value);
            break;
        case HPS_STRING_POLICY_OCTAL:
            result = policy_octal(value);
            break;
        case HPS_STRING_POLICY_DIGIT:
            result = policy_digit(value);
            break;
        case HPS_STRING_POLICY_DECIMAL_NUMBER:
            result = policy_decimal_number(value);
            break;
        case HPS_STRING_POLICY_LOWER_ALPHA:
            result = policy_lower_alpha(value);
            break;
        case HPS_STRING_POLICY_UPPER_ALPHA:
            result = policy_upper_alpha(value);
            break;
        case HPS_STRING_POLICY_ALPHA:
            result = policy_alpha(value);
            break;
        case HPS_STRING_POLICY_ALNUM:
            result = policy_alnum(value);
            break;
        case HPS_STRING_POLICY_ALNUM_SPACE:
            result = policy_alnum_space(value);
            break;
        case HPS_STRING_POLICY_HEX_LOWER:
            result = policy_hex_lower(value);
            break;
        case HPS_STRING_POLICY_HEX_UPPER:
            result = policy_hex_upper(value);
            break;
        case HPS_STRING_POLICY_HEX:
            result = policy_hex(value);
            break;
        case HPS_STRING_POLICY_SIGN:
            result = policy_sign(value);
            break;
        case HPS_STRING_POLICY_SIGNED_INTEGER_CHAR:
            result = policy_signed_integer_char(value);
            break;
        case HPS_STRING_POLICY_DECIMAL_CHAR:
            result = policy_decimal_char(value);
            break;
        case HPS_STRING_POLICY_SIGNED_DECIMAL_CHAR:
            result = policy_signed_decimal_char(value);
            break;
        case HPS_STRING_POLICY_SIGNED_NUMBER:
            result = policy_signed_number(value);
            break;
        case HPS_STRING_POLICY_AMOUNT:
            result = policy_amount(value);
            break;
        case HPS_STRING_POLICY_AMOUNT_DECIMAL:
            result = policy_amount_decimal(value);
            break;
        case HPS_STRING_POLICY_AMOUNT_DECIMAL_COMMA:
            result = policy_amount_decimal_comma(value);
            break;
        case HPS_STRING_POLICY_IDENTIFIER_FIRST:
            result = policy_identifier_first(value);
            break;
        case HPS_STRING_POLICY_IDENTIFIER_CONTINUE:
            result = policy_identifier_continue(value);
            break;
        case HPS_STRING_POLICY_SLUG:
            result = policy_slug(value);
            break;
        case HPS_STRING_POLICY_FILENAME_PORTABLE:
            result = policy_filename_portable(value);
            break;
        case HPS_STRING_POLICY_PATH_PORTABLE:
            result = policy_path_portable(value);
            break;
        case HPS_STRING_POLICY_HOSTNAME:
            result = policy_hostname(value);
            break;
        case HPS_STRING_POLICY_URI_UNRESERVED:
            result = policy_uri_unreserved(value);
            break;
        case HPS_STRING_POLICY_EMAIL_CHAR:
            result = policy_email_char(value);
            break;
        case HPS_STRING_POLICY_IPV4_CHAR:
            result = policy_ipv4_char(value);
            break;
        case HPS_STRING_POLICY_IPV6_CHAR:
            result = policy_ipv6_char(value);
            break;
        case HPS_STRING_POLICY_MAC_ADDRESS_CHAR:
            result = policy_mac_address_char(value);
            break;
        case HPS_STRING_POLICY_PORT_CHAR:
            result = policy_port_char(value);
            break;
        case HPS_STRING_POLICY_PAN:
            result = policy_pan(value);
            break;
        case HPS_STRING_POLICY_PIN:
            result = policy_pin(value);
            break;
        case HPS_STRING_POLICY_PHONE_CHAR:
            result = policy_phone_char(value);
            break;
        case HPS_STRING_POLICY_TRACK1:
            result = policy_track1(value);
            break;
        case HPS_STRING_POLICY_TRACK2:
            result = policy_track2(value);
            break;
        case HPS_STRING_POLICY_TRACK3:
            result = policy_track3(value);
            break;
        case HPS_STRING_POLICY_ISO8583_NUMERIC:
            result = policy_iso8583_numeric(value);
            break;
        case HPS_STRING_POLICY_ISO8583_ALPHA:
            result = policy_iso8583_alpha(value);
            break;
        case HPS_STRING_POLICY_ISO8583_ALNUM:
            result = policy_iso8583_alnum(value);
            break;
        case HPS_STRING_POLICY_ISO8583_ANS:
            result = policy_iso8583_ans(value);
            break;
        case HPS_STRING_POLICY_DATE_CHAR:
            result = policy_date_char(value);
            break;
        case HPS_STRING_POLICY_TIME_CHAR:
            result = policy_time_char(value);
            break;
        case HPS_STRING_POLICY_DATETIME_CHAR:
            result = policy_datetime_char(value);
            break;
        case HPS_STRING_POLICY_BASE32:
            result = policy_base32(value);
            break;
        case HPS_STRING_POLICY_BASE64:
            result = policy_base64(value);
            break;
        case HPS_STRING_POLICY_BASE64_URL:
            result = policy_base64_url(value);
            break;
        default:
            break;
    }
    return result;
}

static int32_t policy_write(hps_string_buffer *buffer,
                            const hps_string_view source,
                            const uint8_t policy, const uint8_t append)
{
    size_t remaining = 0U;
    int32_t status = hps_string_buffer_remaining(buffer, &remaining);
    if (status == HPS_STRING_OK) {
        status = hps_string_check_view(source);
        if (status == HPS_STRING_OK) {
            if (policy > HPS_STRING_POLICY_BASE64_URL) {
                status = HPS_STRING_INVALID_ARGUMENT;
            }
            size_t index = 0U;
            while ((index < source.length) && (status == HPS_STRING_OK)) {
                if ((source.data[index] == '\0') ||
                    (hps_string_policy_accept(policy, (uint8_t)source.data[index]) == 0U)) {
                    status = HPS_STRING_INVALID_ARGUMENT;
                }
                index += 1U;
            }
            if (status == HPS_STRING_OK) {
                status = (append == 1U) ? hps_string_buffer_append(buffer, source) :
                    hps_string_buffer_assign(buffer, source);
            }
        }
    }
    return status;
}

int32_t hps_string_policy_assign(hps_string_buffer *buffer,
                                 const hps_string_view source,
                                 const uint8_t policy)
{
    return policy_write(buffer, source, policy, 0U);
}

int32_t hps_string_policy_append(hps_string_buffer *buffer,
                                 const hps_string_view source,
                                 const uint8_t policy)
{
    return policy_write(buffer, source, policy, 1U);
}
