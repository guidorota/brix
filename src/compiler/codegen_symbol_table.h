/*
 * codegen_symbol_table.h
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

#ifndef CODEGEN_SYMBOL_TABLE_H_
#define CODEGEN_SYMBOL_TABLE_H_

#include "types.h"
#include "configuration.h"

enum bx_comp_creation_modifier {
	BX_COMP_EXISTING,
	BX_COMP_NEW
};

struct bx_comp_field_symbol {
	enum bx_builtin_type data_type;
	enum bx_comp_creation_modifier creation_modifier;
	char identifier[DM_FIELD_IDENTIFIER_LENGTH];
};

struct bx_comp_variable_symbol {
	enum bx_builtin_type data_type;
	char identifier[DM_FIELD_IDENTIFIER_LENGTH];
};

bx_int8 bx_cgsy_init();

bx_int8 bx_cgsy_add_field(char *identifier, enum bx_builtin_type data_type,
		enum bx_comp_creation_modifier creation_modifier);

struct bx_comp_field_symbol *bx_cgsy_get_field(char *identifier);

bx_int8 bx_cgsy_reset();

#endif /* CODEGEN_SYMBOL_TABLE_H_ */
