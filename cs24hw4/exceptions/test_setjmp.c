#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "my_setjmp.h"

static jmp_buf env;

// f() is not used in main so we must declare here.
int f();

/*
 * checks return of setjmp to make sure it is 1 when longjmp
 * passes 0
 */
int ret1_test() {

    switch(setjmp(env)) {
        case 0 :
            longjmp(env, 0);
        case 1 :
            return 1;
        default :
            return 0;
    }
}

/*
 * checks that longjmp(env, n) for n != 0 causes setjmp to return n
 */
int ret_n_test(int n) {

    int ret = setjmp(env);
    switch(ret) {
        case 0 :
            longjmp(env, n);
        default :
            if (ret == n) {
                return 1;
            }
            else {
                return 0;
            }
    }

}

/*
 * tests the ability of setjmp to span multiple levels of function invokation.
 */
int test_mul_funcs() {

    switch(setjmp(env)) {
        case 0 :
            f();
        case 1:
            return 1;
        default :
            return 0;
    }
}

/*
 * function to call from test_mul_funcs
 */
int f() {
    // do stuff
    int x = 1;
    int y = 2 + x;
    if (y < 2) {
        longjmp(env, 1); // jump back to super call
    }
    return y;
}


/*
 * tests for obvious stack corruption by declaring local variables at
 * various points relative to setjmp calls and checks to see if memory
 * is overwritten
 */
int stack_test(int n, int m) {
    int a = n;
    int b;

    switch(setjmp(env)) {
        case 0 :
            b = m;
            longjmp(env, 0);
        case 1 :
            if (a == n && b == m) {
                return 1;
            }
            else {
                return 0;
            }
    }
}

/*
 * Generates random int 0<n<max
 * adapted from problem set3 test code
 */
int random_int(int max) {
    int rnd;
    int result;
    rnd = rand();
    result = 1 + (int) ((long long) max * (long long) rnd /
                                        ((long long) RAND_MAX + 1.0));

    return result;
}


int main(){
    printf("TESTING longjmp(buf, 0) returns 1:     \n");
    if (ret1_test()) {
        printf("PASSED \n\n\n");
    }

    printf("TESTING longjmp(buf, n) returns n for n!=0 \n");
    printf("Testing with 3 sets of random ints \n");

    int rand;
    for (int i = 0; i < 3; i++) {
        rand = random_int(20) + 1;
        printf("testing with value %d \n", rand);
        if (ret_n_test(rand)) {
            printf("PASSED\n");
        }
        else {
            printf("FAILED");
        }
    }
    printf("\n\n");

    printf("TESTING setjmp ACCROSS NESTED FUNCTION CALLS:     \n");

    if (test_mul_funcs()) {
        printf("PASSED\n");
    }
    else {
        printf("FAILED\n");
    }

    printf("\n\n");

    printf("TESTING FOR STACK CORRUPTION: \n");
    printf("Testing with 3 sets of random ints: \n");

    int rand1;
    int rand2;
    for (int i = 0; i < 3; i++) {
        rand1 = random_int(20) + 1;
        rand2 = random_int(20) + 1;
        printf("Testing with values %d, and %d", rand1, rand2);

        if (stack_test(rand1, rand2)) {
            printf("   Passed!\n");
        }
        else {
            printf("   Failed!   \n");
        }
    }

    return 0;
}
