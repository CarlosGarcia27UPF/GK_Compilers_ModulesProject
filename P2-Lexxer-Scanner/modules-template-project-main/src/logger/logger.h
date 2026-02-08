/**
 * @file logger.h
 * @brief Logger Module (Debug Router) - STUB
 * 
 * ===============================================
 * LOGGER - DEBUG ROUTER
 * ===============================================
 * 
 * Centralized routing via fprintf.
 * Routes messages to stdout/file/dbgcnt based on config.
 * 
 * Called by: driver, automata, error, counter, out_writer
 * Calls into: stdout / files
 * 
 * TODO: This is a STUB - implement full functionality
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdbool.h>

/* Log levels */
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

/**
 * @brief Initialize logger
 * @param output Output stream (NULL for stdout)
 * @param debug_enabled Enable debug output
 */
void logger_init(FILE* output, bool debug_enabled);

/**
 * @brief Log a message
 * @param level Log level
 * @param format Printf format string
 * @param ... Format arguments
 */
void logger_log(LogLevel level, const char* format, ...);

/**
 * @brief Log debug message
 */
void logger_debug(const char* format, ...);

/**
 * @brief Log info message
 */
void logger_info(const char* format, ...);

/**
 * @brief Log warning message
 */
void logger_warn(const char* format, ...);

/**
 * @brief Log error message
 */
void logger_error(const char* format, ...);

/**
 * @brief Close logger
 */
void logger_close(void);

#endif /* LOGGER_H */
