/*
 * stack.h
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

#ifndef STACK_H_
#define STACK_H_

#include "types.h"

#define BX_STACK_SIZE 4

#define BX_STACK_STORAGE_SIZE(desired_capacity) (desired_capacity + BX_STACK_SIZE)

#define BX_STACK_PUSH_VARIABLE(stack_pointer, variable) bx_stack_push(stack_pointer, &variable, sizeof variable)
#define BX_STACK_POP_VARIABLE(stack_pointer, variable) bx_stack_pop(stack_pointer, &variable, sizeof variable)

/**
 * Initializes the stack data structure.
 *
 * @param storage Pointer to the storage memory reserved for the stack
 * @param storage_size Size of the storage memory reserved for the stack
 *
 * @return Stack instance pointer, NULL on error
 */
struct bx_stack *bx_stack_init(void *storage, bx_size storage_size);

/**
 * Pushes bytes from a variable the stack.
 *
 * @param stack Stack pointer
 * @param from Starting memory position to push
 * @param size Number of bytes to push
 *
 * @return 0 on success, -1 on error
 */
bx_int8 bx_stack_push(struct bx_stack *stack, const void *from, bx_size size);

/**
 * Pops bytes from the stack to a variable.
 *
 * @param stack Stack pointer
 * @param to Destination memory address
 * @param size Number of bytes to pop
 *
 * @return 0 on success, -1 on error
 */
bx_int8 bx_stack_pop(struct bx_stack *stack, void *to, bx_size size);

/**
 * Returns the current stack size.
 *
 * @param stack Stack pointer
 *
 * return Number of bytes in the stack, -1 on error
 */
bx_ssize bx_stack_size(struct bx_stack *stack);

/**
 * Returns the stack capacity.
 *
 * @param stack Stack pointer
 *
 * @return Stack capacity, -1 on error
 */
bx_ssize bx_stack_capacity(struct bx_stack *stack);

/**
 * Resets the stack.
 *
 * @param stack Stack pointer
 */
bx_int8 bx_stack_reset(struct bx_stack *stack);

#endif /* STACK_H_ */
