/*
 * vm_utils.c
 * Created on: Mar 2, 2014
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

#include "vm_utils.h"
#include "configuration.h"
#include "utils/memory_utils.h"

bx_int8 bx_vmutils_add_instruction(struct bx_byte_buffer *buffer, enum bx_instruction instruction) {
	bx_uint8 uint8_instruction;

	if (buffer == NULL) {
		return -1;
	}

	uint8_instruction = (bx_uint8) instruction;
	return bx_bbuf_append(buffer, &uint8_instruction, 1);
}

bx_int8 bx_vmutils_add_int(struct bx_byte_buffer *buffer, bx_int32 data) {

	if (buffer == NULL) {
		return -1;
	}

	data = BX_MUTILS_HTB32(data);
	return bx_bbuf_append(buffer, &data, 4);
}

bx_int8 bx_vmutils_add_short(struct bx_byte_buffer *buffer, bx_uint16 data) {

	if (buffer == NULL) {
		return -1;
	}

	data = BX_MUTILS_HTB16(data);
	return bx_bbuf_append(buffer, &data, 2);
}

bx_int8 bx_vmutils_add_float(struct bx_byte_buffer *buffer, bx_float32 data) {
	bx_float32 brix_byte_order_float;

	if (buffer == NULL) {
		return -1;
	}

	BX_MUTILS_HTB_COPY(&brix_byte_order_float, &data, 4);
	return bx_bbuf_append(buffer, &brix_byte_order_float, 4);
}

bx_int8 bx_vmutils_add_identifier(struct bx_byte_buffer *buffer, char *identifier) {

	if (buffer == NULL) {
		return -1;
	}

	return bx_bbuf_append(buffer, identifier, DM_FIELD_IDENTIFIER_LENGTH);
}
