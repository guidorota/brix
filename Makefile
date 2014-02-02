MODULES := language
EXECUTABLES := brix
ARCH := linux

TARGETS := $(EXECUTABLES:%=src/arch/$(ARCH)/%)
SRC_DIRS := src src/$(ARCH) $(MODULES:%=src/%) $(MODULES:%=src/$(ARCH)/%)
CFLAGS := $(SRC_DIRS:%=-I%) -g -Wall -pedantic -Wno-variadic-macros
SOURCES := $(wildcard $(SRC_DIRS:%=%/*.c))
OBJECTS := $(filter-out $(TARGETS:%=%.o), $(SOURCES:.c=.o))

TEST_TARGETS := test/ha_test
TEST_DIRS := test test/$(ARCH) $(MODULES:%=test/%) $(MODULES:%=test/$(ARCH)/%)
TEST_SOURCES := $(wildcard $(TEST_DIRS:%=%/*.c))
TEST_OBJECTS := $(filter-out $(TEST_TARGETS:%=%.o), $(TEST_SOURCES:.c=.o))
TEST_CFLAGS := $(TEST_DIRS:%=-I%) $(CFLAGS)

OUTPUT_DIR := out

.SUFFIXES:
.SUFFIXES: .o .c

.PHONY: all
all: targets

.PHONY: targets
targets: $(TARGETS)
	test -d $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)
	mv $(TARGETS) $(OUTPUT_DIR)

.SECONDEXPANSION:
$(TARGETS): $(OBJECTS) $$(@).o
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(@).o
	
.PHONY: test
test: $(TEST_TARGETS)

.SECONDEXPANSION:
$(TEST_TARGETS): $(TEST_OBJECTS) $(OBJECTS) $$(@).o
	$(CC) $(TEST_CFLAGS) -o $@ $(TEST_OBJECTS) $(OBJECTS) $(@).o `pkg-config --cflags --libs check`
	./$(@)
	
include $(SOURCES:.c=.d)
include $(TEST_SOURCES:.c=.d)

%.d: %.c
	bash depend.sh $(CC) $^ $(CFLAGS) > $@

.PHONY: clean
clean:
	-rm -f $(OUTPUT_DIR)/*
	-rmdir $(OUTPUT_DIR)
	-rm -f $(wildcard $(SRC_DIRS:%=%/*.d) $(SRC_DIRS:%=%/*.o))
	-rm -f $(OUTPUT_DIR)
	-rm -f $(TEST_TARGETS)
	-rm -f $(wildcard $(TEST_DIRS:%=%/*.d) $(TEST_DIRS:%=%/*.o))
	