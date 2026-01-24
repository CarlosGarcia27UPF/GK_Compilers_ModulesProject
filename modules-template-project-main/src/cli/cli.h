/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module defines the CLI options structure and parsing interface.
 *
 * - `cli_parse`: Parses argv into structured preprocessing options.
 * - `cli_print_help`: Prints usage and available flags.
 *
 * Usage:
 *     Include this header in main or test modules that need CLI parsing.
 *
 * Status:
 *     Active - interface used by the main entry point.
 * -------------------------------------------------------------------------- */

#ifndef CLI_H
#define CLI_H

/* Parsed CLI options controlling preprocessing stages. */
typedef struct {
    /* Enable comment removal (-c). */
    int do_comments;
    /* Enable directive processing and macro expansion (-d). */
    int do_directives;
    /* Print help and exit (-help). */
    int do_help;
} cli_options_t;

/* Parse argv into structured CLI options. */
cli_options_t cli_parse(int argc, char **argv);
/* Print the user-facing help text (man page). */
void cli_print_help(const char *progname);

#endif
