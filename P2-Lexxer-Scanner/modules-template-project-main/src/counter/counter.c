/*
 * -----------------------------------------------------------------------------
 * counter.c
 *
 * Counter implementation. Increments and prints operation counts.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "counter.h"

// Zeros all counters.
void counter_init(counter_t *cnt) {
    if (cnt == NULL) {
        return;
    }
    cnt->comp = 0;
    cnt->io = 0;
    cnt->gen = 0;
}

// Adds to comparison counter.
void counter_add_comp(counter_t *cnt, long amount) {
    if (cnt == NULL) {
        return;
    }
    cnt->comp += amount;
}

// Adds to I/O counter.
void counter_add_io(counter_t *cnt, long amount) {
    if (cnt == NULL) {
        return;
    }
    cnt->io += amount;
}

// Adds to general counter.
void counter_add_gen(counter_t *cnt, long amount) {
    if (cnt == NULL) {
        return;
    }
    cnt->gen += amount;
}

// Prints counter summary.
void counter_print(const counter_t *cnt, FILE *dest, const char *func_name,
                   int line) {
    if (cnt == NULL || dest == NULL || func_name == NULL) {
        return;
    }
    fprintf(dest, "[COUNTER] Line %d | Func: %s | COMP=%ld IO=%ld GEN=%ld\n",
            line, func_name, cnt->comp, cnt->io, cnt->gen);
}
