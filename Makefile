MODULES := utils virtual_machine document_manager
EXECUTABLES := brix
ARCH := linux

TARGETS := $(EXECUTABLES:%=src/%)
SRC_DIRS := src src/arch/$(ARCH) $(MODULES:%=src/%) $(MODULES:%=src/arch/$(ARCH)/%)
CFLAGS := $(SRC_DIRS:%=-I%) -g -Wall -pedantic -Wno-variadic-macros
SOURCES := $(wildcard $(SRC_DIRS:%=%/*.c))
OBJECTS := $(filter-out $(TARGETS:%=%.o), $(SOURCES:.c=.o))
COMPILER_DIR := src/compiler

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

	
# Compiler 

.PHONY: compiler
compiler: $(COMPILER_DIR)/compiler.o $(COMPILER_DIR)/y.tab.o $(COMPILER_DIR)/lex.yy.o
	$(CC) -o $(COMPILER_DIR)/compiler $(COMPILER_DIR)/compiler.o $(COMPILER_DIR)/y.tab.o $(COMPILER_DIR)/lex.yy.o
	test -d $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)
	mv $(COMPILER_DIR)/compiler $(OUTPUT_DIR)/bxc

$(COMPILER_DIR)/compiler.o: $(COMPILER_DIR)/compiler.c
	$(CC) -c -o $(COMPILER_DIR)/compiler.o $(COMPILER_DIR)/compiler.c

$(COMPILER_DIR)/y.tab.o: $(COMPILER_DIR)/y.tab.c $(COMPILER_DIR)/y.tab.h
	$(CC) -c -o $(COMPILER_DIR)/y.tab.o $(COMPILER_DIR)/y.tab.c
	
$(COMPILER_DIR)/lex.yy.o: $(COMPILER_DIR)/lex.yy.c $(COMPILER_DIR)/y.tab.h
	$(CC) -c -o $(COMPILER_DIR)/lex.yy.o $(COMPILER_DIR)/lex.yy.c

$(COMPILER_DIR)/y.tab.h $(COMPILER_DIR)/y.tab.c: $(COMPILER_DIR)/language.y
	bison -d -y -v -b $(COMPILER_DIR)/y $(COMPILER_DIR)/language.y
	
$(COMPILER_DIR)/lex.yy.c: $(COMPILER_DIR)/language.l
	flex -o $(COMPILER_DIR)/lex.yy.c $(COMPILER_DIR)/language.l


# Unit Tests

.PHONY: test
test: $(TEST_TARGETS) 

.SECONDEXPANSION:
$(TEST_TARGETS): $(TEST_OBJECTS) $(OBJECTS) $$(@).o
	$(CC) $(TEST_CFLAGS) -o $@ $(TEST_OBJECTS) $(OBJECTS) $(@).o `pkg-config --cflags --libs check`
	./$(@)
	
.PHONY: test_compiler
test_compiler: test/compiler/test_compiler

test/compiler/test_compiler: test/compiler/test_compiler.o $(COMPILER_DIR)/y.tab.o $(COMPILER_DIR)/lex.yy.o
	$(CC) -o $(@) test/compiler/test_compiler.o $(COMPILER_DIR)/y.tab.o $(COMPILER_DIR)/lex.yy.o `pkg-config --cflags --libs check`
	./$(@)
	
test/compiler/test_compiler.o: test/compiler/test_compiler.c
	$(CC) -c -o $(@) test/compiler/test_compiler.c
	
	
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
	-rm -f $(OUTPUT_DIR)
	-rm -f $(TEST_TARGETS)
	-rm -f $(wildcard $(TEST_DIRS:%=%/*.d) $(TEST_DIRS:%=%/*.o))
	# Clean compiler
	-rm -f $(wildcard $(COMPILER_DIR)/*.o)
	-rm -f $(COMPILER_DIR)/y.tab.c
	-rm -f $(COMPILER_DIR)/lex.yy.c
	-rm -f $(wildcard $(COMPILER_DIR)/*.h)
	-rm -f $(wildcard $(COMPILER_DIR)/*.output)
	-rm -f $(wildcard $(COMPILER_DIR)/*.gch)
	-rm -f test/compiler/test_compiler.o
	-rm -f test/compiler/test_compiler
	