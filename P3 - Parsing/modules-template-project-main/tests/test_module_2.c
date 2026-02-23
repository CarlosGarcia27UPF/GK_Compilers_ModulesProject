/*  
 * tests/test_module_2.c
 *
 * Functionality: [update with the functionality of the module]
 * This file is an autonomous test program (with its own main) for module 2.
 * It creates a separate executable to test independently the module.
 * It keeps the output in a separate log file or stdout, depending on the configuration.
 * All the logs are stored in the logs directory with a timestamped filename, and .log extension.
 * 
 * Use this as a template to create new test modules for each module in the project.
 * The overall project structure is modular, allowing each module to be developed 
 * and tested independently with minimal dependencies (minimal conflicts when committing).
 * 
 * The .h file contains the configurations and constants only needed for testing the specific module. 
 * So they are not seen by the main program, and there should be a separate header file 
 * for each module. There is the test_modules.h file that includes the common definitions
 * and function prototypes for all test modules.
 * 
 * Work should be distributed among team members, with each member responsible for one 
 * or more modules and the respective the test modules. Team members should not modify 
 * code outside their modules. If changes are required in other modules, they should be 
 * requested to the responsible member of that module.
 * 
 * The overall team should create the main module (main.c) that integrates all modules, 
 * at the beginning of the project. It should decide the organization of the data 
 * common in the program (data structure in main.h), and the core functions/modules 
 * to implement, building a project template with the main program calling the key 
 * functions and defining the empty modules to be worked out to develop the project. 
 * The modules have to have empty functions that just print a message when called, 
 * so that the program runs as an empty template to start working independently 
 * on each module.
 * 
 * Each module should be worked out in a different branch, and create a pull request to 
 * the main branch when the module is ready. The main branch should always be 
 * compilable and runnable.
 * 
 * Related files:
 * Files of the test module:
 * - tests/test_module_2.h: constains the configurations only needed for testing module (2).
 * - tests/test_module_2.c: constains the testing of the module (2).
 * Files of the module:
 * - src/module_2/module_2.h: contains the function prototypes of module 2 to test
 * - src/module_2/module_2.c: contains the implementation of module 2 
 * Files used by the module:
 * - tests/test_modules.h: contains test configurations common to all test modules
 * - src/main.h: cotains the data structure and configurations of the main program to be used by all modules
 * 
 * Team: [Your Team Name]
 * Author: [The responsible member/s]
*/
#include "../tests/test_module_2.h"

FILE *ofile = NULL; // file handler to send the module's output (to a file or stdout)

// Function to test the Fibonacci function from module 2
// It includes all edge cases and 
// one intentional mistake as example (comment it out to avoid the program aborting)
void test_fib() {
    int n = 0;
    static_assert(MOD2NEGFIBTEST == -5, "MOD2NEGFIBTEST should be -5"); // only for C11, and compile-time check

    fprintf(ofile, "%s\n", MOD2TESTMSG);
    fflush(ofile);

    // Fibonacci arguments cannot be negative: error case
    n = fib(MOD2TEST1NEG);
    assert(n == MOD2TEST1NEGRES);
    fprintf(ofile, "Fibonacci negative test passed!: fig(%d) = %d\n", MOD2TEST1NEG, MOD2TEST1NEGRES);
    fflush(ofile);

    // special case n = 0
    n = fib(MOD2TEST2ZERO);
    assert(n == MOD2TEST2ZERORES);
    fprintf(ofile, "Fibonacci special case n = 0 test Passed!: fig(%d) = %d\n", MOD2TEST2ZERO, MOD2TEST2ZERORES);
    fflush(ofile);

    // special case n = 1
    n = fib(MOD2TEST3ONE);
    assert(n == MOD2TEST3ONERES);
    fprintf(ofile, "Fibonacci special case n = 1 test Passed!: fig(%d) = %d\n", MOD2TEST3ONE, MOD2TEST3ONERES);
    fflush(ofile);

    // first successive case n = 2
    n = fib(MOD2TEST4SUCC1);
    assert(n == MOD2TEST4SUCC1RES);
    fprintf(ofile, "Fibonacci first successive case (n = 2) test Passed!: fig(%d) = %d\n", MOD2TEST4SUCC1, MOD2TEST4SUCC1RES);
    fflush(ofile);
    
    // successive case n
    n = fib(MOD2TEST5SUCCN);
    assert(n == MOD2TEST5SUCCNRES);
    fprintf(ofile, "Fibonacci successive n case test Passed!: fig(%d) = %d\n", MOD2TEST5SUCCN, MOD2TEST5SUCCNRES);
    //fflush(ofile);

    fprintf(ofile, "Fibonacci function tests passed!\n");
    fflush(ofile);
    // A failed test example (intentional mistake in the assert)
    // Uncomment to see how the program aborts at failure
    // n = fib(MOD2TESTINTENTIONAL);
    // assert(fib(n) == MOD2TESTINTENTIONALRES);
    fprintf(ofile, "Fibonacci Intentional mistake. If you see this message the test is commented out. Uncomment the assert to see mistake: fib(%d) = %d (and program will abort)\n", MOD2TESTINTENTIONAL, fib(MOD2TESTINTENTIONAL));
    fflush(ofile);
}

// Main function to run the tests of Module 2
int main(void) {
    ofile = stdout; // Default output to stdout
    ofile = set_output_test_file(MOD2TESTLOGFILENAME);
    printf("Running tests for Module 2: Fibonacci function\n");
    test_fib();
    printf("Finished tests for Module 2: Fibonacci function\n");
    fprintf(ofile, "Finished tests for Module 2: Fibonacci function\n\n");
    fclose(ofile); 
    return 0;
}
    
