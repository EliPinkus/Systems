/*
 * General implementation of semaphores.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "sthread.h"
#include "semaphore.h"
#include "queue.h"
#include "glue.h"

/*
 * The semaphore data structure contains TODO
 */
struct _semaphore {
    int val;
    Queue * blocked;
};

/************************************************************************
 * Top-level semaphore implementation.
 */

/*
 * Allocate a new semaphore.  The initial value of the semaphore is
 * specified by the argument.
 */
Semaphore *new_semaphore(int init) {
    Semaphore *semp = malloc(sizeof(Semaphore));
    if (semp == NULL) {
        printf("ERROR:  couldn't allocate %u bytes!\n",
               (unsigned int) sizeof(Semaphore));
        exit(1);
    }

    semp->val = init;

    Queue *blocked = malloc(sizeof(Queue));
    if (blocked == NULL) {
        printf("ERROR:  couldn't allocate %u bytes!\n",
               (unsigned int) sizeof(Queue));
        exit(1);
    }
    semp->blocked = blocked;

    return semp;
}

/*
 * Decrement the semaphore.
 * This operation must be atomic, and it blocks iff the semaphore is zero.
 */
void semaphore_wait(Semaphore *semp) {
    /* lock is called such that camparison and the decrementing are done
     * atomically
     */
     __sthread_lock();

     /* loop until nonzero value of semaphore, blocking threads and trying
      * other threads
      */
     while (semp->val == 0) {
         queue_append(semp->blocked, sthread_current());
         sthread_block();
     }

     semp->val -= 1;
     __sthread_unlock();
}

/*
 * Increment the semaphore.
 * This operation must be atomic.
 */
void semaphore_signal(Semaphore *semp) {

    __sthread_lock();
    semp->val += 1;

    if (!queue_empty(semp->blocked)) {
        sthread_unblock(queue_take(semp->blocked));
    }

    __sthread_unlock();
}
