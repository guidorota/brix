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
	BX_INSTR_IADD,		// Integer addition
	BX_INSTR_ISUB,		// Integer subtraction
	BX_INSTR_IMUL,		// Integer multiplication
	BX_INSTR_IDIV,		// Integer division
	BX_INSTR_IMOD,		// Integer division remainder
	BX_INSTR_INEG,		// Integer negation
	BX_INSTR_IAND,		// Integer bitwise and
	BX_INSTR_IOR,		// Integer bitwise or
	BX_INSTR_IXOR,		// Integer bitwise xor
	BX_INSTR_INOT,		// Integer bitwise not
	BX_INSTR_IEQ,		// Integer equality comparison
	BX_INSTR_INE,		// Integer inequality comparison
	BX_INSTR_IGT,		// Integer greater than comparison
	BX_INSTR_IGE,		// Integer greater than or equal comparison
	BX_INSTR_ILT,		// Integer less than comparison
	BX_INSTR_ILE,		// Integer less than or equal comparison
	BX_INSTR_FADD,		// Float addition
	BX_INSTR_FSUB,		// Float subtraction
	BX_INSTR_FMUL,		// Float multiplication
	BX_INSTR_FDIV,		// Float division
	BX_INSTR_FNEG,		// Float negation
	BX_INSTR_FEQ,		// Float equality comparison
	BX_INSTR_FNE,		// Float inequality comparison
	BX_INSTR_FGT,		// Float greater than comparison
	BX_INSTR_FGE,		// Float greater than or equal comparison
	BX_INSTR_FLT,		// Float less than comparison
	BX_INSTR_FLE,		// Float less than or equal comparison
	BX_INSTR_PUSH32,	// Push 32 bit data on the stack
	BX_INSTR_IPUSH_0,	// Push integer constant 0 on the stack
	BX_INSTR_IPUSH_1,	// Push integer constant 1 on the stack
	BX_INSTR_FPUSH_0,	// Push float constant 0 on the stack
	BX_INSTR_FPUSH_1,	// Push float constant 1 on the stack
	BX_INSTR_RLOAD32,	// Load 32 bit reference value on the stack
	BX_INSTR_RSTORE32,	// Store 32 bit stack data into a reference
	BX_INSTR_DUP32,		// Duplicate top 32 bit of stack
	BX_INSTR_JUMP,		// Unconditional jump
	BX_INSTR_JEQZ,		// Jump if 32 bit stack value is zero
	BX_INSTR_JNEZ,		// Jump if 32 bit stack value is not zero
	BX_INSTR_JGTZ,		// Jump if 32 bit stack value is greater than zero
	BX_INSTR_JGEZ,		// Jump if 32 bit stack value is greater or equal to zero
	BX_INSTR_JLTZ,		// Jump if 32 bit stack value is less than zero
	BX_INSTR_JLEZ,		// Jump if 32 bit stack value is less or equal to zero
	BX_INSTR_NOP,		// Do nothing
	BX_INSTR_I2F,		// Convert top stack value from integer to float
	BX_INSTR_F2I,		// Convert top stack value from float to integer
	BX_INSTR_HALT		// Halt the execution of the virtual machine
};

bx_int8 bx_vm_virtual_machine_init();

bx_int8 bx_vm_execute(bx_uint8 *code, bx_size code_size);

#endif /* VIRTUAL_MACHINE_H_ */
