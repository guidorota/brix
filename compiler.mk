COMPILER_DIR := src/compiler

.PHONY: all
all: compiler

.PHONY: compiler
compiler: $(COMPILER_DIR)/y.tab.h $(COMPILER_DIR)/y.tab.c $(COMPILER_DIR)/lex.yy.h $(COMPILER_DIR)/lex.yy.y $(COMPILER_DIR)/lex.yy.c 

$(COMPILER_DIR)/y.tab.h $(COMPILER_DIR)/y.tab.c: $(COMPILER_DIR)/language.y
	bison -d -y -v -b $(COMPILER_DIR)/y $(COMPILER_DIR)/language.y
	
$(COMPILER_DIR)/lex.yy.h $(COMPILER_DIR)/lex.yy.y $(COMPILER_DIR)/lex.yy.c: $(COMPILER_DIR)/language.l
	flex --header-file=$(COMPILER_DIR)/lex.yy.h -o $(COMPILER_DIR)/lex.yy.c $(COMPILER_DIR)/language.l

	
.PHONY:
clean:
	-rm -f $(COMPILER_DIR)/y.tab.c
	-rm -f $(COMPILER_DIR)/y.tab.h
	-rm -f $(COMPILER_DIR)/lex.yy.c
	-rm -f $(COMPILER_DIR)/lex.yy.h
	-rm -f $(wildcard $(COMPILER_DIR)/*.output)
	-rm -f $(wildcard $(COMPILER_DIR)/*.gch)