/*
 * -----------------------------------------------------------------------------
 * errors.h
 *
 * Module: errors - Error reporting
 * Responsible for: error(line, msg) style function and line-number support
 *
 * Author: [Team Member 6]
 * -----------------------------------------------------------------------------
 */

#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>

extern FILE* ofile;

void errors_init(void);

#endif // ERRORS_H
