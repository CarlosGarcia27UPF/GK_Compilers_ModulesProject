/*
 * -----------------------------------------------------------------------------
 * module_args.c
 *
 * Parses command-line arguments for the p3_parser program and stores them
 * in a config_t struct.
 *
 * Usage: p3_parser <input.cscn> [language_spec.txt]
 *   argv[1] : path to the .cscn scanner-output file  (required)
 *   argv[2] : path to the grammar spec file           (optional,
 *             defaults to DEFAULT_LANG_FILE)
 *
 * Author: [Team]
 * -----------------------------------------------------------------------------
 */

#include <string.h>
#include <stdio.h>
#include "module_args.h"

/* Print all raw command-line arguments to ofile. */
void print_arguments(int argc, char *argv[])
{
    fprintf(ofile, "Arguments received (%d):\n", argc);
    for (int i = 0; i < argc; i++) {
        fprintf(ofile, "  argv[%d]: %s\n", i, argv[i]);
    }
    fflush(ofile);
}

/* Parse argv into *config.
 * Returns 0 on success, -1 if the required input file is missing. */
int process_arguments(int argc, char *argv[], config_t *config)
{
    print_arguments(argc, argv);

    if (argc < ARGS_MIN_COUNT) {
        fprintf(stderr, "Usage: %s <input.cscn> [language_spec.txt]\n",
                argv[0]);
        return -1;
    }

    strncpy(config->input_file, argv[ARGS_INPUT_IDX], MAXFILENAME - 1);
    config->input_file[MAXFILENAME - 1] = '\0';

    if (argc >= ARGS_WITH_LANG) {
        strncpy(config->lang_file, argv[ARGS_LANG_IDX], MAXFILENAME - 1);
    } else {
        strncpy(config->lang_file, DEFAULT_LANG_FILE, MAXFILENAME - 1);
    }
    config->lang_file[MAXFILENAME - 1] = '\0';

    fprintf(ofile, "Input file : %s\n", config->input_file);
    fprintf(ofile, "Lang  file : %s\n", config->lang_file);
    fflush(ofile);
    return 0;
}
