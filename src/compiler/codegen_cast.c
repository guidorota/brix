/*
 * codegen_cast.c
 * Created on: Mar 10, 2014
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

#include <stdlib.h>
#include "logging.h"
#include "compiler/codegen_cast.h"

static struct bx_comp_expr *cast_to_int(struct bx_comp_expr *expression);
static struct bx_comp_expr *cast_to_float(struct bx_comp_expr *expression);
static struct bx_comp_expr *cast_to_bool(struct bx_comp_expr *expression);
static struct bx_comp_expr *cast_to_string(struct bx_comp_expr *expression);

struct bx_comp_expr *bx_cgca_cast(struct bx_comp_expr *expression, enum bx_builtin_type type) {

	if (expression == NULL) {
		return NULL;
	}

	switch (type) {
	case BX_INT:
		return cast_to_int(expression);
	case BX_FLOAT:
		return cast_to_float(expression);
	case BX_BOOL:
		return cast_to_bool(expression);
	case BX_STRING:
		return cast_to_string(expression);
	case BX_SUBNET:
		BX_LOG(LOG_ERROR, "compiler", "Cast to type 'subnet' is not allowed.");
		return NULL;
	case BX_STREAM:
		BX_LOG(LOG_ERROR, "compiler", "Cast to type 'subnet' is not allowed.");
		return NULL;
	default:
		BX_LOG(LOG_ERROR, "codegen_cast", "Unknown type encountered in function bx_cgca_cast.");
		return NULL;
	}
}

static struct bx_comp_expr *cast_to_int(struct bx_comp_expr *expression) {
	return NULL; //TODO: Stub
}

static struct bx_comp_expr *cast_to_float(struct bx_comp_expr *expression) {
	return NULL; //TODO: Stub
}

static struct bx_comp_expr *cast_to_bool(struct bx_comp_expr *expression) {
	return NULL; //TODO: Stub
}

static struct bx_comp_expr *cast_to_string(struct bx_comp_expr *expression) {
	return NULL; //TODO: Stub
}
