/*============================================================================
 * Implementation of the FIFO page replacement policy.
 *
 * We don't mind if paging policies use malloc() and free(), just because it
 * keeps things simpler.  In real life, the pager would use the kernel memory
 * allocator to manage data structures, and it would endeavor to allocate and
 * release as little memory as possible to avoid making the kernel slow and
 * prone to memory fragmentation issues.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vmpolicy.h"


/*============================================================================
 * NODE Data Structure
 *
 * This is anode in the queue
 */

typedef struct node {
    page_t page;
    struct node *next;
} node;


/* Global variables */
static node *head;
static node *tail;

/*============================================================================
 * Policy Implementation
 */



/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using FIFO eviction policy.\n\n");

    /* init queue */
    head = NULL;
    tail = NULL;

    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    head = NULL;
    tail = NULL;
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    /* alloc space foe node */
    node *new = (node *) malloc(sizeof(node));
    if (!new) {
        fprintf(stderr, "Couldn't allocate notde");
    }

    /* add node to end to queue */
    new->page = page;
    new->next = NULL;

    /* queue could be empty */
    if (head == NULL && tail == NULL) {
        head = new;
    }
    else {
        tail->next = new;
    }

    tail = new;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /*nothing!*/
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.
 */
page_t choose_and_evict_victim_page(void) {
    page_t victim = head->page;

    node *temp = head;
    head = head->next;
    free(temp);
    temp = NULL;

#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
