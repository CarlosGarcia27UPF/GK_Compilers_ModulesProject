/*
 * -----------------------------------------------------------------------------
 * logger.c
 *
 * Logger implementation. Routes messages through fprintf to either
 * stdout or the output file based on the DEBUG compile-time setting.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "logger.h"
#include <stdarg.h>

// Chooses logger destination based on DEBUG.
void logger_init(logger_t *lg, FILE *outfile) {
    if (lg == NULL) {
        return;
    }
    if (DEBUG == DEBUG_ON && outfile != NULL) {
        lg->dest = outfile;
    } else {
        lg->dest = stdout;
    }
}

// Returns current destination stream.
FILE* logger_get_dest(const logger_t *lg) {
    if (lg == NULL || lg->dest == NULL) {
        return stdout;
    }
    return lg->dest;
}

// Writes one formatted message.
void logger_write(const logger_t *lg, const char *fmt, ...) {
    va_list args;
    FILE *dest = logger_get_dest(lg);

    if (fmt == NULL) {
        return;
    }

    va_start(args, fmt);
    vfprintf(dest, fmt, args);
    va_end(args);
}
