/*
 * stack.c
 * Created on: Feb 16, 2014
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

#include "stack.h"
#include "compile_assert.h"
#include <string.h>

/**
 * Stack data structure.
 */
struct bx_stack {
	bx_size top;
	bx_size capacity;
};

#define STORAGE_POINTER(stack_pointer) ((bx_uint8 *) stack_pointer + sizeof (struct bx_stack))

#define STACK_TOP_POINTER(stack_pointer) (STORAGE_POINTER(stack_pointer) + stack_pointer->top)

struct bx_stack *bx_stack_init(void *storage, bx_size storage_size) {
	struct bx_stack *stack;

	BX_COMPILE_ASSERT((sizeof (struct bx_stack)) == BX_STACK_SIZE);

	if (storage == NULL) {
		return NULL;
	}

	if (storage_size <= sizeof (struct bx_stack)) {
		return NULL;
	}

	stack = (struct bx_stack *) storage;
	stack->capacity = storage_size - sizeof (struct bx_stack);
	stack->top = 0;

	return stack;
}

bx_int8 bx_stack_push(struct bx_stack *stack, const void *from, bx_size size) {

	if (stack == NULL || from == NULL) {
		return -1;
	}

	if (stack->capacity - stack->top < size) {
		return -1;
	}

	memcpy(STACK_TOP_POINTER(stack), from, size);
	stack->top += size;

	return 0;
}

bx_int8 bx_stack_pop(struct bx_stack *stack, void *to, bx_size size) {

	if (stack == NULL) {
		return -1;
	}

	if (stack->top < size) {
		return -1;
	}

	memcpy(to, STACK_TOP_POINTER(stack) - size, size);
	stack->top -= size;

	return 0;
}

bx_ssize bx_stack_size(struct bx_stack *stack) {

	if (stack == NULL) {
		return -1;
	}

	return stack->top;
}

bx_ssize bx_stack_capacity(struct bx_stack *stack) {

	if (stack == NULL) {
		return -1;
	}

	return stack->capacity;
}

bx_int8 bx_stack_reset(struct bx_stack *stack) {

	if (stack == NULL) {
		return -1;
	}

	stack->top = 0;

	return 0;
}
