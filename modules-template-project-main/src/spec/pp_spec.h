/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module provides centralized constants for preprocessing behavior.
 *
 * - `PP_MAX_IF_DEPTH`, `PP_MAX_PATH_LEN`: Limits for nesting and paths.
 * - `PP_FLAG_*`: Command-line flags.
 * - `PP_STR_*`, `PP_FMT_*`: Help/usage strings and formats.
 * - `PP_ERR_*`: Standardized error messages.
 * - `PP_CHAR_*`: Core character constants.
 * - `PP_RUN_*`: Result codes for `pp_run`.
 *
 * Usage:
 *     Include this header wherever shared constants are required.
 *
 * Status:
 *     Active - shared specification values for the whole project.
 * -------------------------------------------------------------------------- */

#ifndef PP_SPEC_H
#define PP_SPEC_H

/* Max nesting depth for conditional compilation (#ifdef). */
#define PP_MAX_IF_DEPTH 64
/* Maximum path length used when composing include paths. */
#define PP_MAX_PATH_LEN 512

/* CLI flag for comment removal mode. */
#define PP_FLAG_C "-c"
/* CLI flag for directive + macro processing mode. */
#define PP_FLAG_D "-d"
/* CLI flag that enables all supported processing. */
#define PP_FLAG_ALL "-all"
/* CLI flag that prints the help page and exits. */
#define PP_FLAG_HELP "-help"

/* Default program name used when argv[0] is not available. */
#define PP_DEFAULT_PROGNAME "pp"
/* "Usage:" label line for help output. */
#define PP_STR_USAGE_LABEL "Usage:\n"
/* "Options:" label line for help output (includes a blank line). */
#define PP_STR_OPTIONS_LABEL "\nOptions:\n"
/* Format line for the usage synopsis. */
#define PP_FMT_USAGE_LINE "  %s [options] <file.c|file.h>\n"
/* Format line for the -c option description. */
#define PP_FMT_OPTION_C "  %s     Remove comments (default if no flags)\n"
/* Format line for the -d option description. */
#define PP_FMT_OPTION_D "  %s     Process directives (#include, #define, #ifdef/#endif) + macro expansion\n"
/* Format line for the -all option description. */
#define PP_FMT_OPTION_ALL "  %s   Equivalent to %s %s\n"
/* Format line for the -help option description. */
#define PP_FMT_OPTION_HELP "  %s  Show this help\n"

/* Error message when comment processing fails. */
#define PP_ERR_COMMENTS_PROCESS "Failed to process comments"
/* Error message when an allocation or buffer append fails. */
#define PP_ERR_OUT_OF_MEMORY "Out of memory"
/* Error message when macro expansion fails. */
#define PP_ERR_MACRO_EXPANSION "Macro expansion failed"

/* Preprocessor directive marker character ('#'). */
#define PP_CHAR_HASH '#'
/* CLI flag prefix character ('-'). */
#define PP_CHAR_DASH '-'
/* Newline character used to detect line boundaries. */
#define PP_CHAR_NL '\n'

/* pp_run success return code. */
#define PP_RUN_SUCCESS 0
/* pp_run error for invalid inputs or null parameters. */
#define PP_RUN_ERR_INVALID_ARGS 1
/* pp_run error for processing failures on regular lines. */
#define PP_RUN_ERR_PROCESSING 2
/* pp_run error for processing failures on the last line. */
#define PP_RUN_ERR_PROCESSING_LAST_LINE 3

#endif
