/**
 * @file driver.h
 * @brief Driver Module (Main / Orchestrator)
 * 
 * ===============================================
 * DRIVER - MAIN / ORCHESTRATOR
 * ===============================================
 * 
 * Wrapper only. No scanning, no formatting.
 * Coordinates all other modules.
 * 
 * Called by: OS / user execution
 * Calls into: char_stream, logger, counter, token_list, automata, out_writer, error
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <stdio.h>
#include <stdbool.h>

/* Compile-time flags */
#ifndef DEBUG_MODE
#define DEBUG_MODE 0        /* 0=Release, 1=Debug */
#endif

#ifndef COUNTCONFIG
#define COUNTCONFIG 0       /* 0=Disabled, 1=Enabled */
#endif

/**
 * @brief Run the scanner on input file
 * @param input_filename Input source file
 * @param output_filename Output token file (NULL for stdout)
 * @return Exit code (0=success)
 */
int driver_run_file(const char* input_filename, const char* output_filename);

/**
 * @brief Run the scanner on input string
 * @param source Source code string
 * @param output Output file (NULL for stdout)
 * @return Exit code (0=success)
 */
int driver_run_string(const char* source, FILE* output);

/**
 * @brief Print usage information
 * @param program_name Name of the executable
 */
void driver_print_usage(const char* program_name);

#endif /* DRIVER_H */
