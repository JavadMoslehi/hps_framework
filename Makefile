CC ?= cc
PYTHON ?= python3
CPPFLAGS += -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic
BUILD_DIR ?= build/make
.PHONY: all test clean
TEST_SOURCES := tests/main.c tests/base/test_base.c tests/memory/test_pool.c tests/bytes/test_bytes.c tests/bytes/test_reader.c
CORE_SOURCES := src/memory/pool.c src/bytes/view.c src/bytes/buffer.c src/bytes/endian.c src/bytes/reader.c
TEST_HEADERS := tests/test_suites.h $(shell find include/hps -name '*.h')
all: $(BUILD_DIR)/hps_tests
$(BUILD_DIR)/hps_tests: $(TEST_SOURCES) $(CORE_SOURCES) $(TEST_HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -UNDEBUG $(TEST_SOURCES) $(CORE_SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

test: all
	$(BUILD_DIR)/hps_tests
	$(PYTHON) tests/base/check_portability.py --compiler "$(CC)"

clean:
	rm -rf $(BUILD_DIR)
