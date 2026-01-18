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

void errors_init(void) {
    if (ofile != NULL) {
        fprintf(ofile, "[errors] OK\n");
    }
    error_count = 0;
}

void error(int line, const char *fmt, ...) {
    error_count++;
    
    // Print to the file log if available
    if (ofile != NULL) {
        fprintf(ofile, "Error on line %d: ", line);
        
        va_list args;
        va_start(args, fmt);
        vfprintf(ofile, fmt, args);
        va_end(args);
        
        fprintf(ofile, "\n");
        fflush(ofile);
    }

    // Also print to stderr for immediate user feedback
    fprintf(stderr, "Error on line %d: ", line);
    
    va_list args_stderr;
    va_start(args_stderr, fmt);
    vfprintf(stderr, fmt, args_stderr);
    va_end(args_stderr);
    
    fprintf(stderr, "\n");
}

int get_error_count(void) {
    return error_count;
}
