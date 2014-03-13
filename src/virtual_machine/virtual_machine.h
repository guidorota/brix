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

enum bx_instruction {
	BX_INSTR_IADD,
	BX_INSTR_ISUB,
	BX_INSTR_IMUL,
	BX_INSTR_IDIV,
	BX_INSTR_IMOD,
	BX_INSTR_INEG,
	BX_INSTR_IAND,
	BX_INSTR_IOR,
	BX_INSTR_IXOR,
	BX_INSTR_INOT,
	BX_INSTR_IEQ,
	BX_INSTR_INE,
	BX_INSTR_IGT,
	BX_INSTR_IGE,
	BX_INSTR_ILT,
	BX_INSTR_ILE,
	BX_INSTR_FADD,
	BX_INSTR_FSUB,
	BX_INSTR_FMUL,
	BX_INSTR_FDIV,
	BX_INSTR_FNEG,
	BX_INSTR_FEQ,
	BX_INSTR_FNE,
	BX_INSTR_FGT,
	BX_INSTR_FGE,
	BX_INSTR_FLT,
	BX_INSTR_FLE,
	BX_INSTR_PUSH32,
	BX_INSTR_IPUSH_0,
	BX_INSTR_IPUSH_1,
	BX_INSTR_FPUSH_0,
	BX_INSTR_FPUSH_1,
	BX_INSTR_LOAD32,
	BX_INSTR_STORE32,
	BX_INSTR_DUP32,
	BX_INSTR_JUMP,
	BX_INSTR_JEQZ,
	BX_INSTR_JNEZ,
	BX_INSTR_JGTZ,
	BX_INSTR_JGEZ,
	BX_INSTR_JLTZ,
	BX_INSTR_JLEZ,
	BX_INSTR_NOP,
	BX_INSTR_I2F,
	BX_INSTR_F2I,
	BX_INSTR_HALT
};

bx_int8 bx_vm_virtual_machine_init();

bx_int8 bx_vm_execute(bx_uint8 *code, bx_size code_size);

#endif /* VIRTUAL_MACHINE_H_ */
