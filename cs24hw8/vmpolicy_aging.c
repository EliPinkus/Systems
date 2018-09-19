/*============================================================================
 * Implementation of the AGING page replacement policy.
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



/* struct for page_age */
typedef struct page_age_t {
    page_t page;
    uint32_t age;
} page_age_t;


/*============================================================================
 * "Loaded Pages" Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose a random page to evict very easily.
 */

typedef struct loaded_pages_t {
    /* The maximum number of pages that can be resident in memory at once. */
    int max_resident;

    /* The number of pages that are currently loaded.  This can initially be
     * less than max_resident.
     */
    int num_loaded;

    /* This is the array of pages that are actually loaded.  Note that only the
     * first "num_loaded" entries are actually valid.
     */
    struct page_age_t *pages[];
} loaded_pages_t;


/*============================================================================
 * Policy Implementation
 */


/* The list of pages that are currently resident. */
static loaded_pages_t *loaded;


/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using AGING eviction policy.\n\n");

    loaded = malloc(sizeof(loaded_pages_t) + max_resident * sizeof(page_age_t));
    if (loaded) {
        loaded->max_resident = max_resident;
        loaded->num_loaded = 0;
    }

    /* Return nonzero if initialization succeeded. */
    return (loaded != NULL);
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    for (int i = 0; i < loaded->num_loaded; i++) {
        free(loaded->pages[i]);
    }

    free(loaded);
    loaded = NULL;
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    assert(loaded->num_loaded < loaded->max_resident);
    assert(is_page_resident(page));

    /* alloc for the struct */
    page_age_t *p_age = (page_age_t *) malloc(sizeof(page_age_t));

    if (!p_age) {
        fprintf(stderr, "Failed to alloc page_age struct");
        abort();
    }
    /* top bit 1 */
    p_age->page = page;
    p_age->age = 0x80000000;
    loaded->pages[loaded->num_loaded] = p_age;
    loaded->num_loaded++;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* update */
    for (int i = 0; i < loaded->num_loaded; i++) {

        loaded->pages[i]->age >>= 1;

        if (is_page_accessed(loaded->pages[i]->page)) {
            /* apply bitmask */
            loaded->pages[i]->age |= 0x80000000;
            clear_page_accessed(loaded->pages[i]->page);

            /* update permission */
            /*set_page_permission(loaded->pages[i]->page, PAGEPERM_NONE);*/
        }

    }
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  
 */
page_t choose_and_evict_victim_page(void) {
    /* search for lowest age */

    int i_victim = 0;
    uint32_t low = loaded->pages[0]->age;;
    page_t victim;

    for (int i = 1; i < loaded->num_loaded; i++) {
        if (loaded->pages[i]->age < low) {
            i_victim = i;
            low = loaded->pages[i]->age;
        }
    }
    victim = (loaded->pages[i_victim])->page;



    /* Shrink the collection of loaded pages now, by moving the last page in the
     * collection into the spot that the victim just occupied.
     */
    loaded->num_loaded--;
    free(loaded->pages[i_victim]);
    loaded->pages[i_victim] = loaded->pages[loaded->num_loaded];

#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
