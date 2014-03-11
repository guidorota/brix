/*
 * codegen_code.h
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

#ifndef CODEGEN_CODE_H_
#define CODEGEN_CODE_H_

#include "types.h"
#include "virtual_machine/virtual_machine.h"

struct bx_comp_code {
	void *data;
	bx_size size;
	bx_size capacity;
};

typedef bx_size bx_comp_label;

struct bx_comp_code *bx_cgco_create();

void bx_cgco_destroy(struct bx_comp_code *code);

bx_ssize bx_cgco_add_instruction(struct bx_comp_code *code, enum bx_instruction instruction);

bx_ssize bx_cgco_add_identifier(struct bx_comp_code *code, char *identifier);

bx_ssize bx_cgco_add_int_constant(struct bx_comp_code *code, bx_int32 value);

bx_ssize bx_cgco_add_address(struct bx_comp_code *code, bx_int16 address);

bx_ssize bx_cgco_add_float_constant(struct bx_comp_code *code, bx_float32 value);

bx_ssize bx_cgco_add_bool_constant(struct bx_comp_code *code, bx_uint32 value);

bx_ssize bx_cgco_append_code(struct bx_comp_code *code, struct bx_comp_code *append);

bx_comp_label bx_cgco_create_address_label(struct bx_comp_code *code);

void bx_cgco_set_address_label(struct bx_comp_code *code, bx_comp_label label, bx_uint16 address);

#endif /* CODEGEN_CODE_H_ */
