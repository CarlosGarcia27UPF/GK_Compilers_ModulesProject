/*
 * -----------------------------------------------------------------------------
 * error.c
 *
 * Error catalog and reporter implementation.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "error.h"

// Maps an error ID to its message.
const char* err_get_message(int err_id) {
    switch (err_id) {
        case ERR_FILE_OPEN:        return ERR_MSG_FILE_OPEN;
        case ERR_FILE_OUTPUT:      return ERR_MSG_FILE_OUTPUT;
        case ERR_UNTERMINATED_LIT: return ERR_MSG_UNTERMINATED_LIT;
        case ERR_NONRECOGNIZED:    return ERR_MSG_NONRECOGNIZED;
        case ERR_INTERNAL:         return ERR_MSG_INTERNAL;
        default:                   return ERR_MSG_INTERNAL;
    }
}

// it prints a formatted error message to the given stream.
// then if destination or step is NULL, safe defaults are used.
void err_report(FILE *dest, int err_id, const char *step, int line,
                const char *context) {
    const char *msg = err_get_message(err_id);

    if (dest == NULL) {
        dest = stdout;
    }
    if (step == NULL) {
        step = ERR_STEP_SCANNER;
    }

    // Print context only when available
    if (context != NULL) {
        fprintf(dest, "[ERROR %d][%s] Line %d: %s: %s\n",
                err_id, step, line, msg, context);
    } else {
        fprintf(dest, "[ERROR %d][%s] Line %d: %s\n",
                err_id, step, line, msg);
    }
}
