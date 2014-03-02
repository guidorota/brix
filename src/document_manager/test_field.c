/*
 * test_field.c
 * Created on: Feb 28, 2014
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

#include "test_field.h"

static bx_int8 test_field_get(struct bx_document_field *instance, void *data);
static bx_int8 test_field_set(struct bx_document_field *instance, void *data);

static bx_int32 internal_value;

bx_int8 bx_test_field_init(struct bx_document_field *field) {

	if (field == NULL) {
		return -1;
	}

	field->get = &test_field_get;
	field->set = &test_field_set;
	field->private_data = &internal_value;
	field->type = INT;

	return 0;
}

void bx_test_field_set_data(bx_int32 value) {
	internal_value = value;
}

bx_int32 bx_test_field_get_data() {
	return internal_value;
}

bx_int8 test_field_get(struct bx_document_field *instance, void *data) {

	if (instance == NULL || data == NULL) {
		return -1;
	}

	*(bx_int32 *) data = internal_value;
	return 0;
}

bx_int8 test_field_set(struct bx_document_field *instance, void *data) {
	if (instance == NULL || data == NULL) {
		return -1;
	}

	internal_value = *(bx_int32 *) data;
	return 0;
}