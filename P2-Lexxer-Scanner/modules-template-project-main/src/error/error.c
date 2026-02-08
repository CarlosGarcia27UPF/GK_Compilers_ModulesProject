/**
 * @file error.c
 * @brief Error Module Implementation - STUB
 * 
 * TODO: This is a STUB - implement full functionality
 */

#include "error.h"
#include <stdio.h>

/* Error count */
static int g_error_count = 0;

/* Error messages catalog */
static const char* ERROR_MESSAGES[] = {
    "No error",                             /* ERR_NONE */
    "Unterminated string literal",          /* ERR_UNTERMINATED_LITERAL */
    "Non-recognized character(s)",          /* ERR_NON_RECOGNIZED_CHAR */
    "File not found",                       /* ERR_FILE_NOT_FOUND */
    "Memory allocation failed",             /* ERR_MEMORY_ALLOCATION */
    "Invalid argument"                      /* ERR_INVALID_ARGUMENT */
};

void error_init(void) {
    g_error_count = 0;
    printf("[ERROR STUB] Error module initialized\n");
}

void error_report(ErrorID error_id, int line, const char* context) {
    g_error_count++;
    
    const char* msg = error_get_message(error_id);
    
    printf("[ERROR STUB] Error at line %d: %s", line, msg);
    if (context != NULL && context[0] != '\0') {
        printf(" '%s'", context);
    }
    printf("\n");
}

const char* error_get_message(ErrorID error_id) {
    if (error_id < 0 || error_id >= ERR_COUNT) {
        return "Unknown error";
    }
    return ERROR_MESSAGES[error_id];
}

int error_get_count(void) {
    return g_error_count;
}

bool error_has_errors(void) {
    return g_error_count > 0;
}

void error_reset(void) {
    g_error_count = 0;
}

void error_close(void) {
    printf("[ERROR STUB] Error module closed (total errors: %d)\n", g_error_count);
}
