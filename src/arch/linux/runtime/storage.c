/*
 * storage.c
 * Created on: Mar 25, 2014
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
#include "logging.h"
#include "runtime/storage.h"
#include "utils/memory_utils.h"
#include "utils/linked_list.h"

struct bx_st_file {
	bx_file_id file_id;
	void *data;
	bx_size data_length;
};

static bx_file_id current_index;
static struct bx_linked_list *file_list;

static bx_int8 file_id_equals(struct bx_st_file *list_element, bx_file_id *file_id) {
	return list_element->file_id == *file_id;
}

bx_int8 bx_st_init(void *param) {
	current_index = 0;

	return 0;
}

bx_int8 bx_st_persist(void *data, bx_size data_length, bx_file_id *file_id) {
	struct bx_st_file *file;
	void *result;

	file = BX_MALLOC(struct bx_st_file);
	if (file == NULL) {
		return -1;
	}

	file->data = malloc(data_length);
	if (file->data == NULL) {
		free(file);
		return -1;
	}

	*file_id = current_index++;
	file->file_id = *file_id;
	memcpy(file->data, data, data_length);
	file->data_length = data_length;

	result = bx_llist_add(&file_list, (void *) file);
	if (result == NULL) {
		free(file->data);
		free(file);
		return -1;
	}

	return 0;
}

bx_int8 bx_st_retrieve(bx_file_id file_id, void *buffer, bx_size buffer_size, bx_size *bytes_read) {
	struct bx_st_file *file;

	file = (struct bx_st_file *) bx_llist_find_equals(
			file_list, (void *) &file_id, (bx_llist_equals) file_id_equals);
	if (file == NULL) {
		BX_LOG(LOG_ERROR, "storage", "File '%zu' not found.", file_id);
		return -1;
	}

	if (file->data_length > buffer_size) {
		return -1;
	}

	memcpy(buffer, file->data, file->data_length);
	*bytes_read = file->data_length;

	return 0;
}

bx_int8 bx_st_delete(bx_file_id file_id) {
	struct bx_st_file *file;

	file = bx_llist_remove_equals(&file_list, (void *) &file_id, (bx_llist_equals) file_id_equals);
	if (file == NULL) {
		BX_LOG(LOG_ERROR, "storage", "File '%zu' not found.", file_id);
		return -1;
	}

	free(file->data);
	free(file);
	return 0;
}
