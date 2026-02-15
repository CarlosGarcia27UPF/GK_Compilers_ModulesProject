#include "logger.h"
#include <stdarg.h>
#include <string.h>


void logger_init(logger_t *lg, FILE *outfile, const char *input_filename) {
    if (lg == NULL) return;

    // set main destination based on debug flag and provided output file
    if (DEBUG_FLAG == DEBUG_ON && outfile != NULL) {
        lg->dest = outfile;
    } else {
        lg->dest = stdout;
    }

    // set up counter destination based on COUNTOUT configuration
#ifdef COUNTCONFIG
    if (COUNTOUT == COUNTOUT_MAIN) {
        lg->count_dest = lg->dest;
        lg->count_file[0] = '\0';
    } else {
        if (input_filename != NULL) {
            snprintf(lg->count_file, sizeof(lg->count_file),
                     "%s.dbgcnt", input_filename);
            lg->count_dest = fopen(lg->count_file, "w");
            if (lg->count_dest == NULL) {
                lg->count_dest = stdout;  
                lg->count_file[0] = '\0';
            }
        } else {
            lg->count_dest = stdout;
            lg->count_file[0] = '\0';
        }
    }
#else
    lg->count_dest = NULL;
    lg->count_file[0] = '\0';
#endif
}


//get the destination file for messages

FILE* logger_get_dest(const logger_t *lg) {
    if (lg == NULL || lg->dest == NULL) {
        return stdout;
    }
    return lg->dest;
}

// write a formatted message to destination
void logger_write(const logger_t *lg, const char *fmt, ...) {
    va_list args;
    FILE *dest = logger_get_dest(lg);

    if (fmt == NULL) return;

    va_start(args, fmt);
    vfprintf(dest, fmt, args);
    va_end(args);
}

/*
 * write tokens with proper formatting
 * in debug mode: line number + tokens + blank line
 * in release mode: just tokens
 */
void logger_write_tokens(logger_t *lg, int line_num, const char *token_str) {
    FILE *dest = logger_get_dest(lg);
    if (dest == NULL || token_str == NULL) return;

#if OUTFORMAT == OUTFORMAT_DEBUG
    fprintf(dest, "%d %s\n", line_num, token_str);
    fprintf(dest, "\n"); 
#else
    fprintf(dest, "%s\n", token_str);
#endif
}

/*
 * log operation counts with line and function info
 * format: [CNT] Line X | Func: name | COMP=n IO=n GEN=n
 */
void logger_log_counts(logger_t *lg, int line, const char *func,
                       long comp, long io, long gen) {
    FILE *dest = NULL;

    if (lg == NULL || func == NULL) return;

#ifdef COUNTCONFIG
    if (COUNTOUT == COUNTOUT_MAIN) {
        dest = logger_get_dest(lg);
    } else {
        dest = lg->count_dest;
        if (dest == NULL) dest = stdout;
    }

    fprintf(dest, "[CNT] Line %d | Func: %s | COMP=%ld IO=%ld GEN=%ld\n",
            line, func, comp, io, gen);
#endif
}


// close counter file if it was opened separately
void logger_close(logger_t *lg) {
    if (lg == NULL) return;

#ifdef COUNTCONFIG
    if (COUNTOUT == COUNTOUT_DBGCNT && lg->count_dest != NULL &&
        lg->count_dest != stdout && lg->count_file[0] != '\0') {
        fclose(lg->count_dest);
        lg->count_dest = NULL;
    }
#endif
}
