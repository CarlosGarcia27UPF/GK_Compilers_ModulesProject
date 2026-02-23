/*
 * -----------------------------------------------------------------------------
 * module_args.h
 *
 * Header file for the module_args module.
 * Declares config_t (CLI arguments) and the functions that populate it.
 *
 * Functions:
 * - process_arguments : Parse argc/argv into a config_t.
 * - print_arguments   : Log raw arguments to ofile.
 *
 * Usage:
 *     Include in main.h or any module that needs access to CLI configuration.
 *
 * Team: [Your Team Name]
 * -----------------------------------------------------------------------------
 */

#ifndef MODULE_ARGS_H
#define MODULE_ARGS_H

#include <stdio.h>
#include "../utils_files.h"

/* Default language specification file */
#define DEFAULT_LANG_FILE "./data/language1.txt"  /* Path used when no lang file is specified on CLI */

/* Argument index constants for the p3_parser CLI */
#define ARGS_MIN_COUNT    2  /* Minimum argc: program name + input file (required) */
#define ARGS_WITH_LANG    3  /* argc when the optional language file is also given  */
#define ARGS_INPUT_IDX    1  /* argv index for the .cscn input file                 */
#define ARGS_LANG_IDX     2  /* argv index for the optional language spec file      */

/* CLI configuration populated by process_arguments */
typedef struct {
    char input_file[MAXFILENAME]; /* Path to the .cscn scanner-output file */
    char lang_file[MAXFILENAME];  /* Path to the grammar/language spec file */
} config_t;

extern FILE *ofile; /* Shared output log defined in main.c */

/* Parse argc/argv and fill *config. Returns 0 on success, -1 on error. */
int  process_arguments(int argc, char *argv[], config_t *config);

/* Print all raw arguments to ofile for debugging. */
void print_arguments(int argc, char *argv[]);

#endif /* MODULE_ARGS_H */

