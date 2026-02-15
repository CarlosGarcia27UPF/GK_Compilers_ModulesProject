/*
 * -----------------------------------------------------------------------------
 * logger.h
 *
 * Centralized debug/message router. All output from the scanner goes
 * through this module so that the DEBUG flag can route messages to
 * stdout or the output file.
 *
 * DEBUG ON  (1) = messages written to the output file
 * DEBUG OFF (0) = messages written to stdout
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// Debug ON / OFF values.
#define DEBUG_ON  1
#define DEBUG_OFF 0

// DEBUG configuration (compile-time).
#ifndef DEBUG
#define DEBUG DEBUG_OFF  // Default OFF: messages go to stdout.
#endif

// Logger destination state.
typedef struct {
    FILE *dest;  // Current output destination (stdout or file).
} logger_t;

// Initializes logger destination.
void logger_init(logger_t *lg, FILE *outfile);

// Returns current destination stream.
FILE* logger_get_dest(const logger_t *lg);

// Writes a formatted message.
void logger_write(const logger_t *lg, const char *fmt, ...);

#endif /* LOGGER_H */
