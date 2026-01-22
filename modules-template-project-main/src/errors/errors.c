/*
 * -----------------------------------------------------------------------------
 * errors.c
 *
 * Module: errors - Error reporting
 * Responsible for: error(line, msg) style function and line-number support
 *
 * -----------------------------------------------------------------------------
 */

#include "errors.h"

static int error_count = 0;

void error_report(const char *filename, int line, const char *fmt, ...) {
    error_count++;
    
    /* Print to stderr for immediate user feedback */
    if (filename) {
        fprintf(stderr, "Error in %s:%d: ", filename, line);
    } else {
        fprintf(stderr, "Error on line %d: ", line);
    }
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}

int error_get_count(void) {
    return error_count;
}

void error_reset_count(void) {
    error_count = 0;
}
