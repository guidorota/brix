/*
 * list.h
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

/**
 * An ordered collection of data of the same type backed by a static byte array.
 */

#ifndef LIST_H_
#define LIST_H_

#include "types.h"

#define BX_LIST_GET(list_pointer, index, type) (type *) bx_list_get(list_pointer, index)

struct bx_list {
	void *storage;
	bx_size storage_size;
	bx_size element_size;
	bx_size storage_used;
};

typedef bx_boolean (*equals_function)(void *list_element, void *comparison_element);

/**
 * List data structure initialization.
 *
 * @param list List to initialize
 * @param storage Storage byte array
 * @param storage_size Size of the storage byte array
 * @param element_size Size in bytes of a single element of the list
 *
 * @return 0 upon successful initialization, -1 otherwise
 */
bx_int8 bx_list_init(struct bx_list *list, void *storage, bx_size storage_size, bx_size element_size);

/**
 * Appends the element at the end of the list.
 * New elements are copied in the underliying storage byte array upon insertion.
 * Adding an element whose type is not consistent with the list element size
 * results in undefined behavious.
 *
 * @param list Destination list
 * @param element Element to add in the list
 *
 * @return Pointer to element added, NULL on error
 */
void *bx_list_add(struct bx_list *list, void *element);


/**
 * Returns a pointer to an element given its index.
 *
 * @param list List pointer
 * @param index Index of the element to retrieve
 *
 * @return Pointer to the element at the specified index, NULL if error or index out of bound
 */
void *bx_list_get(struct bx_list *list, bx_size index);

/**
 * Creates a copy of the element at the specified index.
 * This function assumes that the destination memory location is ample enough
 * to contain a list element
 *
 * @param list List pointer
 * @param index Index of the element to copy
 * @param to Pointer to the destination memory area
 *
 * @return 0 if copy is successful, -1 otherwise
 */
bx_int8 bx_list_copy(struct bx_list *list, bx_size index, void *to);

/**
 * Returns the index of the first occurrence of an element.
 *
 * @param list List pointer
 * @param element Element to search
 * @param equals Function used to compare the 2 elements
 *
 * @return Index of the element, -1 on error
 */
bx_ssize bx_list_indexof(struct bx_list *list, void *element, equals_function equals);

/**
 * Returns a pointer to the first occurrence of an element
 *
 * @param list List pointer
 * @param element Element to search
 * @param equals Function used to compare the list elements with the
 *
 * @return Pointer to the, NULL if not found or if error occurs
 */
void *bx_list_search(struct bx_list *list, void *element, equals_function equals);

/**
 * Removes an element from a list.
 * All data following the removed element will be shifted to avoid fragmentation.
 * All pointers to the removed list element will be invalid after the reset
 * operation. Continued use of these pointers will lead to undefined behaviour.
 *
 * @param List pointer
 * @param index Index of the element to remove
 *
 * @return 0 upon success, -1 otherwise
 */
bx_int8 bx_list_remove(struct bx_list *list, bx_size index);

/**
 * Resets the list.
 * The reset operation does not alter the underlying storage array.
 * All pointers to list elements will be invalid after the reset operation.
 * Continued use of these pointers will lead to undefined behaviour.
 *
 * @param list List pointer
 */
void bx_list_reset(struct bx_list *list);

#endif /* LIST_H_ */
