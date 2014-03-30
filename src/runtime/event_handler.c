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
#include "runtime/pcode_manager.h"

enum bx_handler_type {
	BX_HANDLER_NATIVE,	///< Native C handler
	BX_HANDLER_PCODE		///< Virtual machine handler
};

struct bx_event_handler {
	enum bx_handler_type handler_type;
	union bx_event_handler_data {
		native_handler native_function;
		struct bx_pcode *pcode;
	} handler;
};

static bx_size handler_count;
//static bx_uint8 event_list_storage[EV_MAX_HANDLER_NUMBER];

bx_int8 bx_ev_init() {
	handler_count = 0;

	return 0;
}

bx_int8 bx_ev_invoke_handler(struct bx_event_handler *event_handler) {

	if (event_handler == NULL) {
		return -1;
	}

	switch (event_handler->handler_type) {
	case BX_HANDLER_NATIVE:
		event_handler->handler.native_function();
		return 0;
	case BX_HANDLER_PCODE:
		return bx_pm_execute(event_handler->handler.pcode);
	default:
		BX_LOG(LOG_ERROR, "event_handler", "Unexpected handler type "
				"encountered in function 'bx_ev_invoke_handler'");
		return -1;
	}
}
