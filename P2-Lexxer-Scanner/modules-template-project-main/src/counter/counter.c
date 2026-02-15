/*
 * -----------------------------------------------------------------------------
 * counter.c
 *
 * Counter implementation. Increments and prints operation counts.
 *
 * Team Member: Emmanuel Kwabena Cooper Acheampong
 * -----------------------------------------------------------------------------
 */

#include "counter.h"
#include <string.h>

#define COUNTER_UNKNOWN_FUNC "unknown"

// Starts a new partial scope when function changes.
static void counter_sync_scope(counter_t *cnt, const char *func_name) {
    if (cnt == NULL || func_name == NULL) {
        return;
    }
    if (strcmp(cnt->current_func, func_name) != 0) {
        strncpy(cnt->current_func, func_name, sizeof(cnt->current_func) - 1);
        cnt->current_func[sizeof(cnt->current_func) - 1] = '\0';
        cnt->partial_comp = 0;
        cnt->partial_io = 0;
        cnt->partial_gen = 0;
    }
}

// Emits one structured trace update.
static void counter_emit_update(const counter_t *cnt, const char *counter_name,
                                long amount, int line, const char *func_name) {
    if (cnt == NULL || counter_name == NULL || func_name == NULL) {
        return;
    }
    if (!cnt->trace_enabled || cnt->trace_dest == NULL) {
        return;
    }

    fprintf(cnt->trace_dest,
            "[COUNT] line=%d func=%s counter=%s amount=%ld "
            "partial{COMP=%ld IO=%ld GEN=%ld} total{COMP=%ld IO=%ld GEN=%ld}\n",
            line, func_name, counter_name, amount,
            cnt->partial_comp, cnt->partial_io, cnt->partial_gen,
            cnt->comp, cnt->io, cnt->gen);
}

// Zeros all counters.
void counter_init(counter_t *cnt) {
    if (cnt == NULL) {
        return;
    }
    cnt->comp = 0;
    cnt->io = 0;
    cnt->gen = 0;
    cnt->partial_comp = 0;
    cnt->partial_io = 0;
    cnt->partial_gen = 0;
    cnt->current_func[0] = '\0';
    cnt->trace_dest = stdout;
    cnt->trace_enabled = 0;
}

// Configures tracing behavior.
void counter_set_trace(counter_t *cnt, FILE *dest, int enabled) {
    if (cnt == NULL) {
        return;
    }
    cnt->trace_dest = dest;
    cnt->trace_enabled = enabled;
}

// Adds to comparison counter.
void counter_add_comp(counter_t *cnt, long amount) {
    counter_add_comp_trace(cnt, amount, COUNTER_UNKNOWN_FUNC, 0);
}

// Adds to comparison counter with metadata.
void counter_add_comp_trace(counter_t *cnt, long amount, const char *func_name,
                            int line) {
    if (cnt == NULL) {
        return;
    }
    if (func_name == NULL) {
        func_name = COUNTER_UNKNOWN_FUNC;
    }
    counter_sync_scope(cnt, func_name);
    cnt->comp += amount;
    cnt->partial_comp += amount;
    counter_emit_update(cnt, "COUNTCOMP", amount, line, func_name);
}

// Adds to I/O counter.
void counter_add_io(counter_t *cnt, long amount) {
    counter_add_io_trace(cnt, amount, COUNTER_UNKNOWN_FUNC, 0);
}

// Adds to I/O counter with metadata.
void counter_add_io_trace(counter_t *cnt, long amount, const char *func_name,
                          int line) {
    if (cnt == NULL) {
        return;
    }
    if (func_name == NULL) {
        func_name = COUNTER_UNKNOWN_FUNC;
    }
    counter_sync_scope(cnt, func_name);
    cnt->io += amount;
    cnt->partial_io += amount;
    counter_emit_update(cnt, "COUNTIO", amount, line, func_name);
}

// Adds to general counter.
void counter_add_gen(counter_t *cnt, long amount) {
    counter_add_gen_trace(cnt, amount, COUNTER_UNKNOWN_FUNC, 0);
}

// Adds to general counter with metadata.
void counter_add_gen_trace(counter_t *cnt, long amount, const char *func_name,
                           int line) {
    if (cnt == NULL) {
        return;
    }
    if (func_name == NULL) {
        func_name = COUNTER_UNKNOWN_FUNC;
    }
    counter_sync_scope(cnt, func_name);
    cnt->gen += amount;
    cnt->partial_gen += amount;
    counter_emit_update(cnt, "COUNTGEN", amount, line, func_name);
}

// Prints counter summary.
void counter_print(const counter_t *cnt, FILE *dest, const char *func_name,
                   int line) {
    if (cnt == NULL || dest == NULL || func_name == NULL) {
        return;
    }
    fprintf(dest,
            "[COUNTER] line=%d func=%s "
            "partial{COMP=%ld IO=%ld GEN=%ld} total{COMP=%ld IO=%ld GEN=%ld}\n",
            line, func_name,
            cnt->partial_comp, cnt->partial_io, cnt->partial_gen,
            cnt->comp, cnt->io, cnt->gen);
}
