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

bx_boolean search_field_by_id(struct bx_document_field *field, char *field_identifier);

struct bx_document_field field_storage[DM_MAX_FIELD_NUMBER];

struct bx_list field_list;

bx_int8 bx_dm_document_manager_init() {
	BX_LOG(LOG_INFO, "document_manager", "Initializing document manager...");

	bx_list_init(&field_list, field_storage, DM_MAX_FIELD_NUMBER, sizeof (struct bx_document_field));

	return 0;
}

bx_int8 bx_dm_add_field(struct bx_document_field *field) {
	void *result;
	if (field == NULL) {
		return -1;
	}

	result = bx_list_add(&field_list, field);

	return result != NULL ? 0 : -1;
}

bx_int8 bx_dm_invoke_get(char *field_identifier, void *data) {
	return 0; //TODO: Stub
}

bx_int8 bx_dm_invoke_set(char *field_identifier, void *data) {
	return 0; //TODO: Stub
}

bx_boolean search_field_by_id(struct bx_document_field *field, char *field_identifier) {

	if (memcmp((void *) field->identifier, (void *) field_identifier, DM_FIELD_IDENTIFIER_LENGTH) == 0) {
		return BX_BOOLEAN_TRUE;
	} else {
		return BX_BOOLEAN_FALSE;
	}
}
