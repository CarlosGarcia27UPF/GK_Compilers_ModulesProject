/*
 * -----------------------------------------------------------------------------
 * test_module.h
 * 
 * This header file is used for testing purposes. It contains definitions and
 * function prototypes for the test modules.
 *
 * Usage:
 *     Include this header in test modules that require access to the test
 *     functionality.
 * Notes:
 *     This is part of a modular project structure, allowing each module to be  
 * 
 * developed and tested independently.
 * Author: [Your Name or Team]
 * -----------------------------------------------------------------------------
 */     
#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include <assert.h>

#include "../src/utils_files.h"

extern FILE *ofile; // file handler to send the module's output (to a file or stdout)

#endif // TEST_MODULE_H