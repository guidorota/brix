/*
 * byte_buffer.c
 * Created on: Mar 1, 2014
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

#include "utils/byte_buffer.h"

bx_int8 bx_bbuf_init(struct bx_byte_buffer *buffer, bx_uint8 *storage, bx_size capacity) {

	if (buffer == NULL || storage == NULL) {
		return -1;
	}

	buffer->storage = storage;
	buffer->capacity = capacity;
	buffer->head = 0;
	buffer->size = 0;

	return 0;
}

bx_int8 bx_bbuf_append(struct bx_byte_buffer *buffer, void *data, bx_size length) {
	bx_uint8 *tail;

	if (buffer == NULL || data == NULL) {
		return -1;
	}

	if (buffer->size + length > buffer->capacity) {
		return -1;
	}

	tail = (buffer->head + buffer->size) % buffer->capacity;
	memcpy((void *) buffer->storage + tail, data, length);
	buffer->size += length;

	return 0;
}

bx_int8 bx_bbuf_get(struct bx_byte_buffer *buffer, void *data, bx_size length) {

	return -1; //TODO: Stub
}

bx_ssize bx_bbuf_size(struct bx_byte_buffer *buffer) {

	if (buffer == NULL) {
		return -1;
	}

	return buffer->size;
}

bx_int8 bx_bbuf_reset(struct bx_byte_buffer *buffer) {

	if (buffer == NULL) {
		return -1;
	}

	buffer->head = 0;
	buffer->size = 0;

	return 0;
}
