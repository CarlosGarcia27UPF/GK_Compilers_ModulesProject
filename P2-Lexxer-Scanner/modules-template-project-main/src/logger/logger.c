/**
 * @file logger.c
 * @brief Logger Module Implementation - STUB
 * 
 * TODO: This is a STUB - implement full functionality
 */

#include "logger.h"
#include <stdarg.h>

/* Global logger state */
static FILE* g_log_output = NULL;
static bool g_debug_enabled = false;

void logger_init(FILE* output, bool debug_enabled) {
    g_log_output = (output != NULL) ? output : stdout;
    g_debug_enabled = debug_enabled;
    
    printf("[LOGGER STUB] Logger initialized (debug=%s)\n", 
           debug_enabled ? "ON" : "OFF");
}

void logger_log(LogLevel level, const char* format, ...) {
    if (g_log_output == NULL) g_log_output = stdout;
    
    /* Skip debug if not enabled */
    if (level == LOG_DEBUG && !g_debug_enabled) return;
    
    const char* prefix;
    switch (level) {
        case LOG_DEBUG:   prefix = "[DEBUG]"; break;
        case LOG_INFO:    prefix = "[INFO]"; break;
        case LOG_WARNING: prefix = "[WARN]"; break;
        case LOG_ERROR:   prefix = "[ERROR]"; break;
        default:          prefix = "[???]"; break;
    }
    
    fprintf(g_log_output, "%s ", prefix);
    
    va_list args;
    va_start(args, format);
    vfprintf(g_log_output, format, args);
    va_end(args);
    
    fprintf(g_log_output, "\n");
}

void logger_debug(const char* format, ...) {
    if (!g_debug_enabled) return;
    if (g_log_output == NULL) g_log_output = stdout;
    
    fprintf(g_log_output, "[DEBUG] ");
    va_list args;
    va_start(args, format);
    vfprintf(g_log_output, format, args);
    va_end(args);
    fprintf(g_log_output, "\n");
}

void logger_info(const char* format, ...) {
    if (g_log_output == NULL) g_log_output = stdout;
    
    fprintf(g_log_output, "[INFO] ");
    va_list args;
    va_start(args, format);
    vfprintf(g_log_output, format, args);
    va_end(args);
    fprintf(g_log_output, "\n");
}

void logger_warn(const char* format, ...) {
    if (g_log_output == NULL) g_log_output = stdout;
    
    fprintf(g_log_output, "[WARN] ");
    va_list args;
    va_start(args, format);
    vfprintf(g_log_output, format, args);
    va_end(args);
    fprintf(g_log_output, "\n");
}

void logger_error(const char* format, ...) {
    if (g_log_output == NULL) g_log_output = stdout;
    
    fprintf(g_log_output, "[ERROR] ");
    va_list args;
    va_start(args, format);
    vfprintf(g_log_output, format, args);
    va_end(args);
    fprintf(g_log_output, "\n");
}

void logger_close(void) {
    printf("[LOGGER STUB] Logger closed\n");
    g_log_output = NULL;
}
