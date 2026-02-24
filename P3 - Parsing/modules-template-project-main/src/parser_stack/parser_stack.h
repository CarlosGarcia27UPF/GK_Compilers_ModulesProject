/*
 * =============================================================================
 * parser_stack.h
 * Shift-Reduce Automaton (SRA) stack data structure and operations.
 *
 * Each item on the stack stores the automaton state, the index and display
 * name of the grammar symbol that was pushed, and the original lexeme.
 * The bottom-of-stack sentinel has symbol_index == PS_BOTTOM_SYMBOL.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef PARSER_STACK_H
#define PARSER_STACK_H

#include <stdio.h>
#include "../lang_spec/lang_spec.h"

/* ---- Trace flag ---- */
#define TRACE_PARSER_STACK 0  /* Set to 1 to enable parser_stack debug trace output */

/* ---- Constants ---- */
#define PS_MAX_DEPTH       512  /* Maximum stack depth                              */
#define PS_BOTTOM_SYMBOL    -1  /* symbol_index for the bottom-of-stack item        */
#define PS_MAX_LEXEME      128  /* Max lexeme string length on the stack             */
#define PS_EMPTY_STRING     ""  /* Empty string used for bottom-sentinel name/lexeme */
#define PS_FMT_STATE       "%d" /* printf format for printing a state number         */
#define PS_FMT_ITEM        " (%d,%s)" /* printf format for printing one stack item   */

/* ---- Data structures ---- */

/* One item on the SRA parse stack */
typedef struct {
    int  state;                     /* Automaton state pushed with this symbol  */
    int  symbol_index;              /* Index in terminals or nonterminals array;
                                       PS_BOTTOM_SYMBOL for the initial sentinel */
    char symbol_name[LS_MAX_NAME];  /* Symbol name string for display            */
    char lexeme[PS_MAX_LEXEME];     /* Lexeme string (empty for non-terminals)   */
} ps_item_t;

/* The SRA parse stack */
typedef struct {
    ps_item_t items[PS_MAX_DEPTH];  /* Stack storage (items[0] = bottom)  */
    int       top;                  /* Index of current top item          */
} parser_stack_t;

/* ---- Function prototypes ---- */

/* Clear the stack and push the initial state-0 sentinel item. */
void ps_init(parser_stack_t *stack);

/* Push a new item onto the stack. */
void ps_push(parser_stack_t *stack, int state, int symbol_index,
             const char *symbol_name, const char *lexeme);

/* Pop the top item from the stack. */
void ps_pop(parser_stack_t *stack);

/* Return the state of the top stack item. */
int  ps_top_state(const parser_stack_t *stack);

/* Return 1 if the stack is empty (top < 0), 0 otherwise. */
int  ps_is_empty(const parser_stack_t *stack);

/* Write a human-readable stack trace to f.
 * Uses ls to look up symbol names when needed. */
void ps_print(const parser_stack_t *stack, const lang_spec_t *ls, FILE *f);

#endif /* PARSER_STACK_H */
