/*
 * -----------------------------------------------------------------------------
 * directives.h
 *
 * Module: directives - Directive detection and execution
 * Responsible for: #include, #define, #ifdef/#endif processing
 *
 * Author: [Team Member 4]
 * -----------------------------------------------------------------------------
 */

#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#include <stdio.h>

extern FILE* ofile;

void directives_init(void);

#endif // DIRECTIVES_H
