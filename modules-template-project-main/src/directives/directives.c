/*
 * -----------------------------------------------------------------------------
 * directives.c
 *
 * Module: directives - Directive detection and execution
 * Responsible for: #include, #define, #ifdef/#endif processing
 *
 * Author: Carlos García 
 * -----------------------------------------------------------------------------
 */

#include "directives.h"

#include <string.h>
#include <ctype.h>

void directives_init(void) {
    fprintf(ofile, "[directives] OK\n");
}

int directives_process_line(const char *line) {
    if (!line) {
        return 1;
    }

    /* Skip leading whitespace */
    while (isspace((unsigned char)*line)) {
        line++;
    }

    /* Must start with '#' */
    if (*line != '#') {
        return 0;
    }

    line++; /* skip '#' */

    /* Dispatch directive */
    if (strncmp(line, "include", 7) == 0) {
        /* TODO: implement #include */
        fprintf(ofile, "[directives] include detected\n");
        return 0;
    }

    if (strncmp(line, "define", 6) == 0) {
        /* TODO: implement #define */
        fprintf(ofile, "[directives] define detected\n");
        return 0;
    }

    if (strncmp(line, "ifdef", 5) == 0) {
        /* TODO: implement #ifdef */
        fprintf(ofile, "[directives] ifdef detected\n");
        return 0;
    }

    if (strncmp(line, "endif", 5) == 0) {
        /* TODO: implement #endif */
        fprintf(ofile, "[directives] endif detected\n");
        return 0;
    }

    /* Unknown directive: ignore */
    return 0;
}