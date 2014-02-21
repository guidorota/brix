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
#include <string.h>

#define STACK_TOP_POINTER(stack_pointer) ((uint8_t *) stack_pointer->stack + stack_pointer->top)

bx_int8 bx_stack_setup(struct bx_stack *stack, void *byte_array, bx_size stack_size) {

	stack->stack = byte_array;
	stack->top = 0;
	stack->size = stack_size;

	return 0;
}

bx_int8 bx_stack_push(struct bx_stack *stack, void *variable, bx_size size) {

	if (stack->size - stack->top < size) {
		return -1;
	}

	memcpy(STACK_TOP_POINTER(stack), variable, size);
	stack->top += size;

	return 0;
}

bx_int8 bx_stack_pop(struct bx_stack *stack, void *variable, bx_size size) {

	if (stack->top < size) {
		return -1;
	}

	memcpy(variable, STACK_TOP_POINTER(stack) - size, size);
	stack->top -= size;

	return 0;
}

void bx_stack_reset(struct bx_stack *stack) {

	stack->top = 0;
}
