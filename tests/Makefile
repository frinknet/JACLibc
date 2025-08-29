# JACLibc Test Suite
CC ?= gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I../include
LDFLAGS = -lpthread

# Library configuration (from parent)
LIB_NAME = jaclibc
LIB_TARGET = lib$(LIB_NAME).a

# Standard installation directories
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DESTDIR ?=

# Platform-specific adjustments
UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -D_GNU_SOURCE
endif

# Alpine container detection
ALPINE_CONTAINER := $(shell [ -f /etc/alpine-release ] && echo "yes" || echo "no")
ifeq ($(ALPINE_CONTAINER),yes)
	CFLAGS += -D_GNU_SOURCE
	LDFLAGS += -static-libgcc
endif

# Source files
UNIT_SRCS = $(wildcard unit/*.c)
CASE_SRCS = $(wildcard case/*.c)
BENCH_SRCS = $(wildcard bench/*.c)

# Installed binary names
UNIT_BINS = $(patsubst unit/%.c,$(DESTDIR)$(BINDIR)/test-unit-%,$(UNIT_SRCS))
CASE_BINS = $(patsubst case/%.c,$(DESTDIR)$(BINDIR)/test-case-%,$(CASE_SRCS))
BENCH_BINS = $(patsubst bench/%.c,$(DESTDIR)$(BINDIR)/test-bench-%,$(BENCH_SRCS))
ALL_BINS = $(UNIT_BINS) $(CASE_BINS) $(BENCH_BINS)

# Build info
$(info Building with: $(CC))
$(info Platform: $(UNAME_S))

# Default target
all: unit case bench

# Build test categories
unit: $(UNIT_BINS)
case: $(CASE_BINS)	
bench: $(BENCH_BINS)

# Create installation directory
$(DESTDIR)$(BINDIR):
	@mkdir -p $@

# Build and copy library to BINDIR for proper cleanup
$(DESTDIR)$(BINDIR)/$(LIB_TARGET): | $(DESTDIR)$(BINDIR)
	@$(MAKE) -C .. $(LIB_TARGET)
	@mv ../$(LIB_TARGET) $@

# Pattern rules for building and installing tests - NOW WITH ACTUAL LINKING
$(DESTDIR)$(BINDIR)/test-unit-%: unit/%.c runner.c runner.h $(DESTDIR)$(BINDIR)/$(LIB_TARGET) | $(DESTDIR)$(BINDIR)
	@$(CC) $(CFLAGS) $< runner.c -L$(DESTDIR)$(BINDIR) -l$(LIB_NAME) -o $@ $(LDFLAGS)

$(DESTDIR)$(BINDIR)/test-case-%: case/%.c runner.c runner.h $(DESTDIR)$(BINDIR)/$(LIB_TARGET) | $(DESTDIR)$(BINDIR)
	@$(CC) $(CFLAGS) $< runner.c -L$(DESTDIR)$(BINDIR) -l$(LIB_NAME) -o $@ $(LDFLAGS)

$(DESTDIR)$(BINDIR)/test-bench-%: bench/%.c runner.c runner.h $(DESTDIR)$(BINDIR)/$(LIB_TARGET) | $(DESTDIR)$(BINDIR)
	@$(CC) $(CFLAGS) $< runner.c -L$(DESTDIR)$(BINDIR) -l$(LIB_NAME) -o $@ $(LDFLAGS)

# Pattern rules to run individual tests (build-then-run)
test-unit-%: $(DESTDIR)$(BINDIR)/test-unit-%
	@echo "Running unit test: $*"
	@echo $<
	@$<

test-case-%: $(DESTDIR)$(BINDIR)/test-case-%
	@echo "Running integration test: $*"
	@echo $<
	@$<

test-bench-%: $(DESTDIR)$(BINDIR)/test-bench-%
	@echo "Running benchmark: $*"
	@echo $<
	@$<

# Run test categories (all tests in category)
test-unit: $(UNIT_BINS)
	@echo "=== Running Unit Tests ==="
	@passed=0; failed=0; \
	for test in $(UNIT_BINS); do \
		echo ""; \
		if $$test; then \
			passed=$$((passed + 1)); \
		else \
			failed=$$((failed + 1)); \
		fi; \
	done; \
	echo ""; \
	echo "Unit Tests: $$passed passed, $$failed failed"

test-case: $(CASE_BINS)
	@echo "=== Running Integration Cases ==="
	@passed=0; failed=0; \
	for test in $(CASE_BINS); do \
		echo ""; \
		if $$test; then \
			passed=$$((passed + 1)); \
		else \
			failed=$$((failed + 1)); \
		fi; \
	done; \
	echo ""; \
	echo "Integration Cases: $$passed passed, $$failed failed"

test-bench: $(BENCH_BINS)
	@echo "=== Running Benchmarks ==="
	@for test in $(BENCH_BINS); do \
		echo ""; \
		$$test; \
	done

# Run all tests
test: test-unit test-case
	@echo ""
	@echo "🎉 All test categories completed!"

# Individual test targets
unit-only: unit
case-only: case
bench-only: bench

# Install target (builds everything)
install: all
	@echo "Tests installed to $(DESTDIR)$(BINDIR)"

# Clean up - remove installed binaries AND library
clean:
	@echo "Removing test binaries and library from $(DESTDIR)$(BINDIR)"
	@rm -f $(DESTDIR)$(BINDIR)/test-unit-*
	@rm -f $(DESTDIR)$(BINDIR)/test-case-*
	@rm -f $(DESTDIR)$(BINDIR)/test-bench-*
	@rm -f $(DESTDIR)$(BINDIR)/$(LIB_TARGET)

# Help
help:
	@echo "JACLibc Test Suite"
	@echo ""
	@echo "Installation: $(DESTDIR)$(BINDIR)"
	@echo ""
	@echo "Targets:"
	@echo "  all		- Build and install all tests"
	@echo "  unit		- Build and install unit tests"
	@echo "  case		- Build and install integration cases"
	@echo "  bench		- Build and install benchmarks"
	@echo "  install	- Install all tests (same as 'all')"
	@echo "  test		- Run unit and integration tests"
	@echo "  test-unit	- Run unit tests only"
	@echo "  test-case	- Run integration tests only"
	@echo "  test-bench - Run benchmarks only"
	@echo "  clean		- Remove installed test binaries"
	@echo ""
	@echo "Individual test usage:"
	@echo "  test-unit-strings	 - Build and run strings tests"
	@echo "  test-case-example	- Build and run example integration test"
	@echo "  test-unit-string	 - Build and run string tests"
	@echo "  test-unit-stdlib	 - Build and run stdlib tests"
	@echo ""
	@echo "Configuration:"
	@echo "  PREFIX=/path	 - Change install prefix (default: /usr/local)"
	@echo "  DESTDIR=/path	 - Stage installation for packaging"

.PHONY: all unit case bench test test-unit test-case test-bench unit-only case-only bench-only install clean help test-unit-% test-case-% test-bench-%

