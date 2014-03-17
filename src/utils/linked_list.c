/*
 * linked_list.c
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

#include "types.h"
#include "linked_list.h"
#include "memory_utils.h"

static bx_int8 default_equals(void *element1, void *element2) {
	return element1 == element2 ? 1 : 0;
}

struct bx_linked_list *bx_llist_add(struct bx_linked_list **linked_list, void *element) {
	struct bx_linked_list *node;

	if (linked_list == NULL || element == NULL) {
		return NULL;
	}

	node = BX_MALLOC(struct bx_linked_list);
	if (node == NULL) {
		return NULL;
	}

	node->element = element;
	node->next = *linked_list;
	*linked_list = node;

	return node;
}

void *bx_llist_remove_head(struct bx_linked_list **linked_list) {
	struct bx_linked_list *old_head;
	void *element_content;

	if (linked_list == NULL) {
		return NULL;
	}

	element_content = (*linked_list)->element;
	old_head = *linked_list;
	*linked_list = (*linked_list)->next;
	free(old_head);

	return element_content;
}

void *bx_llist_remove(struct bx_linked_list **linked_list, void *element) {
	return bx_llist_remove_equals(linked_list, element, default_equals);
}

void *bx_llist_remove_equals(struct bx_linked_list **linked_list, void *element, bx_llist_equals equals) {
	struct bx_linked_list *node;
	struct bx_linked_list *previous;
	void *element_content;

	if (linked_list == NULL || element == NULL || equals == NULL) {
		return NULL;
	}

	for (node = *linked_list, previous = NULL; node != NULL; previous = node, node = node->next) {
		if (!equals(node->element, element)) {
			continue;
		}
		element_content = node->element;
		if (previous == NULL) {
			*linked_list = node->next;
		} else {
			previous->next = node->next;
		}
		free(node);
		return element_content;
	}

	return NULL;
}

bx_int8 bx_llist_contains(struct bx_linked_list *linked_list, void *element) {
	return bx_llist_contains_equals(linked_list, element, default_equals);
}

bx_int8 bx_llist_contains_equals(struct bx_linked_list *linked_list, void *element, bx_llist_equals equals) {
	struct bx_linked_list *node;

	if (element == NULL || equals == NULL) {
		return -1;
	}

	for (node = linked_list; node != NULL; node = node->next) {
		if (equals(node->element, element)) {
			return 1;
		}
	}

	return 0;
}

bx_ssize bx_llist_size(struct bx_linked_list *linked_list) {
	struct bx_linked_list *node;
	int size = 0;

	for (node = linked_list; node != NULL; node = node->next) {
		size++;
	}
	return size;
}

void *bx_llist_find(struct bx_linked_list *linked_list, void *element) {
	return bx_llist_find_equals(linked_list, element, default_equals);
}

void *bx_llist_find_equals(struct bx_linked_list *linked_list, void *element, bx_llist_equals equals) {
	struct bx_linked_list *node;

	if (element == NULL || equals == NULL) {
		return NULL;
	}

	for (node = linked_list; node != NULL; node = node->next) {
		if (equals(node->element, element)) {
			return node->element;
		}
	}

	return NULL;
}
