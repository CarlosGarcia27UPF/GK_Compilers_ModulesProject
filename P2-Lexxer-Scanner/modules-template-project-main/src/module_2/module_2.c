/*
 * -----------------------------------------------------------------------------
 * module_2.c
 *
 * Template recursive Fibonacci implementation used by module tests.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "./module_2.h"

// Returns Fibonacci(n). Returns -1 for negative input.
int fib(int n) {
    int result;

    if (n < 0) {
        return -1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }

    result = fib(n - 1) + fib(n - 2);
    return result;
}
