/*
 * =============================================================================
 * sra.c
 * Implementation of the Shift-Reduce Automaton (SRA) parse engine.
 *
 * The SRA is composed of two components: the DFA (action/goto tables) and
 * the parser stack.  Each step calls DFA operations (dfa_get_action,
 * dfa_get_goto) and stack operations (ps_push, ps_pop, ps_top_state):
 *
 *   1. Inspect the current state (top of stack) and current token.
 *   2. Look up the DFA ACTION table:
 *        SHIFT  -> push token + next state (via DFA transition)
 *        REDUCE -> pop RHS symbols, push LHS via DFA GOTO table
 *        ACCEPT -> parsing complete successfully
 *        ERROR  -> report error and halt
 *
 * Author: [Team]
 * =============================================================================
 */

#include <string.h>
#include "sra.h"
#include "../dfa/dfa.h"

/* ---- Helpers ---- */

/* Return the terminal index for the token at position pos.
 * Returns the EOF terminal index when pos is beyond the last token. */
static int get_input_terminal(const parser_token_list_t *tokens,
                               const lang_spec_t *ls, int pos)
{
    if (pos < tokens->count) return tokens->tokens[pos].term_index;
    /* Find EOF terminal */
    return ls_map_token(ls, NULL, NULL);
}

/* Return terminal name for terminal index, or UNKNOWN_SYMBOL_NAME on invalid index. */
static const char *term_name(const lang_spec_t *ls, int term_idx)
{
    if (term_idx < 0 || term_idx >= ls->dfa.num_terminals) return UNKNOWN_SYMBOL_NAME;
    return ls->terminals[term_idx].name;
}

/* ---- Action handlers ---- */

/* Execute a SHIFT action: push the new state and advance pos. */
static void do_shift(parser_stack_t *stack,
                     const parser_token_list_t *tokens,
                     int pos, int next_state,
                     const lang_spec_t *ls, FILE *dbg)
{
    int prev_state  = ps_top_state(stack);
    int term_idx    = tokens->tokens[pos].term_index;
    const char *lex = tokens->tokens[pos].lexeme;
    const char *tn  = term_name(ls, term_idx);

    ps_push(stack, next_state, term_idx, tn, lex);
    ow_write_shift(dbg, pos, prev_state, next_state, tn, lex, stack, ls);
}

/* Execute a REDUCE action: pop RHS, push LHS via DFA GOTO table. */
static void do_reduce(parser_stack_t *stack,
                      const lang_spec_t *ls,
                      int rule_num, int pos, FILE *dbg)
{
    int prev_state = ps_top_state(stack);
    const rule_t *r = &ls->rules[rule_num - 1]; /* rules are 1-indexed */

    for (int i = 0; i < r->rhs_len; i++) ps_pop(stack);

    int top_state  = ps_top_state(stack);
    int goto_state = dfa_get_goto(&ls->dfa, top_state, r->lhs_index);
    const char *lhs_name = ls->nonterminals[r->lhs_index].name;

    ps_push(stack, goto_state, r->lhs_index, lhs_name, "");
    ow_write_reduce(dbg, pos, rule_num, prev_state, goto_state,
                    r->desc, stack, ls);
}

/* Execute an ACCEPT action. */
static void do_accept(parser_stack_t *stack, const lang_spec_t *ls,
                      int pos, FILE *dbg)
{
    ow_write_accept(dbg, pos, ps_top_state(stack), stack, ls);
}

/* Execute an ERROR action. */
static void do_error(const lang_spec_t *ls, int cur_state, int cur_term,
                     int pos, FILE *dbg)
{
    fprintf(stderr, "sra: parse error at pos %d state %d token %s\n",
            pos, cur_state, term_name(ls, cur_term));
    ow_write_error(dbg, pos, cur_state, term_name(ls, cur_term));
}

/* ---- Public interface ---- */

/* Run the SRA engine. Returns 1 on ACCEPT, 0 on ERROR.
 * The SRA is driven by the DFA (action/goto tables) and the parser stack. */
int sra_run(const lang_spec_t *ls, const parser_token_list_t *tokens,
            parser_stack_t *stack, FILE *dbg_file)
{
    ps_init(stack);
    int pos = 0;

    for (;;) {
        int cur_state = ps_top_state(stack);
        int cur_term  = get_input_terminal(tokens, ls, pos);
        action_t act  = dfa_get_action(&ls->dfa, cur_state, cur_term);

#if TRACE_SRA
        fprintf(stderr, "[SRA] pos=%d state=%d term=%d action_type=%d\n",
                pos, cur_state, cur_term, act.type);
#endif

        if (act.type == ACTION_SHIFT) {
            do_shift(stack, tokens, pos, act.value, ls, dbg_file);
            pos++;
        } else if (act.type == ACTION_REDUCE) {
            do_reduce(stack, ls, act.value, pos, dbg_file);
        } else if (act.type == ACTION_ACCEPT) {
            do_accept(stack, ls, pos, dbg_file);
            return 1;
        } else {
            do_error(ls, cur_state, cur_term, pos, dbg_file);
            return 0;
        }
    }
}
