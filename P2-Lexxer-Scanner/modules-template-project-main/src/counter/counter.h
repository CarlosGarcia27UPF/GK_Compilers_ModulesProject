/*
 * -----------------------------------------------------------------------------
 * counter.h
 *
 * Operation counting system controlled by preprocessor flags.
 * When COUNTCONFIG is defined, the macros expand to increment calls;
 * otherwise they compile to nothing (zero overhead in RELEASE).
 *
 * Counters:
 *   COUNTCOMP - number of comparisons
 *   COUNTIO   - number of I/O characters read/written
 *   COUNTGEN  - general instruction count
 *
 * Output controlled by COUNTOUT:
 *   OUT=1  -> messages to output file
 *   DBGCOUNT=0 -> messages to <filename>.<ext>dbgcnt file
 *
 * Team Member: Emmanuel Kwabena Cooper Acheampong
 * -----------------------------------------------------------------------------
 */

#ifndef COUNTER_H
#define COUNTER_H

#include <stdio.h>

// Counter state.
typedef struct {
    long comp;     // Comparison counter.
    long io;       // I/O character counter.
    long gen;      // General instruction counter.
    long partial_comp;  // Per-function partial comparisons.
    long partial_io;    // Per-function partial I/O ops.
    long partial_gen;   // Per-function partial general ops.
    char current_func[64]; // Function currently being counted for partials.
    FILE *trace_dest;   // Destination for update traces.
    int trace_enabled;  // 1 when update traces are enabled.
} counter_t;

// Count output routing configuration.
#define COUNTOUT_STDOUT 0
#define COUNTOUT_OUT    1

#ifndef COUNTOUT
#define COUNTOUT COUNTOUT_STDOUT
#endif

#define COUNTFILE_DBGCNT 0
#define COUNTFILE_OUTPUT 1

#ifndef COUNTFILE
#define COUNTFILE COUNTFILE_DBGCNT
#endif

// Resets all counters to zero.
void counter_init(counter_t *cnt);

// Configures trace destination and enables/disables update tracing.
void counter_set_trace(counter_t *cnt, FILE *dest, int enabled);

// Increments comparison counter with function/line metadata.
void counter_add_comp_trace(counter_t *cnt, long amount, const char *func_name,
                            int line);

// Increments I/O counter with function/line metadata.
void counter_add_io_trace(counter_t *cnt, long amount, const char *func_name,
                          int line);

// Increments general counter with function/line metadata.
void counter_add_gen_trace(counter_t *cnt, long amount, const char *func_name,
                           int line);

// Increments comparison counter.
void counter_add_comp(counter_t *cnt, long amount);

// Increments I/O counter.
void counter_add_io(counter_t *cnt, long amount);

// Increments general counter.
void counter_add_gen(counter_t *cnt, long amount);

// Prints current counter summary.
void counter_print(const counter_t *cnt, FILE *dest, const char *func_name,
                   int line);

// Preprocessor macros for zero-overhead counting.
#ifdef COUNTCONFIG

#define CNT_COMP(cnt_ptr, n)  counter_add_comp_trace((cnt_ptr), (n), __func__, __LINE__)
#define CNT_IO(cnt_ptr, n)    counter_add_io_trace((cnt_ptr), (n), __func__, __LINE__)
#define CNT_GEN(cnt_ptr, n)   counter_add_gen_trace((cnt_ptr), (n), __func__, __LINE__)

#else

#define CNT_COMP(cnt, n)  // no-op
#define CNT_IO(cnt, n)    // no-op
#define CNT_GEN(cnt, n)   // no-op

#endif /* COUNTCONFIG */

#endif /* COUNTER_H */
