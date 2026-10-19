CC ?= cc
PYTHON ?= python3
CPPFLAGS += -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic
BUILD_DIR ?= build/make
.PHONY: all test clean
TEST_SOURCES := tests/main.c tests/base/test_base.c tests/memory/test_pool.c tests/memory/test_memory_operations.c tests/memory/test_memory_clone.c tests/bytes/test_bytes.c tests/bytes/test_reader.c tests/enum/test_enum.c tests/string/test_string.c tests/string/test_string_extended.c tests/string/test_string_builder.c tests/span/test_span.c tests/data_structure/test_data_structure.c tests/cstr/test_cstr.c tests/cstr/test_cstr_convert.c tests/cstr/test_cstr_format.c tests/validation/test_national_code.c tests/enum/test_cstr_enum.c
CORE_SOURCES := src/memory/pool.c src/memory/operations.c src/bytes/view.c src/bytes/buffer.c src/bytes/codec.c src/bytes/endian.c src/bytes/reader.c src/string/view.c src/string/buffer.c src/string/char.c src/string/builder.c src/string/policy.c src/cstr/core.c src/cstr/convert.c src/cstr/algorithm.c src/cstr/format.c src/validation/national_code.c
TEST_HEADERS := tests/test_suites.h tests/string/policy_expectations.h src/internal/text.h $(shell find include/hps -name '*.h')
all: $(BUILD_DIR)/hps_tests
$(BUILD_DIR)/hps_tests: $(TEST_SOURCES) $(CORE_SOURCES) $(TEST_HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -UNDEBUG $(TEST_SOURCES) $(CORE_SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

test: all
	$(BUILD_DIR)/hps_tests
	$(PYTHON) tests/base/check_portability.py --compiler "$(CC)"
	$(PYTHON) tests/base/check_detection.py --compiler "$(CC)"

clean:
	rm -rf $(BUILD_DIR)
