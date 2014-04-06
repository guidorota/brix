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

#include <stdlib.h>
#include "utils/memory_utils.h"
#include "configuration.h"
#include "compiler/codegen_pcode.h"
#include "virtual_machine/virtual_machine.h"
#include "utils/memory_utils.h"

#define DEFAULT_SIZE 256

static bx_ssize add_to_code(struct bx_comp_pcode *pcode, void *data, bx_size data_length, bx_boolean suppress_nop);

struct bx_comp_pcode *bx_cgpc_create() {
	struct bx_comp_pcode *pcode;

	pcode = malloc(sizeof *pcode);
	if (pcode == NULL) {
		return NULL;
	}

	pcode->data = malloc(DEFAULT_SIZE);
	if (pcode->data == NULL) {
		free(pcode);
		return NULL;
	}

	pcode->capacity = DEFAULT_SIZE;
	pcode->size = 0;

	return pcode;
}

struct bx_comp_pcode *bx_cgpc_copy(struct bx_comp_pcode *pcode) {
	struct bx_comp_pcode *copy;

	if (pcode == NULL) {
		return NULL;
	}

	copy = malloc(sizeof *copy);
	if (pcode == NULL) {
		return NULL;
	}

	copy->data = malloc(pcode->capacity);
	if (pcode->data == NULL) {
		free(pcode);
		return NULL;
	}

	copy->capacity = pcode->capacity;
	copy->size = pcode->size;
	memcpy(copy->data, pcode->data, pcode->capacity);

	return copy;
}

void bx_cgpc_destroy(struct bx_comp_pcode *pcode) {

	if (pcode == NULL) {
		return;
	}

	free(pcode->data);
	free(pcode);
}

bx_ssize bx_cgpc_add_instruction(struct bx_comp_pcode *pcode, enum bx_instruction instruction) {
	bx_uint8 uint8_instruction = (bx_uint8) instruction;
	return add_to_code(pcode, (void *) &uint8_instruction, sizeof uint8_instruction, BX_BOOLEAN_TRUE);
}

bx_ssize bx_cgpc_add_identifier(struct bx_comp_pcode *pcode, char *identifier) {
	return add_to_code(pcode, (void *) identifier, DM_FIELD_IDENTIFIER_LENGTH, BX_BOOLEAN_FALSE);
}

bx_ssize bx_cgpc_add_int_constant(struct bx_comp_pcode *pcode, bx_int32 value) {
	bx_int32 brix_byte_order_value;

	BX_MUTILS_HTB_COPY(&brix_byte_order_value, &value, 4);
	return add_to_code(pcode, (void *) &brix_byte_order_value, 4, BX_BOOLEAN_FALSE);
}

bx_ssize bx_cgpc_add_address(struct bx_comp_pcode *pcode, bx_uint16 address) {
	bx_uint16 brix_byte_order_address;

	BX_MUTILS_HTB_COPY(&brix_byte_order_address, &address, 2);
	return add_to_code(pcode, (void *) &brix_byte_order_address, 2, BX_BOOLEAN_FALSE);
}

bx_ssize bx_cgpc_add_float_constant(struct bx_comp_pcode *pcode, bx_float32 value) {
	bx_float32 brix_byte_order_value;

	BX_MUTILS_HTB_COPY(&brix_byte_order_value, &value, 4);
	return add_to_code(pcode, (void *) &brix_byte_order_value, 4, BX_BOOLEAN_FALSE);
}

bx_ssize bx_cgpc_add_bool_constant(struct bx_comp_pcode *pcode, bx_uint32 value) {
	bx_int32 brix_byte_order_value;

	if (value != 0) {
		value = 1;
	}
	BX_MUTILS_HTB_COPY(&brix_byte_order_value, &value, 4);
	return add_to_code(pcode, (void *) &brix_byte_order_value, 4, BX_BOOLEAN_FALSE);
}

bx_ssize bx_cgpc_append_pcode(struct bx_comp_pcode *destination, struct bx_comp_pcode *source) {
	return add_to_code(destination, source->data, source->size, BX_BOOLEAN_TRUE);
}

bx_ssize bx_cgpc_replace_pcode(struct bx_comp_pcode *destination, struct bx_comp_pcode *source) {
	destination->size = 0;
	return add_to_code(destination, source->data, source->size, BX_BOOLEAN_TRUE);
}

static bx_ssize add_to_code(struct bx_comp_pcode *pcode, void *data, bx_size data_length, bx_boolean suppress_nop) {
	bx_ssize address;

	if (pcode == NULL || data == NULL) {
		return -1;
	}

	if (suppress_nop &&
			*((bx_uint8 *) pcode->data + pcode->size - 1) == (bx_uint8) BX_INSTR_NOP) {
		--pcode->size;
	}

	if (pcode->size + data_length > pcode->capacity) {
		pcode->data = realloc(pcode->data, pcode->capacity + DEFAULT_SIZE);
		if (pcode->data == NULL) {
			return -1;
		}
		pcode->capacity += DEFAULT_SIZE;
	}

	address = pcode->size;
	memcpy((bx_uint8 *) pcode->data + pcode->size, data, data_length);
	pcode->size += data_length;

	return address;
}

bx_comp_label bx_cgpc_create_address_label(struct bx_comp_pcode *pcode) {
	bx_uint16 null_data = 0;
	bx_comp_label label;

	label = pcode->size;
	add_to_code(pcode, (void *) &null_data, sizeof null_data, BX_BOOLEAN_FALSE);

	return label;
}

void bx_cgpc_set_address_label(struct bx_comp_pcode *pcode, bx_comp_label label, bx_uint16 address) {
	BX_MUTILS_HTB_COPY((bx_uint8 *) pcode->data + (bx_size) label, &address, 2);
}
