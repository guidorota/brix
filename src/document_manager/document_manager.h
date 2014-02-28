/*
 * document_manager.h
 * Created on: Feb 21, 2014
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

#ifndef DOCUMENT_MANAGER_H_
#define DOCUMENT_MANAGER_H_

/**
 * Document manager.
 * The document manager stores information regarding the documents and the fields
 */

#include "types.h"
#include "configuration.h"

enum bx_field_type {
	INT,
	FLOAT
};

struct bx_document_field {
	enum bx_field_type type;
	void *private_data;
	bx_int8 (*get)(struct bx_document_field *instance, void *data);
	bx_int8 (*set)(struct bx_document_field *instance, void *data);
};

/**
 * Initializes the document manager.
 *
 * @return 0 on success, -1 on failure.
 */
bx_int8 bx_dm_document_manager_init(void);

/**
 * Adds a new field to the document manager.
 * Upon addition the document manager performs a copy of the field data
 * in its internal data structures.
 *
 * @param field Field to add
 * @param identifier Name of the field to add
 *
 * @return 0 on success, -1 on failure
 */
bx_int8 bx_dm_add_field(struct bx_document_field *field, char *identifier);

bx_int8 bx_dm_invoke_get(char *field_identifier, void *data);

bx_int8 bx_dm_invoke_set(char *field_identifier, void *data);

#endif /* DOCUMENT_MANAGER_H_ */
