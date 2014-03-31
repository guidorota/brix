/*
 * uniform_allocator.h
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

/**
 * A component for managing a pseudo-dynamic allocation of uniformly sized
 * memory chunks
 */

#ifndef UNIFORM_ALLOCATOR_H_
#define UNIFORM_ALLOCATOR_H_

#include "types.h"

/**
 * Uniform allocator data structure initialization.
 *
 * @param storage Storage byte array
 * @param storage_size Size of the storage byte array in bytes
 * @param chunk_size Size of the memory portions allocated
 *
 * @return Uniform allocator instance pointer, NULL on error
 */
struct bx_ualloc *bx_ualloc_init(void *storage, bx_size storage_size, bx_size chunk_size);

/**
 * Allocates a new chunk of memory returns a pointer to the allocated memory.
 *
 * @param ualloc Uniform allocator instance pointer
 *
 * @return A pointer to the newly allocated memory chunk, NULL on error
 */
void *bx_ualloc_alloc(struct bx_ualloc *ualloc);

/**
 * Deallocates a chunk of memory.
 *
 * @param ualloc Uniform allocator instance pointer
 * @param chunk_pointer Pointer to the memory pointer to deallocate
 *
 * @return 0 upon success, -1 otherwise
 */
bx_int8 bx_ualloc_free(struct bx_ualloc *ualloc, void *chunk_pointer);

/**
 * Returns the remaining number of chunks that can be allocated.
 *
 * @param ualloc Uniform allocator instance pointer
 *
 * @return Remaining uniform allocator capacity, -1 on error
 */
bx_ssize bx_ualloc_remaining_capacity(struct bx_ualloc *ualloc);

/**
 * Return the size of each memory chunk, expressed in bytes.
 *
 * @param ualloc Uniform allocator instance pointer
 *
 * @return Chunk size, -1 on error
 */
bx_ssize bx_ualloc_chunk_size(struct bx_ualloc *ualloc);

#endif /* UNIFORM_ALLOCATOR_H_ */
