/*
 * list.c
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

#include "utils/uniform_allocator.h"
#include "logging.h"
#include <string.h>

/*
 * The current implementation stores the ualloc structure at the beginning of
 * the storage array.
 * The last bytes of the storage array contain a map of the free and used
 * chunks, called Memory Availability Map. The MAM is partitioned in blocks
 * of 32 bits, each of which is used to store the allocation status of 32
 * different memory chunks. Each bit of the MAM represents the availability
 * status of a memory chunk; if set to 1 the chunk is available, if set to zero
 * the chunk has already been taken.
 *
 * +--------+--------------------------------+----------+
 * |        |                                |          |
 * | Ualloc | Element storage                | MAM      |
 * |        |                                |          |
 * +--------+--------------------------------+----------+
 */

#define CHUNK_BASE_POINTER(ualloc_pointer) ((bx_uint8 *) ualloc_pointer + sizeof (struct bx_ualloc))

#define CHUNK_POINTER(ualloc_pointer, index) \
	(void *) (CHUNK_BASE_POINTER(ualloc_pointer) + index * ualloc_pointer->chunk_size)

#define MAM_BLOCK_POINTER(ualloc_pointer, block_number) (bx_uint32 *) ualloc_pointer->mam + block_number

struct bx_ualloc {
	bx_size chunk_size;		///< Element size in bytes
	bx_size size;			///< Current number of elements in the list
	bx_size capacity;		///< List capacity (max number of elements)
	void *mam;				///< Pointer to the first MAM block
	bx_size mam_size;		///< MAM size in blocks of 32 bits
};

static bx_size get_available_chunk_index(struct bx_ualloc *ualloc);
static void set_mam_bit(struct bx_ualloc *ualloc, bx_size index);
static void unset_mam_bit(struct bx_ualloc *ualloc, bx_size index);

struct bx_ualloc *bx_ualloc_init(void *storage, bx_size storage_size, bx_size chunk_size) {
	bx_size available_bytes;
	struct bx_ualloc *ualloc;

	if (storage == NULL || storage_size == 0 || chunk_size == 0) {
		return NULL;
	}

	ualloc = (struct bx_ualloc *) storage;
	ualloc->chunk_size = chunk_size;
	ualloc->size = 0;

	available_bytes = storage_size - sizeof (struct bx_ualloc);
	ualloc->mam_size = available_bytes / (32 * chunk_size + 4);
	ualloc->capacity = 32 * ualloc->mam_size;
	available_bytes -= ualloc->capacity * (32 * chunk_size + 4) + 4;
	if (available_bytes > chunk_size) {
		ualloc->mam_size += 1;
		ualloc->capacity += available_bytes / chunk_size;
	}
	ualloc->mam = (void *) ((bx_uint8 *) storage + storage_size - 4 * ualloc->mam_size);
	memset(ualloc->mam, 0xFF, 4 * ualloc->mam_size);

	if (ualloc->capacity == 0) {
		return NULL;
	}

	return ualloc;
}

void *bx_ualloc_alloc(struct bx_ualloc *ualloc) {
	bx_size index;
	void *chunk_pointer;

	if (ualloc == NULL || ualloc == NULL) {
		return NULL;
	}

	if (ualloc->size == ualloc->capacity) {
		BX_LOG(LOG_DEBUG, "uniform allocator",
				"Cannot add another element: uniform allocator full");
		return NULL;
	}

	index = get_available_chunk_index(ualloc);
	chunk_pointer = CHUNK_POINTER(ualloc, index);
	set_mam_bit(ualloc, index);
	++ualloc->size;

	return chunk_pointer;
}

static bx_size get_available_chunk_index(struct bx_ualloc *ualloc) {
	bx_size i;
	bx_uint32 mam_block_copy;
	bx_size available_chunk_index;

	available_chunk_index = 0;
	i = 0;
	for (i = 0; i < ualloc->mam_size; i++) {
		mam_block_copy = *MAM_BLOCK_POINTER(ualloc, i);
		if ((mam_block_copy & 0xFFFF0000) == 0) {
			available_chunk_index += 16;
			mam_block_copy = mam_block_copy << 16;
		}
		if ((mam_block_copy & 0xFF000000) == 0) {
			available_chunk_index += 8;
			mam_block_copy = mam_block_copy << 8;
		}
		if ((mam_block_copy & 0xF0000000) == 0) {
			available_chunk_index += 4;
			mam_block_copy = mam_block_copy << 4;
		}
		if ((mam_block_copy & 0xC0000000) == 0) {
			available_chunk_index += 2;
			mam_block_copy = mam_block_copy << 2;
		}
		if ((mam_block_copy & 0x80000000) == 0) {
			available_chunk_index += 1;
			mam_block_copy = mam_block_copy << 1;
		}
		if ((mam_block_copy & 0x80000000) != 0) {
			break;
		}
	}

	return 32 * i + available_chunk_index;
}

static void set_mam_bit(struct bx_ualloc *ualloc, bx_size index) {
	bx_uint32 *mam_block;

	mam_block = MAM_BLOCK_POINTER(ualloc, index / 32);
	*mam_block |= 0x00000001 << index % 32;
}

bx_int8 bx_ualloc_free(struct bx_ualloc *ualloc, void *chunk_pointer) {
	bx_size chunk_offset;
	bx_size chunk_index;

	if (ualloc == NULL || chunk_pointer == NULL) {
		return -1;
	}

	if ((bx_uint8 *) chunk_pointer < CHUNK_BASE_POINTER(ualloc)) {
		BX_LOG(LOG_ERROR, "uniform_allocator",
				"Pointer being freed is not a valid chunk pointer");
		return -1;
	}

	chunk_offset = (bx_uint8 *) chunk_pointer - CHUNK_BASE_POINTER(ualloc);
	if (chunk_offset % ualloc->chunk_size != 0) {
		BX_LOG(LOG_ERROR, "uniform_allocator",
				"Pointer being freed is not a valid chunk pointer");
		return -1;
	}

	chunk_index = chunk_offset / ualloc->chunk_size;
	if (chunk_index >= ualloc->capacity) {
		BX_LOG(LOG_ERROR, "uniform_allocator",
				"Pointer being freed is not a valid chunk pointer");
		return -1;
	}

	unset_mam_bit(ualloc, chunk_offset / ualloc->chunk_size);
	ualloc->size -=1;

	return 0;
}

static void unset_mam_bit(struct bx_ualloc *ualloc, bx_size index) {
	bx_uint32 *mam_block;

	mam_block = MAM_BLOCK_POINTER(ualloc, index / 32);
	*mam_block &= ~(0x00000001 << index % 32);
}

bx_ssize bx_ualloc_remaining_capacity(struct bx_ualloc *ualloc) {

	if (ualloc == NULL) {
		return -1;
	}

	return ualloc->capacity - ualloc->size;
}

bx_ssize bx_ualloc_chunk_size(struct bx_ualloc *ualloc) {

	if (ualloc == NULL) {
		return -1;
	}

	return ualloc->chunk_size;
}
