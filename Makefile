MODULES := utils virtual_machine document_manager compiler
EXECUTABLES := brix compiler/compiler
ARCH := linux

TARGETS := $(EXECUTABLES:%=src/%)
SRC_DIRS := src src/arch/$(ARCH) $(MODULES:%=src/%) $(MODULES:%=src/arch/$(ARCH)/%)
CFLAGS := $(SRC_DIRS:%=-I%) -g -Wall -pedantic -Wno-variadic-macros
SOURCES := $(wildcard $(SRC_DIRS:%=%/*.c))
OBJECTS := $(filter-out $(TARGETS:%=%.o), $(SOURCES:.c=.o))

TEST_TARGETS := test/test
TEST_DIRS := test test/arch/$(ARCH) $(MODULES:%=test/%) $(MODULES:%=test/arch/$(ARCH)/%)
TEST_SOURCES := $(wildcard $(TEST_DIRS:%=%/*.c))
TEST_OBJECTS := $(filter-out $(TEST_TARGETS:%=%.o), $(TEST_SOURCES:.c=.o))
TEST_CFLAGS := $(TEST_DIRS:%=-I%) $(CFLAGS)

OUTPUT_DIR := out

.SUFFIXES:
.SUFFIXES: .o .c

# System

.PHONY: all
all: targets

.PHONY: targets
targets: $(TARGETS)
	test -d $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)
	mv $(TARGETS) $(OUTPUT_DIR)

.SECONDEXPANSION:
$(TARGETS): $(OBJECTS) $$(@).o
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(@).o

# Unit Tests

.PHONY: test
test: $(TEST_TARGETS) 

.SECONDEXPANSION:
$(TEST_TARGETS): $(TEST_OBJECTS) $(OBJECTS) $$(@).o
	$(CC) $(TEST_CFLAGS) -o $@ $(TEST_OBJECTS) $(OBJECTS) $(@).o `pkg-config --cflags --libs check`
	#./$(@)
	
include $(SOURCES:.c=.d)
include $(TEST_SOURCES:.c=.d)

%.d: %.c
	bash depend.sh $(CC) $^ $(CFLAGS) > $@

# Clean

.PHONY: clean
clean:
	-rm -f $(OUTPUT_DIR)/*
	-rmdir $(OUTPUT_DIR)
	-rm -f $(wildcard $(SRC_DIRS:%=%/*.d) $(SRC_DIRS:%=%/*.o))
	-rm -f $(TEST_TARGETS)
	-rm -f $(wildcard $(TEST_DIRS:%=%/*.d) $(TEST_DIRS:%=%/*.o))
	-rm -f $(wildcard $(COMPILER_DIR)/*.o)

	