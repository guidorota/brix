MODULES := 
EXECUTABLES := brix
ARCH := linux

TARGETS := $(EXECUTABLES:%=src/arch/$(ARCH)/%)
SRC_DIRS := src src/arch/$(ARCH) $(MODULES:%=src/%) $(MODULES:%=src/$(ARCH)/%)
CFLAGS := $(SRC_DIRS:%=-I%) -g -Wall -pedantic -Wno-variadic-macros
SOURCES := $(wildcard $(SRC_DIRS:%=%/*.c))
OBJECTS := $(filter-out $(TARGETS:%=%.o), $(SOURCES:.c=.o))
COMPILER_DIR := src/compiler

TEST_TARGETS := test/ha_test
TEST_DIRS := test test/$(ARCH) $(MODULES:%=test/%) $(MODULES:%=test/$(ARCH)/%)
TEST_SOURCES := $(wildcard $(TEST_DIRS:%=%/*.c))
TEST_OBJECTS := $(filter-out $(TEST_TARGETS:%=%.o), $(TEST_SOURCES:.c=.o))
TEST_CFLAGS := $(TEST_DIRS:%=-I%) $(CFLAGS)

OUTPUT_DIR := out

.SUFFIXES:
.SUFFIXES: .o .c

# System

.PHONY: all
all: targets compiler

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
	gcc -o $(COMPILER_DIR)/compiler $(COMPILER_DIR)/compiler.o $(COMPILER_DIR)/y.tab.o $(COMPILER_DIR)/lex.yy.o
	test -d $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)
	mv $(COMPILER_DIR)/compiler $(OUTPUT_DIR)

$(COMPILER_DIR)/compiler.o: $(COMPILER_DIR)/compiler.c
	gcc -c -o $(COMPILER_DIR)/compiler.o $(COMPILER_DIR)/compiler.c

$(COMPILER_DIR)/y.tab.o: $(COMPILER_DIR)/y.tab.c $(COMPILER_DIR)/y.tab.h
	gcc -c -o $(COMPILER_DIR)/y.tab.o $(COMPILER_DIR)/y.tab.c
	
$(COMPILER_DIR)/lex.yy.o: $(COMPILER_DIR)/lex.yy.c $(COMPILER_DIR)/y.tab.h
	gcc -c -o $(COMPILER_DIR)/lex.yy.o $(COMPILER_DIR)/lex.yy.c

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
	-rm -f $(wildcard $(COMPILER_DIR)/*.o)
	-rm -f $(COMPILER_DIR)/y.tab.c
	-rm -f $(COMPILER_DIR)/lex.yy.c
	-rm -f $(wildcard $(COMPILER_DIR)/*.h)
	-rm -f $(wildcard $(COMPILER_DIR)/*.output)
	-rm -f $(wildcard $(COMPILER_DIR)/*.gch)
	