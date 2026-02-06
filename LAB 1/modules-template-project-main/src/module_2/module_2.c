#include "./module_2.h"

int fib(int n) {
    int r; 

    if (n < 0) return -1;
    if (n == 0) return 0; // Set to 1 if you want a fail assert in the workflow tests
    if (n == 1) return 1;
    r = fib(n - 1) + fib(n - 2);
    // fprintf(ofile, "Module_2: Fib(%d) = %d\n", n, r);
    return r;
}
