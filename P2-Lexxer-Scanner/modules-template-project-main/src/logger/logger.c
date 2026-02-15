#include "logger.h"
#include <stdarg.h>

void logger_init(logger_t *lg, FILE *outfile) {
    if (lg == NULL) return;

    // Set destination based on debug flag and provided output file.
    if (DEBUG_FLAG == DEBUG_ON && outfile != NULL) {
        lg->dest = outfile;
    } else {
        lg->dest = stdout;
    }
}

// Gets the destination file for messages.
FILE* logger_get_dest(const logger_t *lg) {
    if (lg == NULL || lg->dest == NULL) {
        return stdout;
    }
    return lg->dest;
}

// write a formatted message to destination
void logger_write(const logger_t *lg, const char *fmt, ...) {
    va_list args;
    FILE *dest = logger_get_dest(lg);

    if (fmt == NULL) return;

    va_start(args, fmt);
    vfprintf(dest, fmt, args);
    va_end(args);
}

/*
 * write tokens with proper formatting
 * in debug mode: line number + tokens + blank line
 * in release mode: just tokens
 */
void logger_write_tokens(logger_t *lg, int line_num, const char *token_str) {
    FILE *dest = logger_get_dest(lg);
    if (dest == NULL || token_str == NULL) return;

#if OUTFORMAT == OUTFORMAT_DEBUG
    fprintf(dest, "%d %s\n", line_num, token_str);
    fprintf(dest, "\n"); 
#else
    fprintf(dest, "%s\n", token_str);
#endif
}

// Close logger resources (currently no-op).
void logger_close(logger_t *lg) {
    (void)lg;
}
