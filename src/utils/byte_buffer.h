/*
 * byte_buffer.h
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

#ifndef BYTE_BUFFER_H_
#define BYTE_BUFFER_H_

#include "types.h"

/**
 * A circular FIFO byte buffer.
 */
struct bx_byte_buffer;

/**
 * Byte buffer data structure initialization.
 *
 * @param list Byte buffer to initialize
 * @param storage Storage byte array
 * @param storage_size Size of the storage byte array in bytes
 * @param element_size Size in bytes of a single element of the list
 *
 * @return Byte buffer instance pointer
 */
struct bx_byte_buffer *bx_bbuf_init(bx_uint8 *storage, bx_size storage_size);

/**
 * Append data to the byte buffer.
 *
 * @param buffer Byte buffer pointer
 * @param data Pointer to the data to append
 * @param length Number of bytes to append
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_bbuf_append(struct bx_byte_buffer *buffer, void *data, bx_size length);

/**
 * Retrieve data from a byte buffer.
 * After retrieval, the data is removed from the byte buffer.
 *
 * @param buffer Byte buffer pointer
 * @param data Pointer to the output memory location
 * @param length Number of bytes to append
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_bbuf_get(struct bx_byte_buffer *buffer, void *data, bx_size length);

/**
 * Retrieves the current size of a byte buffer
 *
 * @param buffer Byte buffer pointer
 *
 * @return Current byte buffer size, -1 on failure
 */
bx_ssize bx_bbuf_size(struct bx_byte_buffer *buffer);

/**
 * Retrieves the maximum number of bytes that the byte buffer can contain
 *
 * @param buffer Byte buffer pointer
 *
 * @return Byte buffer capacity, -1 on failure
 */
bx_ssize bx_bbuf_capacity(struct bx_byte_buffer *buffer);

/**
 * Resets the byte buffer.
 * The reset operation does not alter the underlying storage array.
 *
 * @param buffer Byte buffer pointer
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_bbuf_reset(struct bx_byte_buffer *buffer);

#endif /* BYTE_BUFFER_H_ */
