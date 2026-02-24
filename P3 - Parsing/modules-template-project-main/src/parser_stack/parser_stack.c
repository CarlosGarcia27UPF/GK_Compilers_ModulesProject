/*
 * =============================================================================
 * parser_stack.c
 * Implementation of the SRA parse-stack operations.
 *
 * Author: [Team]
 * =============================================================================
 */

#include <string.h>
#include <stdio.h>
#include "parser_stack.h"

/* Clear the stack and push the initial state-0 bottom-of-stack sentinel. */
void ps_init(parser_stack_t *stack)
{
    memset(stack, 0, sizeof(*stack));
    stack->top = -1;
    ps_push(stack, 0, PS_BOTTOM_SYMBOL, PS_EMPTY_STRING, PS_EMPTY_STRING);
}

/* Push a new item. Silently ignores overflow (stack depth >= PS_MAX_DEPTH). */
void ps_push(parser_stack_t *stack, int state, int symbol_index,
             const char *symbol_name, const char *lexeme)
{
    if (stack->top + 1 >= PS_MAX_DEPTH) {
        fprintf(stderr, "ps_push: stack overflow\n");
        return;
    }
    stack->top++;
    ps_item_t *item  = &stack->items[stack->top];
    item->state        = state;
    item->symbol_index = symbol_index;
    strncpy(item->symbol_name, symbol_name ? symbol_name : PS_EMPTY_STRING,
            LS_MAX_NAME - 1);
    item->symbol_name[LS_MAX_NAME - 1] = '\0';
    strncpy(item->lexeme, lexeme ? lexeme : PS_EMPTY_STRING, PS_MAX_LEXEME - 1);
    item->lexeme[PS_MAX_LEXEME - 1] = '\0';
#if TRACE_PARSER_STACK
    fprintf(stderr, "[STACK] push state=%d sym=%s top=%d\n",
            state, symbol_name ? symbol_name : PS_EMPTY_STRING, stack->top);
#endif
}

/* Pop the top item.  Silently ignores underflow. */
void ps_pop(parser_stack_t *stack)
{
    if (stack->top > 0) stack->top--;
}

/* Return the state stored in the top item. */
int ps_top_state(const parser_stack_t *stack)
{
    if (stack->top < 0) return 0;
    return stack->items[stack->top].state;
}

/* Return 1 if the stack is logically empty (no items at all), 0 otherwise. */
int ps_is_empty(const parser_stack_t *stack)
{
    return stack->top < 0;
}

/* Write a compact stack trace to f.
 * Format: "0 (s1,sym1) (s2,sym2) ..." where 0 is the bottom-of-stack state. */
void ps_print(const parser_stack_t *stack, const lang_spec_t *ls, FILE *f)
{
    (void)ls; /* reserved for future name lookup */
    for (int i = 0; i <= stack->top; i++) {
        const ps_item_t *item = &stack->items[i];
        if (item->symbol_index == PS_BOTTOM_SYMBOL) {
            fprintf(f, PS_FMT_STATE, item->state);
        } else {
            fprintf(f, PS_FMT_ITEM, item->state, item->symbol_name);
        }
    }
}
