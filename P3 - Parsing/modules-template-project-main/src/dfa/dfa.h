/*
 * =============================================================================
 * dfa.h
 * Deterministic Finite Automaton (DFA) data structure and operations.
 *
 * The DFA is the core transition mechanism of the Shift-Reduce Automaton (SRA).
 * It encapsulates the ACTION table (indexed by state and terminal) and the
 * GOTO table (indexed by state and non-terminal).
 *
 * Each operation of the SRA that needs to advance or query the automaton
 * should call the DFA operations defined here.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef DFA_H
#define DFA_H

/* ---- Trace flag ---- */
#define TRACE_DFA 0  /* Set to 1 to enable DFA debug trace output */

/* ---- Capacity limits for the DFA ---- */
#define DFA_MAX_STATES       64  /* Maximum number of automaton states         */
#define DFA_MAX_TERMINALS    32  /* Maximum number of terminal symbols (inputs) */
#define DFA_MAX_NONTERMINALS 16  /* Maximum number of non-terminal symbols      */

/* ---- SRA action types ---- */
#define ACTION_ERROR   0  /* No valid action (parse error)   */
#define ACTION_SHIFT   1  /* Shift: push next state          */
#define ACTION_REDUCE  2  /* Reduce: apply production rule n */
#define ACTION_ACCEPT  3  /* Accept: input fully parsed      */

/* ---- Data structures ---- */

/* One cell in the ACTION table */
typedef struct {
    int type;   /* ACTION_ERROR / ACTION_SHIFT / ACTION_REDUCE / ACTION_ACCEPT */
    int value;  /* SHIFT: target state number; REDUCE: rule number (1-based)   */
} action_t;

/* The DFA data structure: holds all automaton states and transition tables */
typedef struct {
    int      num_states;                                                /* Number of active states     */
    int      num_terminals;                                             /* Alphabet size (terminals)   */
    int      num_nonterminals;                                          /* Number of non-terminals     */
    action_t action_table[DFA_MAX_STATES][DFA_MAX_TERMINALS];           /* ACTION table                */
    int      goto_table[DFA_MAX_STATES][DFA_MAX_NONTERMINALS];          /* GOTO table                  */
} dfa_t;

/* ---- Function prototypes ---- */

/* Initialise a DFA to an empty / all-error state. */
void     dfa_init(dfa_t *dfa);

/* Return the ACTION entry for (state, terminal index).
 * Returns an ACTION_ERROR entry if indices are out of range. */
action_t dfa_get_action(const dfa_t *dfa, int state, int term);

/* Return the GOTO entry for (state, non-terminal index).
 * Returns -1 if indices are out of range. */
int      dfa_get_goto(const dfa_t *dfa, int state, int nonterm);

/* Set an ACTION entry. Silently ignores indices outside the configured
 * num_states / num_terminals dimensions. */
void     dfa_set_action(dfa_t *dfa, int state, int term, action_t a);

/* Set a GOTO entry. Silently ignores indices outside the configured
 * num_states / num_nonterminals dimensions. */
void     dfa_set_goto(dfa_t *dfa, int state, int nonterm, int goto_state);

#endif /* DFA_H */
