#include "../test_suites.h"
#include "hps/status.h"
#include "hps/bytes/buffer.h"
#include "hps/memory/pool.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/**
 * @file tests/status/test_status.c
 * @brief Prepared shared-status namespace, diagnostic and integration tests.
 */

typedef struct {
    hps_status_t status;
    int64_t expected_value;
    uint16_t module;
    const char *message;
} status_case;

typedef struct {
    hps_status_t status;
    uint16_t module;
} module_case;

static void assert_status_message(hps_status_t status, const char *expected)
{
    const char *message = hps_status_string(status);
    assert(message != NULL);
    assert(strlen(message) < HPS_STATUS_MESSAGE_CAPACITY);
    assert(strcmp(message, expected) == 0);
}

static void test_status_registered_codes(void)
{
    const status_case cases[] = {
        {HPS_STATUS_INVALID_ARGUMENT, -INT64_C(1),
         HPS_STATUS_MODULE_FRAMEWORK, "framework: invalid argument"},
        {HPS_STATUS_OUT_OF_RANGE, -INT64_C(2),
         HPS_STATUS_MODULE_FRAMEWORK, "framework: out of range"},
        {HPS_STATUS_NO_SPACE, -INT64_C(3),
         HPS_STATUS_MODULE_FRAMEWORK, "framework: no space"},
        {HPS_STATUS_INVALID_STATE, -INT64_C(4),
         HPS_STATUS_MODULE_FRAMEWORK, "framework: invalid state"},
        {HPS_POOL_INVALID_ARGUMENT, -INT64_C(65537),
         HPS_STATUS_MODULE_MEMORY, "memory: invalid argument"},
        {HPS_POOL_EXHAUSTED, -INT64_C(65538),
         HPS_STATUS_MODULE_MEMORY, "memory: exhausted"},
        {HPS_POOL_INVALID_POINTER, -INT64_C(65539),
         HPS_STATUS_MODULE_MEMORY, "memory: invalid pointer"},
        {HPS_POOL_DOUBLE_FREE, -INT64_C(65540),
         HPS_STATUS_MODULE_MEMORY, "memory: double free"},
        {HPS_POOL_NOT_INITIALIZED, -INT64_C(65541),
         HPS_STATUS_MODULE_MEMORY, "memory: not initialized"},
        {HPS_POOL_ALREADY_INITIALIZED, -INT64_C(65542),
         HPS_STATUS_MODULE_MEMORY, "memory: already initialized"},
        {HPS_POOL_CORRUPT, -INT64_C(65543),
         HPS_STATUS_MODULE_MEMORY, "memory: corrupt"},
        {HPS_BYTE_INVALID_ARGUMENT, -INT64_C(131073),
         HPS_STATUS_MODULE_BYTES, "bytes: invalid argument"},
        {HPS_BYTE_OUT_OF_RANGE, -INT64_C(131074),
         HPS_STATUS_MODULE_BYTES, "bytes: out of range"},
        {HPS_BYTE_NO_SPACE, -INT64_C(131075),
         HPS_STATUS_MODULE_BYTES, "bytes: no space"},
        {HPS_BYTE_INVALID_STATE, -INT64_C(131076),
         HPS_STATUS_MODULE_BYTES, "bytes: invalid state"}
    };
    const size_t count = sizeof(cases) / sizeof(cases[0]);
    assert(sizeof(hps_status_t) == sizeof(int64_t));
    assert(sizeof(hps_byte_status) == sizeof(hps_status_t));
    assert(sizeof(hps_pool_status) == sizeof(hps_status_t));
    assert(HPS_STATUS_OK == INT64_C(0));
    assert(HPS_BYTE_OK == HPS_STATUS_OK && HPS_POOL_OK == HPS_STATUS_OK);
    assert(HPS_STATUS_MODULE_FRAMEWORK == UINT16_C(0));
    assert(HPS_STATUS_MODULE_MEMORY == UINT16_C(1));
    assert(HPS_STATUS_MODULE_BYTES == UINT16_C(2));
    assert(HPS_STATUS_MODULE_UNKNOWN == UINT16_MAX);
    assert(HPS_STATUS_MESSAGE_CAPACITY == (size_t)32U);
    const uint16_t success_module = hps_status_module(HPS_STATUS_OK);
    assert(success_module == HPS_STATUS_MODULE_FRAMEWORK);
    assert_status_message(HPS_STATUS_OK, "ok");

    for (size_t index = 0U; index < count; index += 1U) {
        const uint16_t module = hps_status_module(cases[index].status);
        assert(cases[index].status == cases[index].expected_value);
        assert(cases[index].status < HPS_STATUS_OK);
        assert(module == cases[index].module);
        assert_status_message(cases[index].status, cases[index].message);
        for (size_t other = index + 1U; other < count; other += 1U) {
            assert(cases[index].status != cases[other].status);
        }
    }
}

