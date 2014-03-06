/*
 * codegen_code.c
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

#include <string.h>
#include "configuration.h"
#include "utils/memory_utils.h"
#include "compiler/codegen_code.h"

#define DEFAULT_SIZE 256

static bx_int8 add_to_code(struct bx_comp_code *code, void *data, bx_size data_length);

struct bx_comp_code *bx_cgco_create() {
	struct bx_comp_code *code;

	code = BX_MALLOC(struct bx_comp_code);
	if (code == NULL) {
		return NULL;
	}

	code->data = malloc(DEFAULT_SIZE);
	if (code->data == NULL) {
		free(code);
		return NULL;
	}

	code->capacity = DEFAULT_SIZE;
	code->size = 0;

	return code;
}

void bx_cgco_destroy(struct bx_comp_code *code) {

	if (code == NULL) {
		return;
	}

	free(code->data);
	free(code);
}

bx_int8 bx_cgco_add_instruction(struct bx_comp_code *code, bx_uint8 instruction) {
	return add_to_code(code, (void *) &instruction, sizeof instruction);
}

bx_int8 bx_cgco_add_identifier(struct bx_comp_code *code, char *identifier) {
	return add_to_code(code, (void *) identifier, DM_FIELD_IDENTIFIER_LENGTH);
}

bx_int8 bx_cgco_add_int_constant(struct bx_comp_code *code, bx_int32 value) {
	return add_to_code(code, (void *) &value, sizeof value);
}

bx_int8 bx_cgco_add_float_constant(struct bx_comp_code *code, bx_float32 value) {
	return add_to_code(code, (void *) &value, sizeof value);
}

bx_int8 bx_cgco_add_bool_constant(struct bx_comp_code *code, bx_uint32 value) {
	if (value > 0) {
		value = 1;
	}
	return add_to_code(code, (void *) &value, sizeof value);
}

bx_int8 bx_cgco_append_code(struct bx_comp_code *code, struct bx_comp_code *append) {
	return add_to_code(code, append->data, append->size);
}

static bx_int8 add_to_code(struct bx_comp_code *code, void *data, bx_size data_length) {

	if (code == NULL || data == NULL) {
		return -1;
	}

	if (code->size + data_length > code->capacity) {
		code->data = realloc(code->data, code->capacity + DEFAULT_SIZE);
		if (code->data == NULL) {
			return -1;
		}
		code->capacity += DEFAULT_SIZE;
	}

	memcpy(code->data, data, data_length);
	code->size += data_length;

	return 0;
}