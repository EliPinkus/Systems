/*
 * Define a bounded buffer containing records that describe the
 * results in a producer thread.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "sthread.h"
#include "bounded_buffer.h"
#include "semaphore.h"

/*
 * The bounded buffer data.
 */
struct _bounded_buffer {
    /* The maximum number of elements in the buffer */
    int length;

    /* The index of the first element in the buffer */
    int first;

    /* The number of elements currently stored in the buffer */
    int count;

    /* The values in the buffer */
    BufferElem *buffer;

    /* Number of spots in buffer */
    Semaphore * open;
    Semaphore * new;

    /* Controls access to buffer. Mutex 1 -> available */
    Semaphore * control;

};


#define EMPTY -1


/*
 * Allocate a new bounded buffer.
 */
BoundedBuffer *new_bounded_buffer(int length) {
    BoundedBuffer *bufp;
    BufferElem *buffer;
    int i;

    /* Allocate the buffer */
    buffer = (BufferElem *) malloc(length * sizeof(BufferElem));
    bufp = (BoundedBuffer *) malloc(sizeof(BoundedBuffer));
    if (buffer == 0 || bufp == 0) {
        fprintf(stderr, "new_bounded_buffer: out of memory\n");
        exit(1);
    }

    /* Initialize */

    memset(bufp, 0, sizeof(BoundedBuffer));

    for (i = 0; i != length; i++) {
        buffer[i].id = EMPTY;
        buffer[i].arg = EMPTY;
        buffer[i].val = EMPTY;
    }

    bufp->length = length;
    bufp->buffer = buffer;

    bufp->open = new_semaphore(length);
    bufp->new = new_semaphore(0);

    /* Buffer starts avalaible */
    bufp->control = new_semaphore(1);

    return bufp;
}

/*
 * Add an integer to the buffer.  Yield control to another
 * thread if the buffer is full.
 */
void bounded_buffer_add(BoundedBuffer *bufp, const BufferElem *elem) {
    /* Wait until the buffer has space */
    semaphore_wait(bufp->open);
    semaphore_wait(bufp->control);

    /* Now the buffer has space.  Copy the element data over.
     * The below instruction are atomic
     */
    int idx = (bufp->first + bufp->count) % bufp->length;
    bufp->buffer[idx].id  = elem->id;
    bufp->buffer[idx].arg = elem->arg;
    bufp->buffer[idx].val = elem->val;

    bufp->count = bufp->count + 1;

    /* update used space. Results in unlocked state */
    semaphore_signal(bufp->new);
    semaphore_signal(bufp->control);
}

/*
 * Get an integer from the buffer.  Yield control to another
 * thread if the buffer is empty.
 */
void bounded_buffer_take(BoundedBuffer *bufp, BufferElem *elem) {
    /* Wait until the buffer has a value to retrieve */
    semaphore_wait(bufp->new);
    semaphore_wait(bufp->control);

    /* Copy the element from the buffer, and clear the record
     *The below instructions are atomic
     */
    elem->id  = bufp->buffer[bufp->first].id;
    elem->arg = bufp->buffer[bufp->first].arg;
    elem->val = bufp->buffer[bufp->first].val;

    bufp->buffer[bufp->first].id  = -1;
    bufp->buffer[bufp->first].arg = -1;
    bufp->buffer[bufp->first].val = -1;

    bufp->count = bufp->count - 1;
    bufp->first = (bufp->first + 1) % bufp->length;

    /* update used space. Results in unlocked state */
    semaphore_signal(bufp->open);
    semaphore_signal(bufp->control);
}
