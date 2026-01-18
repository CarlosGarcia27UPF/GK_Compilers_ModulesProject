/*
 * -----------------------------------------------------------------------------
 * directives.h
 *
 * Module: directives - Directive detection and execution
 * Responsible for: #include, #define, #ifdef/#endif processing
 *
 * Author: Carlos García
 * -----------------------------------------------------------------------------
 */

#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#include <stdio.h>

extern FILE* ofile;

void directives_init(void);
int directives_process_line(const char *line);

#endif // DIRECTIVES_H
