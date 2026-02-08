/**
 * @file counter.c
 * @brief Counter Module Implementation - STUB
 * 
 * TODO: This is a STUB - implement full functionality
 */

#include "counter.h"
#include <stdio.h>
#include <string.h>

/* Global state */
static bool g_enabled = false;
static int g_counters[CNT_COUNT];

/* Counter names */
static const char* COUNTER_NAMES[] = {
    "Total Tokens",
    "Numbers",
    "Identifiers",
    "Keywords",
    "Literals",
    "Operators",
    "Special Chars",
    "Non-Recognized",
    "Errors",
    "Lines"
};

void counter_init(bool enabled) {
    g_enabled = enabled;
    memset(g_counters, 0, sizeof(g_counters));
    printf("[COUNTER STUB] Counter module initialized (enabled=%s)\n", 
           enabled ? "YES" : "NO");
}

void counter_increment(CounterID counter_id) {
    if (!g_enabled) return;
    if (counter_id < 0 || counter_id >= CNT_COUNT) return;
    g_counters[counter_id]++;
}

void counter_add(CounterID counter_id, int value) {
    if (!g_enabled) return;
    if (counter_id < 0 || counter_id >= CNT_COUNT) return;
    g_counters[counter_id] += value;
}

int counter_get(CounterID counter_id) {
    if (counter_id < 0 || counter_id >= CNT_COUNT) return 0;
    return g_counters[counter_id];
}

void counter_print_all(void) {
    if (!g_enabled) {
        printf("[COUNTER STUB] Counting disabled\n");
        return;
    }
    
    printf("\n=== COUNTERS ===\n");
    for (int i = 0; i < CNT_COUNT; i++) {
        printf("  %s: %d\n", COUNTER_NAMES[i], g_counters[i]);
    }
    printf("================\n");
}

void counter_reset(void) {
    memset(g_counters, 0, sizeof(g_counters));
}

void counter_close(void) {
    printf("[COUNTER STUB] Counter module closed\n");
    if (g_enabled) {
        counter_print_all();
    }
}
