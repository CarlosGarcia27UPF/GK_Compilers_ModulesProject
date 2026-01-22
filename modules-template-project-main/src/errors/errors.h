/*
 * -----------------------------------------------------------------------------
 * errors.h
 *
 * Module: errors - Error reporting
 * Responsible for: error(line, msg) style function and line-number support
 *
 * -----------------------------------------------------------------------------
 */

#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdarg.h>

/* Report an error at a specific line */
void error_report(const char *filename, int line, const char *fmt, ...);

/* Get total error count */
int error_get_count(void);

/* Reset error count */
void error_reset_count(void);

#endif // ERRORS_H
