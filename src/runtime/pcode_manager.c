/*
 * pcode_manager.c
 * Created on: Mar 25, 2014
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
#include "logging.h"
#include "runtime/pcode_manager.h"
#include "virtual_machine/virtual_machine.h"

#define SPACE_USED (pcode_count * sizeof (struct bx_pcode)) + total_instruction_length
#define PCODE_STRUCT(index) (struct bx_pcode *) (pcode_storage + PR_CODE_STORAGE_SIZE - 1 - (index + 1) * sizeof (struct bx_pcode))

struct bx_pcode {
	bx_boolean valid;
	void *instructions;
	bx_size size;
};

static bx_uint8 pcode_storage[PR_CODE_STORAGE_SIZE];
static bx_size total_instruction_length;
static bx_size pcode_count;

static struct bx_pcode *get_available_pcode();

bx_int8 bx_pm_init() {
	total_instruction_length = 0;
	pcode_count = 0;

	return 0;
}

struct bx_pcode *bx_pm_add(void *buffer, bx_size buffer_size) {
	struct bx_pcode *pcode;

	if (buffer == NULL) {
		return NULL;
	}

	if (buffer_size + sizeof (struct bx_pcode) + SPACE_USED > PR_CODE_STORAGE_SIZE) {
		BX_LOG(LOG_ERROR, "pcode_repository", "Cannot store new pcode program: not enough space");
		return NULL;
	}

	pcode = get_available_pcode();
	pcode->instructions = (void *) (pcode_storage + total_instruction_length);
	pcode->size = buffer_size;
	pcode->valid = BX_BOOLEAN_TRUE;
	memcpy(pcode->instructions, buffer, buffer_size);

	pcode_count += 1;
	total_instruction_length += buffer_size;

	return pcode;
}

static struct bx_pcode *get_available_pcode() {
	struct bx_pcode *pcode;
	bx_size i;

	for (i = 0; i < pcode_count; i++) {
		pcode = PCODE_STRUCT(i);
		if (pcode->valid == BX_BOOLEAN_FALSE) {
			return pcode;
		}
	}

	return PCODE_STRUCT(pcode_count++);
}

bx_int8 bx_pm_execute(struct bx_pcode *pcode) {
	if (pcode == NULL) {
		return -1;
	}

	if ((bx_uint8 *) pcode > pcode_storage + PR_CODE_STORAGE_SIZE - 1 ||
			(bx_uint8 *) pcode < (bx_uint8 *) PCODE_STRUCT(pcode_count - 1) ) {
		BX_LOG(LOG_ERROR, "pcode_repository", "Invalid pcode data structure");
		return -1;
	}

	if (pcode->valid == BX_BOOLEAN_FALSE) {
		BX_LOG(LOG_ERROR, "pcode_repository", "Cannot execute: invalid mark set in pcode structure");
		return -1;
	}

	return bx_vm_execute((bx_uint8 *) pcode->instructions, pcode->size);
}

bx_int8 bx_pm_remove(struct bx_pcode *pcode) {
	void *destination;
	void *source;
	bx_size length;

	if (pcode == NULL) {
		return -1;
	}

	if ((bx_uint8 *) pcode > pcode_storage + PR_CODE_STORAGE_SIZE - 1 ||
			(bx_uint8 *) pcode < (bx_uint8 *) PCODE_STRUCT(pcode_count - 1)) {
		BX_LOG(LOG_ERROR, "pcode_repository", "Invalid pcode data structure");
		return -1;
	}

	pcode->valid = BX_BOOLEAN_FALSE;
	destination = (void *) pcode->instructions;
	source = (void *) ((bx_uint8 *) pcode->instructions + pcode->size);
	length = total_instruction_length - ((bx_uint8 *) source - pcode_storage);
	memmove(destination, source, length);
	total_instruction_length -= pcode->size;

	int i;
	for (i = 0; i < pcode_count; i++) {
		struct bx_pcode *current_pcode = PCODE_STRUCT(i);
		if (current_pcode->instructions > pcode->instructions) {
			current_pcode->instructions = (void *) ((bx_uint8 *) current_pcode->instructions - pcode->size);
		}
	}

	pcode->instructions = NULL;
	pcode->size = 0;

	return 0;
}
