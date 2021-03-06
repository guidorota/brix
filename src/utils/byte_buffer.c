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
#include "compile_assert.h"

#define BBUF_DATA(buffer_pointer) ((bx_uint8 *) buffer_pointer + sizeof (struct bx_byte_buffer))

/**
 * Buffer data structure
 */
struct bx_byte_buffer {
	bx_size capacity;		///< Byte buffer capacity in byte
	bx_size head;			///< Pointer to the first readable byte
	bx_size size;			///< Number of bytes in the buffer
};

struct bx_byte_buffer *bx_bbuf_init(bx_uint8 *storage, bx_size storage_size) {
	struct bx_byte_buffer *buffer;

	BX_COMPILE_ASSERT(sizeof (struct bx_byte_buffer) == BX_BYTE_BUFFER_SIZE);

	if (storage == NULL) {
		return NULL;
	}

	if (sizeof (struct bx_byte_buffer) >= storage_size) {
		return NULL;
	}

	buffer = (struct bx_byte_buffer *) storage;
	buffer->capacity = storage_size - sizeof (struct bx_byte_buffer);
	buffer->head = 0;
	buffer->size = 0;

	return buffer;
}

bx_int8 bx_bbuf_append(struct bx_byte_buffer *buffer, void *data, bx_size length) {
	bx_size tail;
	bx_uint8 *source;

	if (buffer == NULL || data == NULL) {
		return -1;
	}

	if (buffer->size + length > buffer->capacity) {
		return -1;
	}

	source = (bx_uint8 *) data;
	tail = (buffer->head + buffer->size) % buffer->capacity;
	buffer->size += length;
	while(length > 0) {
		*(BBUF_DATA(buffer) + tail) = *source++;
		tail = (tail + 1) % buffer->capacity;
		length--;
	}

	return 0;
}

bx_int8 bx_bbuf_get(struct bx_byte_buffer *buffer, void *data, bx_size length) {
	bx_size i;
	bx_uint8 *destination;

	if (buffer == NULL || data == NULL) {
		return -1;
	}

	if (length > buffer->size) {
		return -1;
	}

	destination = (bx_uint8 *) data;
	i = buffer->head;
	buffer->size -= length;
	while(length > 0) {
		*destination++ = *(BBUF_DATA(buffer) + i);
		i = (i + 1) % buffer->capacity;
		length--;
	}

	return 0;
}

bx_ssize bx_bbuf_size(struct bx_byte_buffer *buffer) {

	if (buffer == NULL) {
		return -1;
	}

	return buffer->size;
}

bx_ssize bx_bbuf_capacity(struct bx_byte_buffer *buffer) {

	if (buffer == NULL) {
		return -1;
	}

	return buffer->capacity;
}

bx_int8 bx_bbuf_reset(struct bx_byte_buffer *buffer) {

	if (buffer == NULL) {
		return -1;
	}

	buffer->head = 0;
	buffer->size = 0;

	return 0;
}
