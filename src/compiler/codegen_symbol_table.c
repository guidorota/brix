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

static struct bx_linked_list *fiedl_symbol_list;

static bx_int8 field_identifier_equals(struct bx_comp_field_symbol *field_symbol, char *identifier) {
	return strncmp(field_symbol->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH) == 0 ? 1 : 0;
}

bx_int8 bx_cgsy_init() {

	if (bx_llist_size(fiedl_symbol_list) != 0) {
		return bx_cgsy_reset();
	}

	return 0;
}

bx_int8 bx_cgsy_add_field(char *identifier, enum bx_builtin_type data_type, enum bx_comp_creation_modifier creation_modifier) {
	struct bx_comp_field_symbol *field;
	struct bx_linked_list *node;

	if (identifier == NULL) {
		return -1;
	}

	if (bx_llist_contains_equals(fiedl_symbol_list, identifier, (bx_llist_equals) &field_identifier_equals) == 1) {
		BX_LOG(LOG_ERROR, "symbol_table", "Duplicate variable %s", identifier);
		return -1;
	}

	field = BX_MALLOC(struct bx_comp_field_symbol);
	if (field == NULL) {
		return -1;
	}

	field->data_type = data_type;
	field->creation_modifier = creation_modifier;
	strncpy(field->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH);
	BX_LOG(LOG_DEBUG, "symbol_table", "Symbol %s added", identifier);

	node = bx_llist_add(&fiedl_symbol_list, (void *) field);
	if (node == NULL) {
		return -1;
	}

	return 0;
}

struct bx_comp_field_symbol *bx_cgsy_get_field(char *identifier) {

	if (identifier == NULL) {
		return NULL;
	}

	return bx_llist_find_equals(fiedl_symbol_list, (void *) identifier, (bx_llist_equals) &field_identifier_equals);
}

bx_int8 bx_cgsy_reset() {
	struct bx_comp_field_symbol *symbol;

	while (fiedl_symbol_list != NULL) {
		symbol = (struct bx_comp_field_symbol *) bx_llist_remove_head(&fiedl_symbol_list);
		free(symbol);
	}

	return 0;
}
