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
/*
 * Our memory pool will be divided into blocks that each begin with a header
 * that encodes important information about the block. Namely, the size of the
 * memory block will be the magnitude of the 4 byte int and a positive value
 * will correspond to a empty block and a negative value a full block.
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
void init_myalloc()
{

  /*
  * Allocate the entire memory pool, from which our simple allocator will
  * serve allocation requests.
  */

  /*
  * Our pool begins as one large block with a 4 byte int header indicating
  * that the block is free
  */
  mem = (unsigned char *) malloc(MEMORY_SIZE);
  if (mem == 0)
  {
    fprintf(stderr,
        "init_myalloc: could not get %d bytes from the system\n",
          MEMORY_SIZE);
    abort();
  }

  freeptr = mem;
  *((int *)(freeptr)) = MEMORY_SIZE - HEADER_SIZE;

}


/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 *
 * Time complexity: O(n) and implements best fit strategy
 * runtime varies with MEMORY_SIZE
 */
unsigned char *myalloc(int size)
{
  /* seeking smallest sufficient free block */
  unsigned char * minhead;
  int min_size = MEMORY_SIZE;
  freeptr = mem;

  while (freeptr < mem + MEMORY_SIZE) /* range memory bank */
  {
    if (*(int*)(freeptr) > size + HEADER_SIZE) /* block big enough? */
    {
      /* this block smaller than previous optimum && its free */
      if (*(int*)(freeptr) < min_size && *(int*)(freeptr) > 0)
      {
        min_size = *(int*)(freeptr);
        minhead = freeptr;
      }
    }
    /* update start point */
    freeptr += HEADER_SIZE + abs(*(int*)(freeptr));
  }

  if (MEMORY_SIZE > min_size) /*if found suitable bloack */
  {
    *((int*)(minhead + HEADER_SIZE + size)) = *(int*)(minhead) - size
                                                        - HEADER_SIZE;
    *(int*)(minhead) = -size; /* mark block as in use */
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
 * Time complexity for total deallocation and coalescing is O(n)
 *
 * Forward coalescing takes place in constant time but since we don't implement
 * footers the backwards component is linear time
 */
void myfree(unsigned char *oldptr)
{

  freeptr = mem;
  *(int*)(oldptr - HEADER_SIZE) = abs(*(int*)(oldptr - HEADER_SIZE));


  /* Check if subsequent headers indicate free and coalesce */
  while (freeptr < mem + MEMORY_SIZE)
  {
    if (*(int*)(freeptr)>0)
    {
      if (*(int*)(freeptr + *(int*)(freeptr) + HEADER_SIZE) > 0
      && freeptr + *(int*)(freeptr) + HEADER_SIZE < mem + MEMORY_SIZE)
      {
        *(int*)(freeptr) = HEADER_SIZE + *(int*)(freeptr) + *(int*)(freeptr
            + HEADER_SIZE + *(int*)(freeptr));
            continue;
      }
    }
    freeptr = freeptr + HEADER_SIZE + abs(*(int*)(freeptr));
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
