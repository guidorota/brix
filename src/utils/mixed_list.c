/*
 * mixed_list.c
 * Created on: Feb 22, 2014
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

#include "utils/mixed_list.h"
#include <string.h>

#define TAIL_POINTER(list_pointer) ((bx_uint8 *) list_pointer->storage) + list_pointer->storage_used
#define OFFSET_POINTER(list_pointer, offset) ((bx_uint8 *) list_pointer->storage + offset)

bx_int8 bx_mlist_init(struct bx_mlist *list, void *storage, bx_size storage_size) {

	if (list == NULL || storage == NULL || storage_size == 0) {
		return -1;
	}

	list->storage = storage;
	list->storage_size = storage_size;
	list->storage_used = 0;

	return 0;
}

bx_int8 bx_mlist_add_element(struct bx_mlist *list, void *element, bx_size element_size) {

	if (list == NULL || element == NULL || element_size == 0) {
		return -1;
	}

	memcpy(TAIL_POINTER(list), &element_size, sizeof element_size);
	list->storage_used += sizeof element_size;
	memcpy(TAIL_POINTER(list), element, element_size);
	list->storage_used += element_size;

	return 0;
}

bx_int8 bx_mlist_get_element(struct bx_mlist *list, bx_size index, void **element, bx_size *element_size) {

	if (list == NULL || element == NULL) {
		return -1;
	}

	if (list->storage_used == 0) {
		return -1;
	}

	int current_index = 0;
	bx_size current_position = 0;
	do {
		memcpy((void *) element_size, (void *) OFFSET_POINTER(list, current_position), sizeof *element_size);
		current_position += sizeof *element_size;
		if (current_index == index) {
			*element = (void *) OFFSET_POINTER(list, current_position);
			return 0;
		}
		current_position += *element_size;
		current_index++;
	} while(current_position < list->storage_used);

	return 1;
}

bx_int8 bx_mlist_remove_element(struct bx_mlist *list, bx_size index) {
	bx_int8 error;
	bx_uint8 *element;
	bx_size element_size;
	bx_size remaining_bytes;

	if (list == NULL) {
		return -1;
	}

	error = bx_mlist_get_element(list, index, (void **) &element, &element_size);
	if (error != 0) {
		return error;
	}
	element -= sizeof element_size;
	element_size += sizeof element_size;
	remaining_bytes = list->storage_used - (element - (bx_uint8 *) list->storage) + element_size;
	memcpy((void *) element, (void *) (element + element_size), remaining_bytes);
	list->storage_used -= element_size;

	return 0;
}

bx_int8 bx_mlist_reset(struct bx_mlist *list) {

	if (list == NULL) {
		return -1;
	}

	list->storage_used = 0;

	return 0;
}
