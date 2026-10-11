CC ?= cc
PYTHON ?= python3
CPPFLAGS += -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic
BUILD_DIR ?= build/make

.PHONY: all test clean
all: $(BUILD_DIR)/hps_base_test

$(BUILD_DIR)/hps_base_test: tests/base/test_base.c $(shell find include/hps -name '*.h')
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

test: all
	$(BUILD_DIR)/hps_base_test
	$(PYTHON) tests/base/check_portability.py --compiler "$(CC)"

clean:
	rm -rf $(BUILD_DIR)
