/*
 * =============================================================================
 * test_module_args.h
 * Test configuration for the module_args module.
 *
 * Declares the log filename constant and includes the necessary headers
 * for testing the command-line argument processor.
 *
 * Author: [Team]
 * Date:   2026
 * =============================================================================
 */
#ifndef TEST_MODULE_ARGS_H
#define TEST_MODULE_ARGS_H
 
#include "../tests/test_modules.h"
#include "../src/module_args/module_args.h"


// Output file with test results separate from the main program, and each module
// It can go to display (stdout) or to a file (one should be commented out)
#define MODARGSTESTLOGFILENAME "./test_module_args.log"
// #define MODARGSTESTLOGFILENAME "stdout"

#endif // TEST_MODULE_ARGS_H