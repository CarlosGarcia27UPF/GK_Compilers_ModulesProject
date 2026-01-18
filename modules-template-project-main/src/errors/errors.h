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

extern FILE* ofile;

void errors_init(void);
void error(int line, const char *fmt, ...);
int get_error_count(void);

#endif // ERRORS_H
