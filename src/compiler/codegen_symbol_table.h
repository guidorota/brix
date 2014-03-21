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
#include "utils/linked_list.h"

enum bx_comp_symbol_type {
	BX_COMP_FIELD_SYMBOL,
	BX_COMP_VARIABLE_SYMBOL
};

enum bx_comp_creation_modifier {
	BX_COMP_EXISTING,
	BX_COMP_NEW
};

struct bx_comp_symbol {
	char identifier[DM_FIELD_IDENTIFIER_LENGTH];
	enum bx_builtin_type data_type;
	enum bx_comp_symbol_type symbol_type;
	union bx_symbol_data {
		enum bx_comp_creation_modifier creation_modifier;
		bx_uint16 variable_number;
	} bx_symbol_data;
};

struct bx_comp_scope {
	struct bx_comp_scope *parent_scope;
	struct bx_linked_list *variable_list;
};

struct bx_comp_symbol_table {
	struct bx_comp_scope *current_scope;	///< Current variable scope
	struct bx_linked_list *scope_list;		///< List of all scopes
	struct bx_linked_list *field_list;		///< List of fields
	bx_int16 current_variable_number;		///< Current variable number
};

/**
 * Creates a new symbol table context.
 * The symbol table context is used for storing references to variables
 * and fields, and to allocate variable numeric references.
 * Variables are stored in a data structure composed of a hierarchy of scopes.
 * Each scope stores variable references encountered in a specific code block.
 * The scope hierarchy is deepened as the parser finds new code blocks.
 * Fields are stored in a flat data structure, since their namespace common
 * for the entire device.
 *
 * @return New symbol table context, NULL on failure
 */
struct bx_comp_symbol_table *bx_cgsy_create_symbol_table();

/**
 * Destroys the symbol table context passed as parameter and reclaims memory.
 *
 * @param symbol_table Symbol table to destroy
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgsy_destroy_symbol_table(struct bx_comp_symbol_table *symbol_table);

/**
 * Adds a new scope to the scope hierarchy, and sets it as the current_scope.
 *
 * @param symbol_table Target symbol table context.
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgsy_scope_down(struct bx_comp_symbol_table *symbol_table);

/**
 * Moves up one position on the scope hierarchy.
 *
 * @param symbol_table Target symbol table context
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgsy_scope_up(struct bx_comp_symbol_table *symbol_table);

/**
 * Adds a new field variable to the symbol table context.
 *
 * @param symbol_table Target symbol table
 * @param identifier Field identifier
 * @param data_type Field data type
 * @param creation_modifier Field creation modifier (new or existing)
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgsy_add_field(struct bx_comp_symbol_table *symbol_table, char *identifier,
		enum bx_builtin_type data_type, enum bx_comp_creation_modifier creation_modifier);

/**
 * Adds a new variable to the current scope.
 *
 * @param symbol_table Target symbol table
 * @param identifier Field identifier
 * @param data_type Field data type
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_cgsy_add_variable(struct bx_comp_symbol_table *symbol_table, char *identifier,
		enum bx_builtin_type data_type);

/**
 * Get field information by identifier.
 *
 * @param symbol_table Current symbol table
 * @param identifier Identifier of the field to retrieve
 *
 * @return Field information, NULL on error or identifier not found
 */
struct bx_comp_symbol *bx_cgsy_get_field(struct bx_comp_symbol_table *symbol_table, char *identifier);

/**
 * Get variable information by identifier.
 * The variable identifier is searched inside the current and higher scopes.
 *
 * @param symbol_table Current symbol table
 * @param identifier Identifier of the variable to retrieve
 *
 * @return Variable information, NULL on error or identifier not found
 */
struct bx_comp_symbol *bx_cgsy_get_variable(struct bx_comp_symbol_table *symbol_table, char *identifier);

#endif /* CODEGEN_SYMBOL_TABLE_H_ */
