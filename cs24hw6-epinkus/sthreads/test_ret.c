/*
 * simple test where each thread runs for a different length of time,
 * and then terminate the thread by returning from the thread-function
 */

#include <stdio.h>
#include "sthread.h"



/*
 * This function prints part 1 of a 4 part secret message a specified number of
 * times :)
 */
static void loop1(void *arg) {
    int arg1 = *((int*)arg);
    for (int i = 0; i < arg1; i++) {
        printf("I\n");
        sthread_yield();
    }
}


/*
 * This function prints part 2 of a 4 part secret message a specified number of
 * times :)
 */
static void loop2(void *arg) {
    int arg1 = *((int*)arg);
    for (int i = 0; i < arg1; i++) {
        printf("LOVE\n");
        sthread_yield();
    }
}

/*
 * This function prints part 3 of a 4 part secret message a specified number of
 * times :)
 */
static void loop3(void *arg) {
    int arg1 = *((int*)arg);
    for (int i = 0; i < arg1; i++) {
        printf("CS\n");
        sthread_yield();
    }
}

/*
 * This function prints part 4 of a 4 part secret message a specified number of
 * times :)
 */
static void loop4(void *arg) {
    int arg1 = *((int*)arg);
    for (int i = 0; i < arg1; i++) {
        printf("24\n\n");
        sthread_yield();
    }
}

/*
 * prints the secret message, each part a specified number of times.
 */
int main(int argc, char **argv) {
	int arg1 = 5;
    int arg2 = 4;
    int arg3 = 3;
    int arg4 = 2;
    sthread_create(loop1, (void*)&arg1);
    sthread_create(loop2, (void*)&arg2);
    sthread_create(loop3, (void*)&arg3);
    sthread_create(loop4, (void*)&arg4);
    sthread_start();
    return 0;
}
