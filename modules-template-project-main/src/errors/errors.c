/*
 * -----------------------------------------------------------------------------
 * errors.c
 *
 * Module: errors - Error reporting
 * Responsible for: error(line, msg) style function and line-number support
 *
 * Author: [Team Member 6]
 * -----------------------------------------------------------------------------
 */

#include "errors.h"

void errors_init(void) {
    fprintf(ofile, "[errors] OK\n");
}
