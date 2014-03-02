/*
 * mixed_list.h
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

#ifndef MIXED_LIST_H_
#define MIXED_LIST_H_

/**
 * An ordered collection of data with different storage sizes.
 */

#include "types.h"

struct bx_mlist {
	void *storage;
	bx_size capacity;
	bx_size size;
};

/**
 * List data structure initialization.
 *
 * @param list List to initialize
 * @param storage Storage byte array
 * @param storage_size Size of the storage byte array
 *
 * @return 0 upon successful initialization, -1 otherwise
 */
bx_int8 bx_mlist_init(struct bx_mlist *list, void *storage, bx_size storage_size);

/**
 * Appends the element at the end of the list.
 * New elements are copied in the underliying storage byte array upon insertion.
 *
 * @param list Destination list
 * @param element Element to add in the list
 * @param element_size Size of the element to add
 *
 * @return Pointer to element added, NULL on error
 */
void *bx_mlist_add(struct bx_mlist *list, void *element, bx_size element_size);

/**
 * Returns a pointer to an element given its index.
 * The size of the element returned is stored inside the element_size parameter.
 *
 * @param list List pointer
 * @param index Index of the element to retrieve
 * @param element_size Pointer to a memory location where
 *
 * @return Pointer to the element at the specified index, NULL if error or index out of bound
 */
void *bx_mlist_get(struct bx_mlist *list, bx_size index, bx_size *element_size);

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
bx_int8 bx_mlist_remove_element(struct bx_mlist *list, bx_size index);

/**
 * Returns the current size of the list.
 *
 * @param list List pointer
 *
 * @return Number of bytes used in the list, -1 on error
 */
bx_ssize bx_mlist_size(struct bx_mlist *list);

/**
 * Returns the capacity of the list.
 *
 * @param list List pointer
 *
 * @return Capacity of the list, -1 on error
 */
bx_ssize bx_mlist_capacity(struct bx_mlist *list);

/**
 * Resets the list.
 * The reset operation does not alter the underlying storage array.
 * All pointers to list elements will be invalid after the reset operation.
 * Continued use of these pointers will lead to undefined behaviour.
 *
 * @param list List pointer
 */
bx_int8 bx_mlist_reset(struct bx_mlist *list);

#endif /* MIXED_LIST_H_ */
