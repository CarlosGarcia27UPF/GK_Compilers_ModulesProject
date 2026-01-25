/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module provides functionality to parse CLI flags and print help text.
 *
 * - `cli_parse`: Parses argv into structured preprocessing options.
 * - `cli_print_help`: Prints usage and available flags.
 *
 * Usage:
 *     Called from main to interpret command-line flags before preprocessing.
 *
 * Status:
 *     Active - supports required flags (-c, -d, -all, -help).
 * -------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "../spec/pp_spec.h"

// Compare a CLI argument against a known flag.
static int is_flag(const char *arg, const char *flag)
{
    // Check if the argument matches the flag exactly
    return (arg != NULL) && (strcmp(arg, flag) == 0);
}

// Parse CLI arguments into an options structure.
cli_options_t cli_parse(int argc, char **argv)
{
    // Initialize all options to disabled by default
    cli_options_t opt;
    opt.do_comments = 0;
    opt.do_directives = 0;
    opt.do_help = 0;

    // First pass: detect if user provided any flags at all.
    int has_any_flag = 0;
    for (int i = 1; i < argc; i++) {
        // Look for arguments starting with '-'
        if (argv[i] != NULL && argv[i][0] == PP_CHAR_DASH) {
            has_any_flag = 1;
            break;
        }
    }

    // Default behavior: if no flags => -c.
    // When the user runs the program without any flags, assume they want comment removal
    if (!has_any_flag) {
        opt.do_comments = 1;
    }

    // Parse flags - scan through all arguments to find which options are enabled.
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];

        if (is_flag(a, PP_FLAG_HELP)) {
            // -help flag: show usage information
            opt.do_help = 1;
        } else if (is_flag(a, PP_FLAG_ALL)) {
            // -all flag: enable both comment removal and directive processing
            opt.do_comments = 1;
            opt.do_directives = 1;
        } else if (is_flag(a, PP_FLAG_C)) {
            // -c flag: enable comment removal only
            opt.do_comments = 1;
        } else if (is_flag(a, PP_FLAG_D)) {
            // -d flag: enable directive processing (includes, defines, macros)
            opt.do_directives = 1;
        } else {
            // Not a recognized flag: likely the input filename.
            // We just skip it here - the main program will handle file arguments
        }
    }

    return opt;
}

// Print the CLI usage and options description.
void cli_print_help(const char *progname)
{
    // Use a default program name if none was provided
    if (!progname) progname = PP_DEFAULT_PROGNAME;

    // Brief description of the tool
    printf(PP_STR_DESCRIPTION);
    // Print the usage section header
    printf(PP_STR_USAGE_LABEL);
    // Print the command syntax: program [options] <file>
    printf(PP_FMT_USAGE_LINE, progname);
    // Print the options section header
    printf(PP_STR_OPTIONS_LABEL);
    // Print each available flag with its description
    printf(PP_FMT_OPTION_C, PP_FLAG_C);
    printf(PP_FMT_OPTION_D, PP_FLAG_D);
    printf(PP_FMT_OPTION_ALL, PP_FLAG_ALL, PP_FLAG_C, PP_FLAG_D);
    printf(PP_FMT_OPTION_HELP, PP_FLAG_HELP);

    // Show practical usage examples
    printf(PP_STR_EXAMPLES_LABEL);
    printf(PP_FMT_EXAMPLE_DEFAULT, progname);
    printf(PP_FMT_EXAMPLE_DIRECTIVES, progname);
    printf(PP_FMT_EXAMPLE_ALL, progname);

    // Explain where output goes
    printf(PP_STR_OUTPUT_LABEL);
}
