/*
 * -----------------------------------------------------------------------------
 * module_args.c
 *
 * Template command-line argument utilities used by module tests.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "./module_args.h"

// Prints each command-line argument with its index.
void print_arguments(int argc, char *argv[]) {
    int i;

    fprintf(ofile, "Arguments received (%d):\n", argc);
    for (i = 0; i < argc; i++) {
        fprintf(ofile, "Argument %d: %s\n", i, argv[i]);
    }
    fflush(ofile);
}

// Template entry point for argument handling.
int process_arguments(int argc, char *argv[]) {
    print_arguments(argc, argv);
    fprintf(ofile, "Module arguments: not implemented yet\n");
    fflush(ofile);
    return 0;
}
