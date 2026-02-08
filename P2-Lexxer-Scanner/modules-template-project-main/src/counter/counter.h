/**
 * @file counter.h
 * @brief Counter Module (COUNTCONFIG) - STUB
 * 
 * ===============================================
 * COUNTER - COUNTCONFIG
 * ===============================================
 * 
 * Per-function & global totals tracking.
 * Compiles out when disabled.
 * 
 * Called by: driver, automata
 * Calls into: logger
 * 
 * TODO: This is a STUB - implement full functionality
 */

#ifndef COUNTER_H
#define COUNTER_H

#include <stdbool.h>

/* Counter IDs */
typedef enum {
    CNT_TOKENS_TOTAL,
    CNT_NUMBERS,
    CNT_IDENTIFIERS,
    CNT_KEYWORDS,
    CNT_LITERALS,
    CNT_OPERATORS,
    CNT_SPECIALCHARS,
    CNT_NONRECOGNIZED,
    CNT_ERRORS,
    CNT_LINES,
    /* Add more counters as needed */
    CNT_COUNT
} CounterID;

/**
 * @brief Initialize counter module
 * @param enabled Enable counting
 */
void counter_init(bool enabled);

/**
 * @brief Increment a counter
 * @param counter_id Counter to increment
 */
void counter_increment(CounterID counter_id);

/**
 * @brief Increment counter by value
 * @param counter_id Counter to increment
 * @param value Value to add
 */
void counter_add(CounterID counter_id, int value);

/**
 * @brief Get counter value
 * @param counter_id Counter ID
 * @return Current value
 */
int counter_get(CounterID counter_id);

/**
 * @brief Print all counters
 */
void counter_print_all(void);

/**
 * @brief Reset all counters
 */
void counter_reset(void);

/**
 * @brief Close counter module
 */
void counter_close(void);

#endif /* COUNTER_H */
