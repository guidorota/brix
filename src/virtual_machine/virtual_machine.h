/*
 * virtual_machine.h
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

#ifndef VIRTUAL_MACHINE_H_
#define VIRTUAL_MACHINE_H_

#include "types.h"

#define BX_INSTR_IADD 0x0
#define BX_INSTR_ISUB 0x1
#define BX_INSTR_IMUL 0x2
#define BX_INSTR_IDIV 0x3
#define BX_INSTR_IMOD 0x4
#define BX_INSTR_FADD 0x5
#define BX_INSTR_FSUB 0x6
#define BX_INSTR_FMUL 0x7
#define BX_INSTR_FDIV 0x8
#define BX_INSTR_PUSH32 0x9
#define BX_INSTR_LOAD32 0xA //TODO: Complete
#define BX_INSTR_SORE32 0xB //TODO: Complete

//TODO: Negation
//TODO: Boolean operations
//TODO: Comparison
//TODO: Cast
//TODO: Variable storage
//TODO: Branching
//TODO: Stop instruction

bx_int8 bx_vm_virtual_machine_init();

bx_int8 bx_vm_execute(bx_uint8 *code, bx_size code_size);

#endif /* VIRTUAL_MACHINE_H_ */
