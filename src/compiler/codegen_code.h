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

/**
 *
 */
typedef bx_size bx_comp_label;

/**
 * Creates a new bx_comp_code structure
 *
 * @return New bx_comp_code structure
 */
struct bx_comp_code *bx_cgco_create();

/**
 * Creates a copy of the bx_comp_code structure passed as parameter
 *
 * @param code Structure bx_comp_code to copy
 *
 * @return Copy of the struct bx_comp_code passed as parameter
 */
struct bx_comp_code *bx_cgco_copy(struct bx_comp_code *code);

/**
 * Destroys the bx_comp_code structure passed as parameter reclaiming memory.
 *
 * @param code Structure bx_comp_code to destroy
 */
void bx_cgco_destroy(struct bx_comp_code *code);

/**
 * Appends a new instruction to the bx_comp_code structure
 *
 * @param code Target bx_comp_code structure
 * @param instruction Instructino to add
 *
 * @return Starting address of the instruction in the bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_add_instruction(struct bx_comp_code *code, enum bx_instruction instruction);

/**
 * Appends an identifier to the bx_comp_code structure
 *
 * @param code Target bx_comp_code structure
 * @param identifier Identifier to add
 *
 * @return Starting address of the identifier in the bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_add_identifier(struct bx_comp_code *code, char *identifier);

/**
 * Appends an integer constant to the bx_comp_code structure
 *
 * @param code Target bx_comp_code structure
 * @param value Identifier to add
 *
 * @return Starting address of the integer constant in the bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_add_int_constant(struct bx_comp_code *code, bx_int32 value);

/**
 * Appends a float constant to the bx_comp_code structure
 *
 * @param code Target bx_comp_code structure
 * @param value Float to add
 *
 * @return Starting address of the float constant in the bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_add_float_constant(struct bx_comp_code *code, bx_float32 value);

/**
 * Appends a boolean constant to the bx_comp_code structure
 *
 * @param code Target bx_comp_code structure
 * @param value boolean to add
 *
 * @return Starting address of the boolean constant in the bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_add_bool_constant(struct bx_comp_code *code, bx_uint32 value);

/**
 * Appends an address (16 bit length number) to the bx_comp_structure
 *
 * @param code Target bx_comp_code structure
 * @param address Address to add
 *
 * @return Starting address of the address in the bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_add_address(struct bx_comp_code *code, bx_int16 address);

/**
 * Appends the code content of the source bx_comp_code structure to the
 * destination bx_comp_code structure
 *
 * @param destination Destination structure
 * @param source Source structure
 *
 * @return Starting address of the appended code in the destinatino bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_append_code(struct bx_comp_code *destination, struct bx_comp_code *source);

/**
 * Replaces the code in destination with the code in source
 *
 * @param destination Destination structure
 * @param source Source structure
 *
 * @return Starting address of the appended code in the destination bx_comp_code structure, -1 on failure
 */
bx_ssize bx_cgco_replace_code(struct bx_comp_code *destination, struct bx_comp_code *source);

/**
 * Creates a new label.
 * This function adds 2 empty bytes to the bx_comp_code structure and returns
 * their starting address (a label). This label can be later used to set
 * or change the value of those bytes
 *
 * @param code Target bx_comp_code structure
 *
 * @return
 */
bx_comp_label bx_cgco_create_address_label(struct bx_comp_code *code);

/**
 * Sets the address passed as parameter at the position referenced by the label
 *
 * @param code Target bx_comp_code structure
 * @param label Target position in the code structure
 * @param address Address value to set at the label position in the code
 */
void bx_cgco_set_address_label(struct bx_comp_code *code, bx_comp_label label, bx_uint16 address);

#endif /* CODEGEN_CODE_H_ */
