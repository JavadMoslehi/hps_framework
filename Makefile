CC ?= cc
PYTHON ?= python3
CPPFLAGS += -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic
BUILD_DIR ?= build/make
.PHONY: all test clean
all: $(BUILD_DIR)/hps_base_test $(BUILD_DIR)/hps_pool_test $(BUILD_DIR)/hps_bytes_test
$(BUILD_DIR)/hps_bytes_test: tests/bytes/test_bytes.c src/bytes/view.c src/bytes/buffer.c include/hps/bytes/view.h include/hps/bytes/buffer.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -UNDEBUG tests/bytes/test_bytes.c src/bytes/view.c src/bytes/buffer.c $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/hps_pool_test: tests/memory/test_pool.c src/memory/pool.c include/hps/memory/pool.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -UNDEBUG tests/memory/test_pool.c src/memory/pool.c $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/hps_base_test: tests/base/test_base.c $(shell find include/hps -name '*.h')
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

test: all
	$(BUILD_DIR)/hps_bytes_test
	$(BUILD_DIR)/hps_pool_test
	$(BUILD_DIR)/hps_base_test
	$(PYTHON) tests/base/check_portability.py --compiler "$(CC)"

clean:
	rm -rf $(BUILD_DIR)
