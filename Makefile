CC ?= cc
PYTHON ?= python3
CPPFLAGS += -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic
BUILD_DIR ?= build/make
HPS_BUILD_HOSTED_DEBUG ?= 0
.PHONY: all test clean
TEST_SOURCES := tests/main.c tests/base/test_base.c tests/base/test_standard_types.c tests/memory/test_pool.c tests/memory/test_memory_operations.c tests/memory/test_memory_clone.c tests/bytes/test_bytes.c tests/bytes/test_reader.c tests/enum/test_enum.c tests/string/test_string.c tests/string/test_string_extended.c tests/string/test_string_builder.c tests/data_structure/test_span.c tests/data_structure/test_data_structure.c tests/cstr/test_cstr.c tests/cstr/test_cstr_convert.c tests/cstr/test_cstr_format.c tests/validation/test_national_code.c tests/enum/test_cstr_enum.c tests/base/test_macro_utilities.c tests/memory/test_memory_utilities.c tests/result/test_result.c tests/data_structure/test_collection_operations.c tests/string/test_string_tokenizer.c tests/string/test_string_indent.c tests/json/test_json.c tests/bytes/test_byte_algorithms.c tests/data_structure/test_optional.c tests/cstr/test_cstr_bool.c
CORE_SOURCES := src/memory/pool.c src/memory/operations.c src/memory/array.c src/bytes/view.c src/bytes/buffer.c src/bytes/codec.c src/bytes/endian.c src/bytes/reader.c src/string/view.c src/string/buffer.c src/string/char.c src/string/builder.c src/string/policy.c src/cstr/core.c src/cstr/convert.c src/cstr/algorithm.c src/cstr/format.c src/validation/national_code.c src/string/tokenizer.c src/string/indent.c src/json/tokenizer.c src/json/reader.c src/json/writer.c src/json/string.c
TEST_HEADERS := tests/test_suites.h tests/string/policy_expectations.h src/internal/text.h src/json/internal.h $(shell find include/hps -name '*.h')
ifneq ($(HPS_BUILD_HOSTED_DEBUG),0)
ifneq ($(HPS_BUILD_HOSTED_DEBUG),1)
$(error HPS_BUILD_HOSTED_DEBUG must be 0 or 1)
endif
endif
TEST_EXECUTABLE := $(BUILD_DIR)/hps_tests
ADAPTER_SOURCES :=
ifeq ($(HPS_BUILD_HOSTED_DEBUG),1)
TEST_EXECUTABLE := $(BUILD_DIR)/hps_tests_hosted
TEST_SOURCES += tests/platform/test_hosted_debug.c
ADAPTER_SOURCES := src/platform/hosted/debug.c
CPPFLAGS += -DHPS_TEST_HOSTED_DEBUG=1
endif
all: $(TEST_EXECUTABLE)
$(TEST_EXECUTABLE): $(TEST_SOURCES) $(CORE_SOURCES) $(ADAPTER_SOURCES) $(TEST_HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -UNDEBUG $(TEST_SOURCES) $(CORE_SOURCES) $(ADAPTER_SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

test: all
	$(TEST_EXECUTABLE)
	$(PYTHON) tests/base/check_portability.py --compiler "$(CC)"
	$(PYTHON) tests/base/check_standard_types.py --compiler "$(CC)"
	$(PYTHON) tests/base/check_headers.py --compiler "$(CC)"
	$(PYTHON) tests/base/check_detection.py --compiler "$(CC)"
	$(PYTHON) tests/base/check_attributes.py --compiler "$(CC)"
	$(PYTHON) tests/json/check_json.py --compiler "$(CC)"
ifeq ($(HPS_BUILD_HOSTED_DEBUG),1)
	$(PYTHON) tests/platform/check_hosted_debug.py --compiler "$(CC)"
endif

clean:
	rm -rf $(BUILD_DIR)
