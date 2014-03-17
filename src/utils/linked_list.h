/*
 * linked_list.h
 * Created on: Mar 4, 2014
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


#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include "types.h"

#define BX_LLIST_ELEMENT(node, type) (type *)node->element

typedef bx_int8 (*bx_llist_equals)(void *list_value, void *template);

struct bx_linked_list {
	void *element;
	struct bx_linked_list *next;
};

struct bx_linked_list *bx_llist_add(struct bx_linked_list **linked_list, void *element);

void *bx_llist_remove_head(struct bx_linked_list **linked_list);

void *bx_llist_remove(struct bx_linked_list **linked_list, void *element);

void *bx_llist_remove_equals(struct bx_linked_list **linked_list, void *element, bx_llist_equals equals);

bx_int8 bx_llist_contains(struct bx_linked_list *linked_list, void *element);

bx_int8 bx_llist_contains_equals(struct bx_linked_list *linked_list, void *element, bx_llist_equals equals);

bx_ssize bx_llist_size(struct bx_linked_list *linked_list);

void *bx_llist_find(struct bx_linked_list *linked_list, void *element);

void *bx_llist_find_equals(struct bx_linked_list *linked_list, void *element, bx_llist_equals equals);

#endif /* LINKED_LIST_H_ */
