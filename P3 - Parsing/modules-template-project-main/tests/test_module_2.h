#ifndef TEST_MODULE_2_H
#define TEST_MODULE_2_H
/* 
 * -----------------------------------------------------------------------------
 * test_module_2.h
 *
 * This header file is only used for testing. 
 * It contains all definitions just for the assert and conditions to test. 
 * It is not seen by the main program and there should be a separate header file for each module.
 * 
 * A module can test more than one function. So name the constants accordingly,
 * and decide what functionality makes a new function inside the same module, 
 * or should be a new separate module.
 * -----------------------------------------------------------------------------
 */

#include "../tests/test_modules.h"
#include "../src/module_2/module_2.h"

// Module 2 test message
#define MOD2TESTMSG "Testing Module 2: Fibonacci Function"

// Output file with test results: It can go to display (stdout) or to a file (one should be commented out)
#define MOD2TESTLOGFILENAME "./test_module_2.log"
// #define MOD2TESTLOGFILENAME "stdout"

// Fibonacci conditions to test: case tests
#define MOD2TEST1NEG  -3     // Negative Fibonacci test case
#define MOD2TEST2ZERO  0     // Fibonacci test case for special case n=0
#define MOD2TEST3ONE   1     // Fibonacci test case for special case n=1
#define MOD2TEST4SUCC1 2     // Fibonacci test case for first successive case n=2
#define MOD2TEST5SUCCN 6     // Fibonacci test case for successive case n=6
#define MOD2TESTS      5     // Total number of tests in this section/function

// Fibonacci test results: results of case tests
#define MOD2TEST1NEGRES    -1     // Result: Negative Fibonacci test case
#define MOD2TEST2ZERORES   0     // Result: Fibonacci test case for special case n=0
#define MOD2TEST3ONERES    1     // Result: Fibonacci test case for special case n=1
#define MOD2TEST4SUCC1RES  1     // Result: Fibonacci test case for first successive case n=2
#define MOD2TEST5SUCCNRES  8     // Result: Fibonacci test case for successive case n

// Intentional mistake test: it aborts the program
#define MOD2TESTINTENTIONAL 5 // This is an intentional mistake to show a failed test case
#define MOD2TESTINTENTIONALRES 3 // fib(5)=5, but we expect 3 to show the failure

// compile-time checks 
#define MOD2NEGFIBTEST -5 // Negative compile-time test case, only for C11, compile-time check

extern FILE *ofile; // file handler to send the module's output (to a file or stdout)

// Function prototypes
void test_fib();

#endif