/*
 * basic test will attempt to pass an argument to the thread-functions, to
 * ensure that the argument is positioned properly in the thread’s machine
 * context such that the functions can retrieve and use it.
 */


#include <stdio.h>
#include "sthread.h"
#include <assert.h>
#define UPPER_BOUND 50
#define LOWER_BOUND 0

/*
 * This function increments the argument until UPPER_BOUND
 */
static void loop1(void *arg) {
    int arg1 = *((int*)arg);
    while (arg1 < UPPER_BOUND) {
        printf("Incrementing %d\n", arg1);
        arg1++;
    }
}


/*
 * This function decrements the argument until LOWER_BOUND
 */
static void loop2(void *arg) {
    int arg2 = *((int*)arg);
    while (arg2 > LOWER_BOUND) {
        printf("Decrementing %d\n", arg2);
        arg2--;
    }
}

/*
 * this function runs the above loops to test for passsing arguments correctly
 */
int main(int argc, char **argv) {
    int arg = 25;
    sthread_create(loop1, (void*)&arg);
    sthread_create(loop2, (void*)&arg);
    sthread_start();
    return 0;
}
