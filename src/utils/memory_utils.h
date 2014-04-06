/*
 * memory_utils.h
 * Created on: Apr 6, 2014
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

#ifndef MEMORY_UTILS_H_
#define MEMORY_UTILS_H_

#include "types.h"
#include <string.h>
#include "configuration.h"

/*
 * Architecture dependent defines.
 */
#ifdef NATIVE_LITTLE_ENDIAN
#	define BX_MUTILS_HTB_COPY(to_ptr, from_ptr, length) bx_mutils_byte_order_switch_copy(to_ptr, from_ptr, length)
#	define BX_MUTILS_HTB16(data) bx_mutils_swap16(data)
#	define BX_MUTILS_HTB32(data) bx_mutils_swap32(data)
#	define BX_MUTILS_BTH_COPY(to_ptr, from_ptr, length) bx_mutils_byte_order_switch_copy(to_ptr, from_ptr, length)
#	define BX_MUTILS_BTH16(data) bx_mutils_swap16(data)
#	define BX_MUTILS_BTH32(data) bx_mutils_swap32(data)
#elif defined NATIVE_BIG_ENDIAN
#	define BX_MUTILS_HTB_COPY(to_ptr, from_ptr, length) memcpy(to_ptr, from_ptr, length)
#	define BX_MUTILS_HTB16(data) data
#	define BX_MUTILS_HTB32(data) data
#	define BX_MUTILS_BTH_COPY(to_ptr, from_ptr, length) memcpy(to_ptr, from_ptr, length)
#	define BX_MUTILS_BTH16(data) data
#	define BX_MUTILS_BTH32(data) data
#endif


/**
 * Copies memory and swaps the byte order
 *
 * @param to Destination memory location
 * @param from Source memory location
 * @param length Memory length
 *
 * @return Pointer to destination memory location
 */
void *bx_mutils_byte_order_switch_copy(void *to, const void *from, bx_size length);

/**
 * Swaps the byte order of a 16 bit long variable
 *
 * @param data Data
 *
 * @return Data with inverted byte order
 */
bx_uint16 bx_mutils_swap16(bx_uint16 data);

/**
 * Swaps the byte order of a 32 bit long variable
 *
 * @param data Data
 *
 * @return Data with inverted byte order
 */
bx_uint32 bx_mutils_swap32(bx_uint32 data);

#endif /* MEMORY_UTILS_H_ */
