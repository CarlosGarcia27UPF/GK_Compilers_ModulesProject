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

// Max nesting depth for conditional compilation (#ifdef).
// This prevents infinite recursion and limits how deep #ifdef blocks can nest
#define PP_MAX_IF_DEPTH 64
// Maximum path length used when composing include paths.
// This should be large enough to handle deeply nested directory structures
#define PP_MAX_PATH_LEN 4096
// Maximum length of a quoted include filename.
// e.g., #include "myfile.h" where "myfile.h" must fit within this limit
#define PP_MAX_INCLUDE_NAME 256
// Maximum length of a macro name in a #define directive.
// e.g., #define MACRO_NAME where MACRO_NAME must fit within this limit
#define PP_MAX_DEFINE_NAME 128
// Maximum length of a macro value in a #define directive.
// e.g., #define NAME value where value must fit within this limit
#define PP_MAX_DEFINE_VALUE 512
// Chunk size used when reading files into buffers.
// Files are read in 4KB chunks for efficiency
#define PP_IO_READ_CHUNK 4096

// CLI flag for comment removal mode.
// When this flag is used, the preprocessor removes C-style and C++ comments
#define PP_FLAG_C "-c"
// CLI flag for directive + macro processing mode.
// When this flag is used, the preprocessor processes #include, #define, #ifdef, and expands macros
#define PP_FLAG_D "-d"
// CLI flag that enables all supported processing.
// Equivalent to using both -c and -d together
#define PP_FLAG_ALL "-all"
// CLI flag that prints the help page and exits.
// Shows usage information and available options
#define PP_FLAG_HELP "-help"

// Default program name used when argv[0] is not available.
// Fallback name for the executable if we can't determine it from command line
#define PP_DEFAULT_PROGNAME "pp"
// Short description of what the tool does.
// Printed ahead of usage to give quick context
#define PP_STR_DESCRIPTION "C preprocessor practice tool that removes comments, handles directives, and expands macros.\n"
// "Usage:" label line for help output.
#define PP_STR_USAGE_LABEL "Usage:\n"
// "Options:" label line for help output (includes a blank line).
#define PP_STR_OPTIONS_LABEL "\nOptions:\n"
// Format line for the usage synopsis.
// Shows how to invoke the program with options and input file
#define PP_FMT_USAGE_LINE "  %s [options] <file.c|file.h>\n"
// Format line for the -c option description.
#define PP_FMT_OPTION_C "  %s     Remove comments (default if no flags)\n"
// Format line for the -d option description.
#define PP_FMT_OPTION_D "  %s     Process directives (#include, #define, #ifdef/#endif) + macro expansion\n"
// Format line for the -all option description.
#define PP_FMT_OPTION_ALL "  %s   Equivalent to %s %s\n"
// Format line for the -help option description.
#define PP_FMT_OPTION_HELP "  %s  Show this help\n"
// Label for the examples section.
#define PP_STR_EXAMPLES_LABEL "\nExamples:\n"
// Example: default behavior (comments only).
#define PP_FMT_EXAMPLE_DEFAULT "  %s input.c              Remove comments (default)\n"
// Example: directives and macros.
#define PP_FMT_EXAMPLE_DIRECTIVES "  %s -d input.h            Process directives and macros\n"
// Example: full preprocessing.
#define PP_FMT_EXAMPLE_ALL "  %s -all input.c          Full preprocessing (comments + directives + macros)\n"
// Label for output behavior section.
#define PP_STR_OUTPUT_LABEL "\nOutput:\n  Writes processed content to stdout. Redirect to a file if needed.\n"


// Error message when comment processing fails.
// Displayed when the comment removal module encounters an error
#define PP_ERR_COMMENTS_PROCESS "Failed to process comments"
// Error message when an allocation or buffer append fails.
// Displayed when we run out of memory during preprocessing
#define PP_ERR_OUT_OF_MEMORY "Out of memory"
// Error message when macro expansion fails.
// Displayed when the macro expansion module encounters an error
#define PP_ERR_MACRO_EXPANSION "Macro expansion failed"

// Preprocessor directive marker character ('#').
// All preprocessor directives start with this character
#define PP_CHAR_HASH '#'
// CLI flag prefix character ('-').
// All command-line flags start with this character
#define PP_CHAR_DASH '-'
// Newline character used to detect line boundaries.
// The preprocessor processes input line-by-line using this delimiter
#define PP_CHAR_NL '\n'

// pp_run success return code.
// Returned when preprocessing completes without errors
#define PP_RUN_SUCCESS 0
// pp_run error for invalid inputs or null parameters.
// Returned when required parameters are missing or null
#define PP_RUN_ERR_INVALID_ARGS 1
// pp_run error for processing failures on regular lines.
// Returned when an error occurs while processing a line in the middle of the file
#define PP_RUN_ERR_PROCESSING 2
// pp_run error for processing failures on the last line.
// Returned when an error occurs while processing the final line of the file
#define PP_RUN_ERR_PROCESSING_LAST_LINE 3

#endif
