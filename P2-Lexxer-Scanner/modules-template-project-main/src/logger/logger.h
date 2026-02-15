/*
routes all messages to either stdout or a file based on debug_flag
 debug on (1) = go to output file
 debug off (0) = go to stdout
supports two output modes:
 - release: just the tokens
 - debug: tokens with line numbers and blank lines for readability
  
 operation counting with zero overhead in release mode when disabled
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifndef DEBUG_FLAG
#define DEBUG_FLAG 0  /* 0 = stdout, 1 = output file */
#endif

#define DEBUG_ON  1
#define DEBUG_OFF 0


#ifndef OUTFORMAT
#define OUTFORMAT 0   /* 0 = release (minimal), 1 = debug (with line numbers) */
#endif

#define OUTFORMAT_RELEASE 0
#define OUTFORMAT_DEBUG   1

// counter output routing configuration
#ifndef COUNTOUT
#define COUNTOUT 0    /* 0 = separate .dbgcnt file, 1 = main output file */
#endif

#define COUNTOUT_DBGCNT 0
#define COUNTOUT_MAIN   1

// This structure to hold counts for operations, only used if COUNTCONFIG is defined
typedef struct {
    long comp;  //comparison operations 
    long io;    // i/o operations 
    long gen;   // other instructions 
} count_tracker_t;

// logger state structure
typedef struct {
    FILE *dest;        // where to write messages 
    FILE *count_dest;  // where to write counter messages 
    char count_file[256];  // filename for .dbgcnt if needed  
} logger_t;

// initialize logger - pass output filename for counter file if needed 
void logger_init(logger_t *lg, FILE *outfile, const char *input_filename);

// get the message destination FILE* (either stdout or the output file)
FILE* logger_get_dest(const logger_t *lg);

// write a message to the destination with printf format
void logger_write(const logger_t *lg, const char *fmt, ...);

// write tokens with proper formatting (line number in debug mode) 
void logger_write_tokens(logger_t *lg, int line_num, const char *token_str);

// log operation counts with line and function context, only if COUNTCONFIG is defined
void logger_log_counts(logger_t *lg, int line, const char *func,
                       long comp, long io, long gen);

// close counter file if opened separately and clean up logger state
void logger_close(logger_t *lg);

// counter macros - expands to nothing when COUNTCONFIG not defined to avoid any overhead
#ifdef COUNTCONFIG
  #define COUNT_COMP(cnt, n)  ((cnt)->comp += (n))
  #define COUNT_IO(cnt, n)    ((cnt)->io += (n))
  #define COUNT_GEN(cnt, n)   ((cnt)->gen += (n))
#else
  #define COUNT_COMP(cnt, n)  ((void)0)
  #define COUNT_IO(cnt, n)    ((void)0)
  #define COUNT_GEN(cnt, n)   ((void)0)
#endif

#endif /* LOGGER_H */
