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

static struct bx_linked_list *field_symbol_list;

static bx_uint16 current_variable_number;

static bx_int8 field_identifier_equals(struct bx_comp_field_symbol *field_symbol, char *identifier) {
	return strncmp(field_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH) == 0 ? 1 : 0;
}

static bx_int8 variable_identifier_equals(struct bx_comp_variable_symbol *variable_symbol, char *identifier) {
	return strncmp(variable_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH) == 0 ? 1 : 0;
}

bx_int8 bx_cgsy_init() {
	bx_int8 error = 0;

	current_variable_number = 0;
	if (bx_llist_size(field_symbol_list) != 0) {
		error = bx_cgsy_reset();
	}

	return error;
}

bx_int8 bx_cgsy_add_field(char *identifier, enum bx_builtin_type data_type, enum bx_comp_creation_modifier creation_modifier) {
	struct bx_comp_field_symbol *field_symbol;
	struct bx_linked_list *node;

	if (identifier == NULL) {
		return -1;
	}

	if (bx_llist_contains_equals(field_symbol_list, identifier, (bx_llist_equals) &field_identifier_equals) == 1) {
		BX_LOG(LOG_ERROR, "symbol_table", "Duplicate variable %s", identifier);
		return -1;
	}

	field_symbol = BX_MALLOC(struct bx_comp_field_symbol);
	if (field_symbol == NULL) {
		return -1;
	}

	field_symbol->data_type = data_type;
	field_symbol->creation_modifier = creation_modifier;
	strncpy(field_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH);
	BX_LOG(LOG_DEBUG, "symbol_table", "Symbol %s added", identifier);

	node = bx_llist_add(&field_symbol_list, (void *) field_symbol);
	if (node == NULL) {
		return -1;
	}

	return 0;
}

struct bx_comp_field_symbol *bx_cgsy_get_field(char *identifier) {

	if (identifier == NULL) {
		return NULL;
	}

	return bx_llist_find_equals(field_symbol_list, (void *) identifier, (bx_llist_equals) &field_identifier_equals);
}

struct bx_comp_variable_scope *bx_cgsy_create_variable_scope(struct bx_comp_variable_scope *parent) {
	struct bx_comp_variable_scope *variable_scope;

	variable_scope = BX_MALLOC(struct bx_comp_variable_scope);
	if (variable_scope == NULL) {
		return NULL;
	}
	variable_scope->variable_list = NULL;
	variable_scope->parent = parent;

	return variable_scope;
}

bx_int8 bx_cgsy_destroy_variable_scope(struct bx_comp_variable_scope *variable_scope) {
	struct bx_linked_list *variable_list;
	struct bx_comp_variable_symbol *variable_symbol;

	if (variable_scope == NULL) {
		return -1;
	}

	variable_list = variable_scope->variable_list;
	while(variable_list != NULL) {
		variable_symbol = bx_llist_remove_head(&variable_list);
		free(variable_symbol);
	}
	free(variable_scope);

	return 0;
}

bx_int8 bx_cgsy_add_variable(char *identifier, enum bx_builtin_type data_type,
		struct bx_comp_variable_scope *scope) {
	struct bx_comp_variable_symbol *variable_symbol;

	if (identifier == NULL || scope == NULL) {
		return -1;
	}

	if (bx_llist_contains_equals(scope->variable_list, identifier, (bx_llist_equals) &variable_identifier_equals) == 1) {
		BX_LOG(LOG_ERROR, "symbol_table", "Duplicate variable %s", identifier);
		return -1;
	}

	variable_symbol = BX_MALLOC(struct bx_comp_variable_symbol);
	if (variable_symbol == NULL) {
		return -1;
	}

	variable_symbol->data_type = data_type;
	variable_symbol->variable_number = current_variable_number++;
	memcpy(variable_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH);

	return 0;
}

struct bx_comp_variable_symbol *bx_cgsy_get_variable(struct bx_comp_variable_scope *scope, char *identifier) {
	struct bx_comp_variable_scope *current_scope;
	struct bx_comp_variable_symbol *variable_symbol;

	if (scope == NULL || identifier == NULL) {
		return NULL;
	}

	current_scope = scope;
	while (current_scope != NULL) {
		variable_symbol = bx_llist_find_equals(
				current_scope->variable_list, identifier, (bx_llist_equals) &variable_identifier_equals);
		if (variable_symbol != NULL) {
			return variable_symbol;
		}
		current_scope = current_scope->parent;
	}

	return NULL;
}

bx_int8 bx_cgsy_reset() {
	struct bx_comp_field_symbol *symbol;

	while (field_symbol_list != NULL) {
		symbol = (struct bx_comp_field_symbol *) bx_llist_remove_head(&field_symbol_list);
		free(symbol);
	}

	return 0;
}
