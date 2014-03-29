/*
 * pcode_manager.h
 * Created on: Mar 25, 2014
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

#ifndef PCODE_MANAGER_H_
#define PCODE_MANAGER_H_

#include "types.h"

struct bx_pcode;

/**
 * Initialize pcode repository internal data structures.
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_pm_init();

/**
 * Adds a new pcode program into the repository.
 *
 * @param buffer Instruction buffer
 * @param buffer_size Instruction buffer size
 *
 * @return New bx_pcode structure, NULL on failure
 */
struct bx_pcode *bx_pm_add(void *buffer, bx_size buffer_size);

/**
 * Invokes the virtual machine and executes a program.
 *
 * @param pcode Program to execute
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_pm_execute(struct bx_pcode *pcode);

/**
 * Removes a pcode program from the repository.
 * Space occupied by the program is reclaimed and freed for future uses.
 *
 * @param pcode Pointer to the pcode structure to remove
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_pm_remove(struct bx_pcode *pcode);

#endif /* PCODE_MANAGER_H_ */
