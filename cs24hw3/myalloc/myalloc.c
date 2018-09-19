/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;
unsigned char *freeptr;
#define HEADER_SIZE sizeof(int)
#define FOOTER_SIZE sizeof(int)
/*
 * Our memory pool will be divided into blocks that each begin with a header
 * and end with a footer that each encodes important information about the
 * block. Namely, the size of the
 * memory block will be the magnitude of the 4 byte int header and footer and a
 * positive value will correspond to a empty block and a negative value a
 * full block.
 */

/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */

 /*
 * Our pool begins as one large block with a 4 byte int header indicating
 * that the block is free and a 4 byte int footer with the same information.
 */
void init_myalloc() {

  /*
  * Allocate the entire memory pool, from which our simple allocator will
  * serve allocation requests.
  */


  mem = (unsigned char *) malloc(MEMORY_SIZE);
  if (mem == 0) {
    fprintf(stderr,
        "init_myalloc: could not get %d bytes from the system\n",
          MEMORY_SIZE);
    abort();
  }

  freeptr = mem;
  /* initialize header and footer for first block */
  *((int*)(freeptr)) = MEMORY_SIZE - HEADER_SIZE - FOOTER_SIZE;
  *((int*)(freeptr + MEMORY_SIZE - FOOTER_SIZE)) = MEMORY_SIZE - HEADER_SIZE -
                  FOOTER_SIZE;

}


/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 *
 * Time complexity: O(n) and implements best fit strategy
 * runtime varies with MEMORY_SIZE
 */
unsigned char *myalloc(int size) {
  /* seeking smallest sufficient free block */
  unsigned char *minhead;
  int min_size = MEMORY_SIZE;
  freeptr = mem;

  /* range memory bank */
  while (freeptr < mem + MEMORY_SIZE) {
    /* block big enough? */
    if (*((int*)(freeptr)) >= size + (int)HEADER_SIZE + (int)FOOTER_SIZE
                  && *((int*)(freeptr)) < min_size) {
      /* this block smaller than previous optimum && its free */
      min_size = *((int*)(freeptr));
      minhead = freeptr;
    }
    /* update start point */
    freeptr += HEADER_SIZE + abs(*((int*)(freeptr))) + FOOTER_SIZE;
  }

  /* if found suitable bloack */
  if (MEMORY_SIZE > min_size)  {


    /* mark block as in use */
    *((int*)(minhead)) = -1*size;
    *((int*)(minhead + HEADER_SIZE + size)) = -1*size;

    freeptr = minhead + size + FOOTER_SIZE + HEADER_SIZE;

    int free_size = min_size - size - HEADER_SIZE - FOOTER_SIZE;
    *((int*)freeptr) = free_size;
    *((int*)(freeptr + free_size + FOOTER_SIZE)) = free_size;
    return minhead + HEADER_SIZE;
  }



  fprintf(stderr, "myalloc: cannot service request of size %d with"
          " %lx bytes allocated\n", size, (freeptr - mem));
  return (unsigned char *) 0;
}


/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc().
 *
 * Time complexity for total deallocation and coalescing is constant time
 *
 * We implement headers and footers to allow constant time deallocation
 * as we can coelesce right and left in constant time each.
 *
 */
void myfree(unsigned char *oldptr) {
  int size_freed;
  int size_next;

  size_freed =  abs(*((int*)(oldptr - HEADER_SIZE)));

  /* header and footer update to free */
  *((int*)(oldptr - HEADER_SIZE)) = size_freed;
  *((int*)(oldptr + size_freed))  = size_freed;

  size_next = *((int*)(oldptr + size_freed + FOOTER_SIZE));

  /* Coalesce forward if avalaible */
  if (size_next > 0) {
    int size_new = size_freed + size_next + HEADER_SIZE + FOOTER_SIZE;
    /* header update */
    /* new size = previous size plus current size */

    /* update oldptr in case we need to do forward coelesce as well */
    *((int*)(oldptr - HEADER_SIZE)) = size_new;
    *((int*)(oldptr + size_new)) = size_new;

    size_freed = size_new;
  }


  size_next = *((int*)(oldptr - HEADER_SIZE - FOOTER_SIZE));

  /* Coalesce backward if avalaible */
  if (size_next > 0) {
    int size_new = size_freed + size_next + HEADER_SIZE + FOOTER_SIZE;
    /* header update */
    *((int*)(oldptr - 2*HEADER_SIZE - FOOTER_SIZE - size_next)) = size_new;
    *((int*)(oldptr + size_freed)) = size_new;
  }
}

/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This function mostly
 * ensures that the test program doesn't leak memory, so it's easy to check
 * if the allocator does.
 */
void close_myalloc() {
  free(mem);
}
