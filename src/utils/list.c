/*
 * list.c
 * Created on: Feb 21, 2014
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

#include "list.h"
#include <string.h>

#define TAIL_POINTER(list_pointer) ((bx_uint8 *) list_pointer->storage) + list->storage_used
#define ELEMENT_POINTER(list_pointer, element_index) ((bx_uint8 *) list_pointer->storage) + element_index * list_pointer->element_size

bx_int8 bx_list_init(struct bx_list *list, void *storage, bx_size storage_size, bx_size element_size) {

	if (list == NULL || storage == NULL || storage_size == 0) {
		return -1;
	}

	list->storage = storage;
	list->storage_size = storage_size;
	list->element_size = element_size;
	list->storage_used = 0;

	return 0;
}

void * bx_list_add(struct bx_list *list, void *element) {
	void *new_element;

	if (list == NULL) {
		return NULL;
	}

	if (list->storage_used + list->element_size > list->storage_size) {
		return NULL;
	}

	new_element = TAIL_POINTER(list);
	memcpy(new_element, element, list->element_size);
	list->storage_used += list->element_size;

	return new_element;
}

void *bx_list_get(struct bx_list *list, bx_size index) {

	if (list == NULL) {
		return NULL;
	}

	if (list->storage_used == 0 ||
			index * list->element_size > list->storage_size ||
			index * list->element_size > list->storage_used) {
		return NULL;
	}

	return ELEMENT_POINTER(list, index);
}

void *bx_list_get_empty(struct bx_list *list) {
	void *element;

	if (list == NULL ||
			list->storage_used + list->element_size > list->storage_size) {
		return NULL;
	}

	element = TAIL_POINTER(list);
	list->storage_used += list->element_size;

	return element;
}

bx_int8 bx_list_copy(struct bx_list *list, bx_size index, void *to) {

	if (list == NULL || to == NULL) {
		return -1;
	}

	if (list->storage_used == 0 ||
			index * list->element_size > list->storage_size ||
			index * list->element_size > list->storage_used) {
		return -1;
	}

	memcpy(to, ELEMENT_POINTER(list, index), list->element_size);

	return 0;
}

bx_ssize bx_list_indexof(struct bx_list *list, void *element, equals_function equals) {

	if (list == NULL || element == NULL || equals == NULL) {
		return -1;
	}

	int i;
	for (i = 0; i * list->element_size < list->storage_used; i++) {
		if (equals(ELEMENT_POINTER(list, i), element) == BX_BOOLEAN_TRUE) {
			return i;
		}
	}

	return -1;
}

void *bx_list_search(struct bx_list *list, void *element, equals_function equals) {

	if (list == NULL || element == NULL || equals == NULL) {
		return NULL;
	}

	int i;
	for (i = 0; i * list->element_size < list->storage_used; i++) {
		if (equals(ELEMENT_POINTER(list, i), element) == BX_BOOLEAN_TRUE) {
			return ELEMENT_POINTER(list, i);
		}
	}

	return NULL;
}

bx_int8 bx_list_remove(struct bx_list *list, bx_size index) {

	if (list == NULL) {
		return -1;
	}

	if (list->storage_used == 0 ||
			index * list->element_size > list->storage_size ||
			index * list->element_size > list->storage_used) {
		return -1;
	}

	if (index * list->element_size < list->storage_used) {
		memcpy(ELEMENT_POINTER(list, index), ELEMENT_POINTER(list, index), list->element_size);
	}
	list->storage_used -= list->element_size;

	return 0;
}

void bx_list_reset(struct bx_list *list) {

	if (list == NULL) {
		return;
	}

	list->storage_used = 0;
}
