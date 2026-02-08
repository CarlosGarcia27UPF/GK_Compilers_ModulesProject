/**
 * @file error.h
 * @brief Error Module (Catalog + Reporter) - STUB
 * 
 * ===============================================
 * ERROR - CATALOG + REPORTER
 * ===============================================
 * 
 * Centralized error handling.
 * 
 * Called by: automata, driver
 * Calls into: logger
 * 
 * TODO: This is a STUB - implement full functionality
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

/* Error IDs */
typedef enum {
    ERR_NONE = 0,
    ERR_UNTERMINATED_LITERAL,
    ERR_NON_RECOGNIZED_CHAR,
    ERR_FILE_NOT_FOUND,
    ERR_MEMORY_ALLOCATION,
    ERR_INVALID_ARGUMENT,
    /* Add more error codes as needed */
    ERR_COUNT
} ErrorID;

/**
 * @brief Initialize error module
 */
void error_init(void);

/**
 * @brief Report an error
 * @param error_id Error ID from catalog
 * @param line Line number where error occurred
 * @param context Additional context (lexeme, etc.)
 */
void error_report(ErrorID error_id, int line, const char* context);

/**
 * @brief Get error message string
 * @param error_id Error ID
 * @return Error message
 */
const char* error_get_message(ErrorID error_id);

/**
 * @brief Get total error count
 * @return Number of errors reported
 */
int error_get_count(void);

/**
 * @brief Check if any errors occurred
 * @return true if errors occurred
 */
bool error_has_errors(void);

/**
 * @brief Reset error count
 */
void error_reset(void);

/**
 * @brief Close error module
 */
void error_close(void);

#endif /* ERROR_H */
