/*
 * event_handler.h
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

#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

#include "types.h"
#include "runtime/pcode_manager.h"

typedef void (*native_handler)();

struct bx_event_handler;

/**
 * Initializes the internal data structures of the event_handler.
 *
 * @return 0 on successful initialization, -1 on error
 */
bx_int8 bx_ev_init();

/**
 * Creates an event handler based on a native C funtion.
 *
 * @param function Handler function.
 *
 * @return Event handler instance pointer, NULL on error
 */
struct bx_event_handler *bx_ev_create_native_handler(native_handler function);

/**
 * Creates an event handler based on a pcode routine.
 * The event_handler invokes the pcode_manager to store the pcode instructions
 * inside a bx_pcode struct.
 *
 * @param buffer Pcode instruction buffer
 * @param buffer_size Pcode instruction buffer size
 *
 * @return Event handler instance pointer, NULL on error
 */
struct bx_event_handler *bx_ev_create_pcode_handler(void *buffer, bx_size buffer_size);

/**
 * Invokes the native handler passed as parameter.
 *
 * @param event_handler Event handler instance to invoke
 *
 * @return 0 on successful invocation, -1 on error
 */
bx_int8 bx_ev_invoke_handler(struct bx_event_handler *event_handler);

/**
 * Removes an event handler instance, and frees memory for new instances.
 *
 * @param event_handler Event handler instance to remove
 *
 * @return 0 on successful removal, -1 on error
 */
bx_int8 bx_ev_remove_handler(struct bx_event_handler *event_handler);

#endif /* EVENT_HANDLER_H_ */
