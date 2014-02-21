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

#ifndef LIST_H_
#define LIST_H_

#include "types.h"

struct bx_list {
	void *storage;
	bx_size storage_size;
	bx_size element_size;
	bx_size storage_used;
};

typedef bx_boolean (*equals_function)(void *list_element, void *comparison_element);

bx_int8 bx_list_init(struct bx_list *list, void *storage, bx_size storage_size, bx_size element_size);

bx_int8 bx_list_add_element(struct bx_list *list, void *element);

bx_int8 bx_list_get_element(struct bx_list *list, bx_size index, void **element);

bx_int8 bx_list_copy_element(struct bx_list *list, bx_size index, void *element);

bx_int8 bx_list_indexof(struct bx_list *list, bx_size *index, void *comparison_element, equals_function equals);

bx_int8 bx_list_search_element(struct bx_list *list, void **element, void *comparison_element, equals_function equals);

bx_int8 bx_list_remove_element(struct bx_list *list, bx_size index);

bx_int8 bx_list_reset(struct bx_list *list);

#endif /* LIST_H_ */
