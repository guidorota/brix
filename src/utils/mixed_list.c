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

#define TAIL_POINTER(list_pointer) ((bx_uint8 *) list_pointer->storage) + list_pointer->size
#define OFFSET_POINTER(list_pointer, offset) ((bx_uint8 *) list_pointer->storage + offset)

static bx_ssize element_count(struct bx_mlist *list);

bx_int8 bx_mlist_init(struct bx_mlist *list, void *storage, bx_size capacity) {

	if (list == NULL || storage == NULL || capacity == 0) {
		return -1;
	}

	list->storage = storage;
	list->capacity = capacity;
	list->size = 0;

	return 0;
}

bx_ssize bx_mlist_add(struct bx_mlist *list, void *element, bx_size element_size) {
	void *new_element;

	if (list == NULL || element == NULL || element_size == 0) {
		return -1;
	}

	memcpy(TAIL_POINTER(list), &element_size, sizeof element_size);
	list->size += sizeof element_size;
	new_element = TAIL_POINTER(list);
	memcpy(new_element, element, element_size);
	list->size += element_size;

	return element_count(list);
}

static bx_ssize element_count(struct bx_mlist *list) {
	bx_size element_count = 0;
	bx_size offset = 0;

	while(offset < list->size) {
		offset = sizeof (bx_size) + * (bx_uint16 *) OFFSET_POINTER(list, offset);
		++element_count;
	}

	return element_count;
}

void *bx_mlist_get(struct bx_mlist *list, bx_size index, bx_size *element_size) {

	if (list == NULL || list->size == 0) {
		return NULL;
	}

	int current_index = 0;
	bx_size current_position = 0;
	do {
		memcpy((void *) element_size, (void *) OFFSET_POINTER(list, current_position), sizeof *element_size);
		current_position += sizeof *element_size;
		if (current_index == index) {
			return (void *) OFFSET_POINTER(list, current_position);
		}
		current_position += *element_size;
		current_index++;
	} while(current_position < list->size);

	return NULL;
}

bx_int8 bx_mlist_remove_element(struct bx_mlist *list, bx_size index) {
	bx_uint8 *element;
	bx_size element_size;
	bx_size remaining_bytes;

	if (list == NULL) {
		return -1;
	}

	element = bx_mlist_get(list, index, &element_size);
	if (element == NULL) {
		return -1;
	}
	element -= sizeof element_size;
	element_size += sizeof element_size;
	remaining_bytes = list->size - (element - (bx_uint8 *) list->storage) + element_size;
	memcpy((void *) element, (void *) (element + element_size), remaining_bytes);
	list->size -= element_size;

	return 0;
}

bx_ssize bx_mlist_size(struct bx_mlist *list) {

	if (list == NULL) {
		return -1;
	}

	return list->size;
}

bx_ssize bx_mlist_capacity(struct bx_mlist *list) {

	if (list == NULL) {
		return -1;
	}

	return list->capacity;
}

bx_int8 bx_mlist_reset(struct bx_mlist *list) {

	if (list == NULL) {
		return -1;
	}

	list->size = 0;

	return 0;
}
