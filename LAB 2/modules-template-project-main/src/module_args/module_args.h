/*
 * -----------------------------------------------------------------------------
 * module2.h 
 *
 * Header file for module2, which provides functions to handle command-line
 * arguments in a modular and testable way.
 *
 * Functions:
 * - `print_arguments`: Logs each argument received by the program.
 * - `process_arguments`: Intended to process arguments for the application.
 *
 * Usage:
 *     Include this header in main modules or test modules that require access
 *     to command-line processing functionality.
 *
 * Notes:
 *     This is part of a modular project structure, allowing each module to be
 *     developed and tested independently.
 *
 * Team: [Your Team Name]
 * Contributor/s: [Your Name]
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE2_H
#define MODULE2_H

#include "../main.h"

int process_arguments(int argc, char *argv[]);
void print_arguments(int argc, char *argv[]);

#endif

