#include "../test_suites.h"
#include "hps/data_structure/hps_span.h"
#include "hps/data_structure/hps_span.h"
#include <assert.h>
#include "hps/base/hps_stdint.h"

/**
 * @file tests/span/test_span.c
 * @brief Typed span construction, hints and argument evaluation checks.
 */

HPS_SPAN_DEFINE(uint16_t, hps_test_words)
static const uint16_t words[] = {11U, 22U, 33U};
static const hps_test_words_t initialized = HPS_SPAN_STATIC_INIT(words, 3U);

static const uint16_t *next_data(uint8_t *calls)
{
    *calls += 1U;
    return words;
}

static size_t next_length(uint8_t *calls)
{
    *calls += 1U;
    return 3U;
}

int32_t test_span(void)
{
    uint8_t data_calls = 0U;
    uint8_t length_calls = 0U;
    uint8_t hint_calls = 0U;
    hps_test_words_t span = HPS_SPAN_MAKE(hps_test_words,
        next_data(&data_calls), next_length(&length_calls));
    assert(data_calls == 1U && length_calls == 1U);
    assert(span.data == words && span.length == 3U && span.data[1] == 22U);
    assert(initialized.data == words && initialized.length == 3U);
    span = hps_test_words_make(NULL, 0U);
    assert(span.data == NULL && span.length == 0U);
    assert(HPS_LIKELY(++hint_calls) == 1 && hint_calls == 1U);
    assert(HPS_UNLIKELY(++hint_calls) == 1 && hint_calls == 2U);
    assert(HPS_LIKELY(0) == 0 && HPS_UNLIKELY(42) == 1);
    return INT32_C(0);
}
