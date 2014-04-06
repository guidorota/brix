/*
 * memory_utils.c
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

#include "utils/memory_utils.h"

void *bx_mutils_byte_order_switch_copy(void *to, const void *from, bx_size length) {
	bx_size n;
	bx_uint8 *from_ptr;
	bx_uint8 *to_ptr;

	n = (length + 7) / 8;
	from_ptr = (bx_uint8 *) from + length - 1;
	to_ptr = (bx_uint8 *) to;

	switch(length % 8) {
	case 0:	do {	*to_ptr++ = *from_ptr--;
	case 7:		*to_ptr++ = *from_ptr--;
	case 6:		*to_ptr++ = *from_ptr--;
	case 5:		*to_ptr++ = *from_ptr--;
	case 4:		*to_ptr++ = *from_ptr--;
	case 3:		*to_ptr++ = *from_ptr--;
	case 2:		*to_ptr++ = *from_ptr--;
	case 1:		*to_ptr++ = *from_ptr--;
			} while(--n > 0);
	}

	return to;
}

bx_uint16 bx_mutils_swap16(bx_uint16 data) {
	bx_uint16 data_swapped;

	data_swapped = 0;
	data_swapped += 0x00FF & (data >> 8);
	data_swapped += 0xFF00 & (data << 8);

	return data_swapped;
}

bx_uint32 bx_mutils_swap32(bx_uint32 data) {
	bx_uint32 data_swapped;

	data_swapped = 0;
	data_swapped += 0x000000FF & (data >> 24);
	data_swapped += 0x0000FF00 & (data >> 8);
	data_swapped += 0x00FF0000 & (data << 8);
	data_swapped += 0xFF000000 & (data << 24);

	return data_swapped;
}
