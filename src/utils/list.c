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
#include "compile_assert.h"
#include <string.h>

/**
 * List data structure
 */
struct bx_list {
	bx_size element_size;	///< Element size in bytes
	bx_size size;			///< Current number of elements in the list
	bx_size capacity;		///< List capacity (max number of elements)
};

#define STORAGE_POINTER(list_pointer) ((bx_uint8 *) list_pointer + sizeof (struct bx_list))

#define TAIL_POINTER(list_pointer) (STORAGE_POINTER(list_pointer)) + list->element_size * list->size

#define ELEMENT_POINTER(list_pointer, element_index) (STORAGE_POINTER(list_pointer)) + element_index * list_pointer->element_size

struct bx_list *bx_list_init(void *storage, bx_size storage_size, bx_size element_size) {
	struct bx_list *list;

	BX_COMPILE_ASSERT((sizeof (struct bx_list)) == BX_LIST_SIZE);

	if (storage == NULL || storage_size == 0) {
		return NULL;
	}

	if (storage_size <= sizeof (struct bx_list) + element_size) {
		return NULL;
	}

	list = (struct bx_list *)storage;

	list->capacity = (storage_size - sizeof (struct bx_list)) / element_size;
	list->element_size = element_size;
	list->size = 0;

	return list;
}

void * bx_list_add(struct bx_list *list, void *element) {
	void *new_element;

	if (list == NULL || list->size >= list->capacity) {
		return NULL;
	}

	new_element = TAIL_POINTER(list);
	memcpy(new_element, element, list->element_size);
	list->size++;

	return new_element;
}

void *bx_list_get(struct bx_list *list, bx_size index) {

	if (list == NULL) {
		return NULL;
	}

	if (list->size == 0 || index > list->size - 1) {
		return NULL;
	}

	return ELEMENT_POINTER(list, index);
}

void *bx_list_get_empty(struct bx_list *list) {
	void *element;

	if (list == NULL || list->size >= list->capacity) {
		return NULL;
	}

	element = TAIL_POINTER(list);
	list->size++;

	return element;
}

bx_int8 bx_list_copy(struct bx_list *list, bx_size index, void *to) {

	if (list == NULL || to == NULL) {
		return -1;
	}

	if (list->size == 0 || index > list->size - 1) {
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
	for (i = 0; i < list->size; i++) {
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
	for (i = 0; i< list->size; i++) {
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

	if (list->size == 0 || index > list->size - 1) {
		return -1;
	}

	if (index < list->size - 1) {
		memcpy(ELEMENT_POINTER(list, index), ELEMENT_POINTER(list, index), list->element_size);
	}
	list->size--;

	return 0;
}

bx_ssize bx_list_capacity(struct bx_list *list) {

	if (list == NULL) {
		return -1;
	}

	return list->capacity;
}

bx_ssize bx_list_size(struct bx_list *list) {

	if (list == NULL) {
		return -1;
	}

	return list->size;
}

bx_ssize bx_list_element_size(struct bx_list *list) {

	if (list == NULL) {
		return -1;
	}

	return list->element_size;
}

void bx_list_reset(struct bx_list *list) {

	if (list == NULL) {
		return;
	}

	list->size = 0;
}
