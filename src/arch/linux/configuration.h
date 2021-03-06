/*
 * configuration.h
 * Created on: Feb 16, 2014
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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

// Define native byte order (NATIVE_LITTLE_ENDIAN or NATIVE_BIG_ENDIAN)
#define NATIVE_LITTLE_ENDIAN

// Virtual machine
#define VM_STACK_SIZE 512
#define VM_VARIABLE_TABLE_SIZE 512

// Pcode repository
#define PR_CODE_STORAGE_SIZE 4092

// Document manager
#define DM_MAX_FIELD_NUMBER 512
#define DM_FIELD_IDENTIFIER_LENGTH 16
#define DM_MMAP_STORAGE_SIZE 512

// Timer
#define TM_TICK_PERIOD_MS 125
#define TM_TIMER_STORAGE_SIZE 512

// Event handler
#define EV_HANDLER_STORAGE_SIZE 512

#endif /* CONFIGURATION_H_ */
