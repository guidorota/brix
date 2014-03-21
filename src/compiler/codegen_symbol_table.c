/*
 * codegen_symbol_table.c
 * Created on: Mar 4, 2014
 * Author: Guido Rota
 *
 * Copyright (c) 2014, Guido Rota
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation and/or 
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "logging.h"
#include "utils/memory_utils.h"
#include "utils/linked_list.h"
#include "compiler/codegen_symbol_table.h"

static bx_int8 field_identifier_equals(struct bx_comp_symbol *field_symbol, char *identifier) {
	return strncmp(field_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH) == 0 ? 1 : 0;
}

static bx_int8 variable_identifier_equals(struct bx_comp_symbol *variable_symbol, char *identifier) {
	return strncmp(variable_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH) == 0 ? 1 : 0;
}

static struct bx_comp_symbol *get_field_symbol(struct bx_comp_symbol_table *symbol_table, char *identifier);
static struct bx_comp_symbol *get_variable_symbol(struct bx_comp_symbol_table *symbol_table, char *identifier);

struct bx_comp_symbol_table *bx_cgsy_create_symbol_table() {
	bx_int8 error;
	struct bx_comp_symbol_table *symbol_table;

	symbol_table = BX_MALLOC(struct bx_comp_symbol_table);
	if (symbol_table == NULL) {
		return NULL;
	}

	memset((void *) symbol_table, 0, sizeof (struct bx_comp_symbol_table));
	error = bx_cgsy_scope_down(symbol_table);
	if (error != 0) {
		free(symbol_table);
		return NULL;
	}

	return symbol_table;
}

bx_int8 bx_cgsy_destroy_symbol_table(struct bx_comp_symbol_table *symbol_table) {
	struct bx_comp_field_symbol *field;
	struct bx_comp_variable_symbol *variable;
	struct bx_comp_scope *scope;

	if (symbol_table == NULL) {
		return -1;
	}

	while (symbol_table->field_list != NULL) {
		field = bx_llist_remove_head(&symbol_table->field_list);
		free(field);
	}

	while (symbol_table->scope_list != NULL) {
		scope = bx_llist_remove_head(&symbol_table->scope_list);
		while (scope->variable_list != NULL) {
			variable = bx_llist_remove_head(&scope->variable_list);
			free(variable);
		}
		free(scope);
	}

	free(symbol_table);
	return 0;
}

bx_int8 bx_cgsy_scope_down(struct bx_comp_symbol_table *symbol_table) {
	struct bx_comp_scope *child_scope;

	if (symbol_table == NULL) {
		return -1;
	}

	child_scope = BX_MALLOC(struct bx_comp_scope);
	if (child_scope == NULL) {
		return -1;
	}

	child_scope->parent_scope = symbol_table->current_scope;
	child_scope->variable_list = NULL;
	symbol_table->current_scope = child_scope;
	bx_llist_add(&symbol_table->scope_list, child_scope);

	return 0;
}

bx_int8 bx_cgsy_scope_up(struct bx_comp_symbol_table *symbol_table) {

	if (symbol_table == NULL) {
		return -1;
	}

	symbol_table->current_scope = symbol_table->current_scope->parent_scope;
	return 0;
}

bx_int8 bx_cgsy_add_field(struct bx_comp_symbol_table *symbol_table, char *identifier,
		enum bx_builtin_type data_type, enum bx_comp_creation_modifier creation_modifier) {
	struct bx_comp_symbol *field_symbol;
	struct bx_linked_list *node;

	if (symbol_table == NULL || identifier == NULL) {
		return -1;
	}

	if (bx_cgsy_get_symbol(symbol_table, identifier) != NULL) {
		BX_LOG(LOG_ERROR, "symbol_table", "Duplicate declaration of variable '%s'", identifier);
		return -1;
	}

	field_symbol = BX_MALLOC(struct bx_comp_symbol);
	if (field_symbol == NULL) {
		return -1;
	}

	field_symbol->data_type = data_type;
	field_symbol->symbol_data.creation_modifier = creation_modifier;
	field_symbol->symbol_type = BX_COMP_FIELD_SYMBOL;
	strncpy(field_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH);
	BX_LOG(LOG_DEBUG, "symbol_table", "Symbol %s added", identifier);

	node = bx_llist_add(&symbol_table->field_list, (void *) field_symbol);
	if (node == NULL) {
		return -1;
	}

	return 0;
}

bx_int8 bx_cgsy_add_variable(struct bx_comp_symbol_table *symbol_table, char *identifier,
		enum bx_builtin_type data_type) {
	struct bx_comp_scope *scope;
	struct bx_comp_symbol *variable_symbol;

	if (symbol_table == NULL || identifier == NULL) {
		return -1;
	}

	scope = symbol_table->current_scope;
	if (bx_llist_contains_equals(scope->variable_list,
			identifier, (bx_llist_equals) &variable_identifier_equals) == 1) {
		BX_LOG(LOG_ERROR, "symbol_table", "Duplicate variable declaration for '%s'", identifier);
		return -1;
	}

	if (bx_llist_contains_equals(symbol_table->field_list,
			identifier, (bx_llist_equals) &field_identifier_equals) == 1) {
		BX_LOG(LOG_ERROR, "symbol_table", "Duplicate use of identifier '%s'", identifier);
		return -1;
	}

	variable_symbol = BX_MALLOC(struct bx_comp_symbol);
	if (variable_symbol == NULL) {
		return -1;
	}

	variable_symbol->data_type = data_type;
	variable_symbol->symbol_data.variable_number = symbol_table->current_variable_number++;
	variable_symbol->symbol_type = BX_COMP_VARIABLE_SYMBOL;
	memcpy(variable_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH);
	bx_llist_add(&scope->variable_list, variable_symbol);

	return 0;
}

struct bx_comp_symbol *bx_cgsy_get_symbol(struct bx_comp_symbol_table *symbol_table, char *identifier) {
	struct bx_comp_symbol *symbol;

	if (symbol_table == NULL || identifier == NULL) {
		return NULL;
	}

	symbol = get_field_symbol(symbol_table, identifier);
	if (symbol != NULL) {
		return symbol;
	} else {
		return get_variable_symbol(symbol_table, identifier);
	}
}

static struct bx_comp_symbol *get_field_symbol(struct bx_comp_symbol_table *symbol_table, char *identifier) {
	return bx_llist_find_equals(symbol_table->field_list,
			(void *) identifier, (bx_llist_equals) &field_identifier_equals);
}

static struct bx_comp_symbol *get_variable_symbol(struct bx_comp_symbol_table *symbol_table, char *identifier) {
	struct bx_comp_scope *current_scope;
	struct bx_comp_symbol *variable_symbol;

	current_scope = symbol_table->current_scope;
	while (current_scope != NULL) {
		variable_symbol = bx_llist_find_equals(
				current_scope->variable_list, identifier, (bx_llist_equals) &variable_identifier_equals);
		if (variable_symbol != NULL) {
			return variable_symbol;
		}
		current_scope = current_scope->parent_scope;
	}

	return NULL;
}


