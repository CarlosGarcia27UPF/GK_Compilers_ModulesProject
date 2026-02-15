/*
 * -----------------------------------------------------------------------------
 * logger.h
 *
 * Message routing module for scanner diagnostics and status logs.
 * This module does not perform operation counting; counting is handled
 * exclusively by the counter module.
 * -----------------------------------------------------------------------------
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifndef DEBUG_FLAG
#define DEBUG_FLAG 0  /* 0 = stdout, 1 = output file */
#endif

#define DEBUG_ON  1
#define DEBUG_OFF 0


#ifndef OUTFORMAT
#define OUTFORMAT 0   /* 0 = release (minimal), 1 = debug (with line numbers) */
#endif

#define OUTFORMAT_RELEASE 0
#define OUTFORMAT_DEBUG   1

// Logger state structure.
typedef struct {
    FILE *dest;  // Message destination.
} logger_t;

// Initializes logger destination.
void logger_init(logger_t *lg, FILE *outfile);

// get the message destination FILE* (either stdout or the output file)
FILE* logger_get_dest(const logger_t *lg);

// write a message to the destination with printf format
void logger_write(const logger_t *lg, const char *fmt, ...);

// write tokens with proper formatting (line number in debug mode) 
void logger_write_tokens(logger_t *lg, int line_num, const char *token_str);

// Closes logger resources (currently a no-op).
void logger_close(logger_t *lg);

#endif /* LOGGER_H */
