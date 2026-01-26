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
#include <string.h>

static int error_count = 0;
static buffer_t *error_buffer = NULL;

void errors_init(void) {
    error_count = 0;
    error_buffer = NULL;
}

void errors_set_buffer(buffer_t *buffer) {
    error_buffer = buffer;
}

void error(int line, const char *fmt, ...) {
    error_count++;
    
    // Print to the output buffer if available
    if (error_buffer != NULL) {
        char buf[1024];
        
        // Format the prefix
        int prefix_len = snprintf(buf, sizeof(buf), "Error on line %d: ", line);
        if (prefix_len > 0) {
            buffer_append_str(error_buffer, buf);
        }

        // Format the message
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        
        buffer_append_str(error_buffer, buf);
        buffer_append_char(error_buffer, '\n');
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

void reset_count(int count) {
    error_count = 0;
}