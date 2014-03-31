/*
 * fmemopen.c
 * Created on: Mar 17, 2014
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

#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "logging.h"
#include "utils/fmemopen.h"

struct bx_comp_fmem {
	bx_size size;
	bx_size position;
	bx_uint8 *data;
};

static int readfn(void *handler, char *buf, int size) {
	struct bx_comp_fmem *instance;

	if (handler == NULL || buf == NULL) {
		return -1;
	}

	instance = (struct bx_comp_fmem *) handler;
	if (instance->position + size > instance->size) {
		size = instance->size - instance->position;
	}

	memcpy(buf, instance->data + instance->position, size);
	instance->position += size;

	return size;
}

static int writefn(void *handler, const char *buf, int size) {
	return -1;
}

static fpos_t seekfn(void *handler, fpos_t offset, int whence) {
	struct bx_comp_fmem *instance;
	bx_size new_position;

	if (handler == NULL) {
		return -1;
	}

	instance = (struct bx_comp_fmem *) handler;
	switch (whence) {
	case SEEK_SET:
		new_position = offset;
		break;

	case SEEK_CUR:
		new_position = instance->position + offset;
		break;

	case SEEK_END:
		new_position = instance->size - offset;
		break;

	default:
		return -1;
	}

	if (new_position > instance->size) {
		return -1;
	}

	instance->position = new_position;
	return new_position;
}

static int closefn(void *instance) {
	free(instance);
	return 0;
}

FILE *fmemopen(void *buf, size_t size, const char *mode) {
	struct bx_comp_fmem *instance;

	if (buf == NULL) {
		return NULL;
	}

	if (memcmp(mode, "r", 1)) {
		BX_LOG(LOG_ERROR, "fmemopen", "BX fmemopen only supports mode 'r'");
		return NULL;
	}

	instance = malloc(sizeof *instance);

	instance->size = size;
	instance->data = buf;
	instance->position = 0;
	return funopen((void *) instance, &readfn, &writefn, &seekfn, &closefn);
}
