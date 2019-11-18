/* realloc.c - C standard library routine.
   Copyright (c) 1989, 1993  Michael J. Haertel
   You may redistribute this library under the terms of the
   GNU Library General Public License (version 2 or any later
   version) as published by the Free Software Foundation.
   THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
   WARRANTY.  IN PARTICULAR, THE AUTHOR MAKES NO REPRESENTATION OR
   WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY OF THIS
   SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE. */

#ifdef XXX
#include <limits.h>
#include <stddef.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "malloc.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))

/* Resize the given region to the new size, returning a pointer
   to the (possibly moved) region.  This is optimized for speed;
   some benchmarks seem to indicate that greater compactness is
   achieved by unconditionally allocating and copying to a
   new region. */
void *
__realloc(void *ptr, size_t size)
{
    void *result, *previous;
    int block, blocks, type;
    int oldlimit;

    if (!ptr)
	return __malloc(size);
    if (!size) {
	__free(ptr);
	return __malloc(0);
    }

    block = BLOCK(ptr);

    switch (type = _heapinfo[block].busy.type) {
    case 0:
	/* Maybe reallocate a large block to a small fragment. */
	if (size <= BLOCKSIZE / 2) {
	    if ((result = __malloc(size)) != NULL) {
	    	memcpy(result, ptr, size);
	    	__free(ptr);
	    }
	    return result;
	}

	/* The new size is a large allocation as well; see if
	   we can hold it in place. */
	blocks = BLOCKIFY(size);
	if (blocks < _heapinfo[block].busy.info.size) {
	    /* The new size is smaller; return excess memory
	       to the free list. */
	    _heapinfo[block + blocks].busy.type = 0;
	    _heapinfo[block + blocks].busy.info.size
		= _heapinfo[block].busy.info.size - blocks;
	    _heapinfo[block].busy.info.size = blocks;
	    __free(ADDRESS(block + blocks));
	    return ptr;
	} else if (blocks == _heapinfo[block].busy.info.size)
	    /* No size change necessary. */
	    return ptr;
	else {
	    /* Won't fit, so allocate a new region that will.  Free
	       the old region first in case there is sufficient adjacent
	       free space to grow without moving. */
	    blocks = _heapinfo[block].busy.info.size;
	    /* Prevent free from actually returning memory to the system. */
	    oldlimit = _heaplimit;
	    _heaplimit = 0;
	    __free(ptr);
	    _heaplimit = oldlimit;
	    result = __malloc(size);
	    if (!result) {
		/* Now we're really in trouble.  We have to unfree
		   the thing we just freed.  Unfortunately it might
		   have been coalesced with its neighbors. */
		if (_heapindex == block)
		    __malloc(blocks * BLOCKSIZE);
		else {
		    previous = __malloc((block - _heapindex) * BLOCKSIZE);
		    __malloc(blocks * BLOCKSIZE);
		    __free(previous);
		}	    
		return NULL;
	    }
	    if (ptr != result)
		memmove(result, ptr, blocks * BLOCKSIZE);
	    return result;
	}
	break;

    default:
	/* Old size is a fragment; type is logarithm to base two of
	   the fragment size. */
	if ((size > 1 << (type - 1)) && (size <= 1 << type))
	    /* New size is the same kind of fragment. */
	    return ptr;
	else {
	    /* New size is different; allocate a new space, and copy
	       the lesser of the new size and the old. */
	    result = __malloc(size);
	    if (!result)
		return NULL;
	    memcpy(result, ptr, MIN(size, 1 << type));
	    __free(ptr);
	    return result;
	}
	break;
    }
}
