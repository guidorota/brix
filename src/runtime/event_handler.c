/*
 * event_handler.c
 * Created on: Mar 27, 2014
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

#include "logging.h"
#include "configuration.h"
#include "runtime/event_handler.h"
#include "runtime/storage.h"
#include "utils/byte_buffer.h"

static bx_uint8 buffer[EV_BUFFER_SIZE];

static bx_int8 invoke_vm_handler(bx_file_id code_id);

bx_int8 bx_ev_invoke(struct bx_event_handler *event_handler) {

	if (event_handler == NULL) {
		return -1;
	}

	switch (event_handler->handler_type) {
	case BX_HANDLER_NATIVE:
		event_handler->handler.native_function();
		break;
	case BX_HANDLER_VM:
		invoke_vm_handler(event_handler->handler.code_file_id);
		break;
	default:
		BX_LOG(LOG_ERROR, "event_handler", "Unexpected handler type "
				"encountered in function 'bx_ev_invoke'");
		return -1;
	}

	return 0;
}

static bx_int8 invoke_vm_handler(bx_file_id code_id) {
	bx_int8 error;
	bx_size data_length;

	error = bx_st_retrieve(code_id, (void *) buffer, EV_BUFFER_SIZE, &data_length);
	if (error != 0) {
		return -1;
	}

	return bx_vm_execute(buffer, data_length);
}