static void test_status_unregistered_and_boundaries(void)
{
    const module_case cases[] = {
        {-INT64_C(5), HPS_STATUS_MODULE_FRAMEWORK},
        {-INT64_C(65535), HPS_STATUS_MODULE_FRAMEWORK},
        {-INT64_C(65536), HPS_STATUS_MODULE_UNKNOWN},
        {-INT64_C(65544), HPS_STATUS_MODULE_MEMORY},
        {-INT64_C(131071), HPS_STATUS_MODULE_MEMORY},
        {-INT64_C(131072), HPS_STATUS_MODULE_UNKNOWN},
        {-INT64_C(131077), HPS_STATUS_MODULE_BYTES},
        {-INT64_C(196607), HPS_STATUS_MODULE_BYTES},
        {-INT64_C(196608), HPS_STATUS_MODULE_UNKNOWN},
        {-INT64_C(196609), HPS_STATUS_MODULE_UNKNOWN},
        {INT64_C(1), HPS_STATUS_MODULE_UNKNOWN},
        {INT64_C(65536), HPS_STATUS_MODULE_UNKNOWN},
        {INT64_MAX, HPS_STATUS_MODULE_UNKNOWN},
        {INT64_MIN, HPS_STATUS_MODULE_UNKNOWN}
    };
    const size_t count = sizeof(cases) / sizeof(cases[0]);
    for (size_t index = 0U; index < count; index += 1U) {
        const uint16_t module = hps_status_module(cases[index].status);
        assert(module == cases[index].module);
        assert_status_message(cases[index].status, "unknown status");
    }
}

static void test_status_diagnostic_lifetime(void)
{
    const char *success = hps_status_string(HPS_STATUS_OK);
    const char *bytes_error = hps_status_string(HPS_BYTE_INVALID_ARGUMENT);
    const char *memory_error = hps_status_string(HPS_POOL_CORRUPT);
    const char *unknown = hps_status_string(INT64_MIN);
    assert_status_message(HPS_STATUS_INVALID_STATE, "framework: invalid state");
    assert_status_message(HPS_POOL_ALREADY_INITIALIZED, "memory: already initialized");
    assert_status_message(HPS_BYTE_NO_SPACE, "bytes: no space");
    assert(strcmp(success, "ok") == 0);
    assert(strcmp(bytes_error, "bytes: invalid argument") == 0);
    assert(strcmp(memory_error, "memory: corrupt") == 0);
    assert(strcmp(unknown, "unknown status") == 0);
}

static void test_status_module_integration(void)
{
    const uint8_t payload[1] = {0xA5U};
    hps_byte_view view = {payload, 1U};
    hps_status_t status = hps_byte_view_init(&view, NULL, 1U);
    assert(status == HPS_BYTE_INVALID_ARGUMENT);
    assert(view.data == payload && view.length == 1U);
    uint16_t module = hps_status_module(status);
    assert(module == HPS_STATUS_MODULE_BYTES);
    assert_status_message(status, "bytes: invalid argument");

    uint8_t storage[1] = {0x5AU};
    hps_byte_buffer buffer = {NULL, 0U, 0U};
    status = hps_byte_buffer_init(&buffer, storage, 1U);
    assert(status == HPS_STATUS_OK);
    status = hps_byte_buffer_append_u8(&buffer, 0xA5U);
    assert(status == HPS_STATUS_OK && buffer.length == 1U);
    status = hps_byte_buffer_append_u8(&buffer, 0xFFU);
    assert(status == HPS_BYTE_NO_SPACE);
    assert(buffer.data == storage && buffer.length == 1U && buffer.capacity == 1U);
    assert(storage[0] == 0xA5U);
    module = hps_status_module(status);
    assert(module == HPS_STATUS_MODULE_BYTES);
    assert_status_message(status, "bytes: no space");

    /* This call checks its NULL output before pool initialization/state and does
     * not mutate the process-wide pool or depend on earlier suite execution. */
    const hps_pool_status last_status = hps_pool_last_status();
    status = hps_pool_get_stats(NULL);
    assert(status == HPS_POOL_INVALID_ARGUMENT);
    const hps_pool_status last_status_after = hps_pool_last_status();
    assert(last_status_after == last_status);
    module = hps_status_module(status);
    assert(module == HPS_STATUS_MODULE_MEMORY);
    assert_status_message(status, "memory: invalid argument");
}

int32_t test_status(void)
{
    test_status_registered_codes();
    test_status_unregistered_and_boundaries();
    test_status_diagnostic_lifetime();
    test_status_module_integration();
    return 0;
}
