.PHONY: all clean

LLVM_CONFIG ?= llvm-config
CXX         ?= clang++
CC          ?= clang

CXXFLAGS = -fPIC -std=c++17 -Wall -Wextra \
           $(shell $(LLVM_CONFIG) --cxxflags)

LDFLAGS = -shared \
          $(shell $(LLVM_CONFIG) --ldflags --libs core passes support)

BUILD   = build
PLUGIN  = $(BUILD)/Asgn1.so

SOURCES = \
    PassPlugin.cpp \
    AlgebraicIdentity.cpp \
    StrengthReduction.cpp \
    MultiInstructionOtp.cpp

# ─────────────────────────────────────────
# Top-level target
# ─────────────────────────────────────────
all: $(BUILD)/testOpt.ll

# ─────────────────────────────────────────
# Build pass plugin
# ─────────────────────────────────────────
$(PLUGIN): $(SOURCES) Passes.h | $(BUILD)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCES) -o $(PLUGIN)

# ─────────────────────────────────────────
# IR pipeline
# ─────────────────────────────────────────

# 1. C → LLVM IR (text)
$(BUILD)/test.ll: test/test.c | $(BUILD)
	$(CC) -S -emit-llvm -O0 -Xclang -disable-O0-optnone $< -o $@

# 2. mem2reg (clean IR, still text)
$(BUILD)/test_clean.ll: $(BUILD)/test.ll
	opt -passes=mem2reg -S $< -o $@

# 3. Run custom passes
$(BUILD)/testOpt.ll: $(BUILD)/test_clean.ll $(PLUGIN)
	opt -load-pass-plugin $(PLUGIN) \
	    -passes="algebraic-identity,strength-reduction,multi-inst-opt" \
	    -S $< -o $@

# ─────────────────────────────────────────
# Build directory
# ─────────────────────────────────────────
$(BUILD):
	mkdir -p $(BUILD)

# ─────────────────────────────────────────
# Clean
# ─────────────────────────────────────────
clean:
	rm -rf $(BUILD)