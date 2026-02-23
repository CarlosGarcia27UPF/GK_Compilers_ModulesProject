/*
 * =============================================================================
 * dfa.c
 * Implementation of the Deterministic Finite Automaton (DFA) operations.
 *
 * The DFA drives the SRA parse loop by storing the ACTION table (state +
 * terminal → action) and the GOTO table (state + non-terminal → next state).
 * The SRA engine calls these operations to advance the automaton during
 * SHIFT, REDUCE, and ACCEPT steps.
 *
 * Author: [Team]
 * =============================================================================
 */

#include <string.h>
#include "dfa.h"

/* Initialise the DFA: zero all fields so every action defaults to ERROR. */
void dfa_init(dfa_t *dfa)
{
    memset(dfa, 0, sizeof(*dfa));
#if TRACE_DFA
    fprintf(stderr, "[DFA] dfa_init called\n");
#endif
}

/* Return the ACTION entry for (state, terminal index).
 * Returns an ACTION_ERROR entry for any out-of-range index. */
action_t dfa_get_action(const dfa_t *dfa, int state, int term)
{
    action_t err = {ACTION_ERROR, 0};
    if (state < 0 || state >= dfa->num_states)    return err;
    if (term  < 0 || term  >= dfa->num_terminals) return err;
    return dfa->action_table[state][term];
}

/* Return the GOTO entry for (state, non-terminal index).
 * Returns -1 for any out-of-range index. */
int dfa_get_goto(const dfa_t *dfa, int state, int nonterm)
{
    if (state   < 0 || state   >= dfa->num_states)        return -1;
    if (nonterm < 0 || nonterm >= dfa->num_nonterminals)  return -1;
    return dfa->goto_table[state][nonterm];
}

/* Set an ACTION entry. Silently ignores out-of-range indices. */
void dfa_set_action(dfa_t *dfa, int state, int term, action_t a)
{
    if (state < 0 || state >= dfa->num_states)    return;
    if (term  < 0 || term  >= dfa->num_terminals) return;
    dfa->action_table[state][term] = a;
}

/* Set a GOTO entry. Silently ignores out-of-range indices. */
void dfa_set_goto(dfa_t *dfa, int state, int nonterm, int goto_state)
{
    if (state   < 0 || state   >= dfa->num_states)        return;
    if (nonterm < 0 || nonterm >= dfa->num_nonterminals)  return;
    dfa->goto_table[state][nonterm] = goto_state;
}
