/*
 * document_manager.c
 * Created on: Feb 21, 2014
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

#include <string.h>
#include "logging.h"
#include "utils/list.h"
#include "document_manager/document_manager.h"

struct internal_field {
	char identifier[DM_FIELD_IDENTIFIER_LENGTH];
	struct bx_document_field field;
};

static struct bx_document_manager {
	struct internal_field field_list_storage[DM_MAX_FIELD_NUMBER];
	struct bx_list *field_list;
} document_manager;

bx_boolean compare_by_id(struct internal_field *field, char *identifier);

bx_int8 bx_docman_init() {
	BX_LOG(LOG_INFO, "document_manager", "Initializing document manager...");

	document_manager.field_list = bx_list_init(document_manager.field_list_storage,
			DM_MAX_FIELD_NUMBER, sizeof (struct internal_field));
	if (document_manager.field_list == NULL) {
		return -1;
	}

	return 0;
}

bx_int8 bx_docman_add_field(struct bx_document_field *field, char *identifier) {
	struct internal_field *internal_field;

	if (field == NULL) {
		return -1;
	}

	if (bx_list_indexof(document_manager.field_list, identifier, (equals_function) compare_by_id) != -1) {
		return -1;
	}

	internal_field = bx_list_get_empty(document_manager.field_list);
	if (internal_field == NULL) {
		return -1;
	}
	memcpy(&internal_field->field, field, sizeof (struct bx_document_field));
	strncpy(internal_field->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH);

	return 0;
}

bx_int8 bx_docman_invoke_get(char *field_identifier, void *data) {
	struct internal_field *internal_field;

	if (field_identifier == NULL) {
		return -1;
	}

	internal_field = bx_list_search(document_manager.field_list, field_identifier, (equals_function) compare_by_id);
	if (internal_field == NULL) {
		return -1;
	}
	internal_field->field.get(&internal_field->field, data);

	return 0;
}

bx_int8 bx_docman_invoke_set(char *field_identifier, void *data) {
	struct internal_field *internal_field;

	if (field_identifier == NULL) {
		return -1;
	}

	internal_field = bx_list_search(document_manager.field_list, field_identifier, (equals_function) compare_by_id);
	if (internal_field == NULL) {
		return -1;
	}
	internal_field->field.set(&internal_field->field, data);

	return 0;
}

bx_boolean compare_by_id(struct internal_field *field, char *identifier) {

	if (strncmp(field->identifier, identifier, DM_FIELD_IDENTIFIER_LENGTH) == 0) {
		return BX_BOOLEAN_TRUE;
	} else {
		return BX_BOOLEAN_FALSE;
	}
}
