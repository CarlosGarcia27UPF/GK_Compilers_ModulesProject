/*
 * -----------------------------------------------------------------------------
 * automata.h
 *
 * Scanner engine using automaton-based lexical analysis.
 * Implements a transition matrix (DFA) for character-by-character
 * token recognition. This is the ONLY module that consumes input
 * characters (cs_get). All lexical decisions live here.
 *
 * Automata implemented:
 *   - NUMBER:      [0-9]+
 *   - IDENTIFIER:  [A-Za-z][A-Za-z0-9]*  (post-check for keywords)
 *   - LITERAL:     "...", any char except newline/EOF inside
 *   - OPERATOR:    single char  = > + *
 *   - SPECIALCHAR: single char  ( ) ; { } [ ] ,
 *   - NONRECOGNIZED: consecutive invalid chars grouped into one token
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "../char_stream/char_stream.h"
#include "../token_list/token_list.h"
#include "../error/error.h"
#include "../logger/logger.h"
#include "../counter/counter.h"

// Character class indices for transition matrix columns.
typedef enum {
    CC_i = 0,
    CC_e,
    CC_w,
    CC_r,
    CC_c,
    CC_v,

    CC_f,
    CC_l,
    CC_s,
    CC_h,
    CC_a,
    CC_t,
    CC_n,
    CC_u,
    CC_o,
    CC_d,

    CC_LETTER,
    CC_DIGIT,
    CC_QUOTE,
    CC_OPERATOR,
    CC_SPECIAL,
    CC_SPACE,
    CC_NEWLINE,
    CC_EOF,
    CC_OTHER,

    CC_COUNT
} char_class_t;

// Scanner states used by DFA rows.
typedef enum {
    ST_START = 0,

    ST_IN_NUMBER,
    ST_IN_IDENT,
    ST_IN_LITERAL,
    ST_IN_NONREC,

    ST_ACCEPT_OP,
    ST_ACCEPT_SC,
    ST_LIT_END,

    // keyword chains
    ST_KW_I,
    ST_KW_IF,
    ST_KW_IN,
    ST_KW_INT,
    ST_KW_C,
    ST_KW_CH,
    ST_KW_CHA,
    ST_KW_CHAR,
    ST_KW_V,
    ST_KW_VO,
    ST_KW_VOI,
    ST_KW_VOID,

    ST_KW_E,
    ST_KW_EL,
    ST_KW_ELS,
    ST_KW_ELSE,

    ST_KW_W,
    ST_KW_WH,
    ST_KW_WHI,
    ST_KW_WHIL,
    ST_KW_WHILE,

    ST_KW_R,
    ST_KW_RE,
    ST_KW_RET,
    ST_KW_RETU,
    ST_KW_RETUR,
    ST_KW_RETURN,

    ST_ERROR,
    ST_STOP,

    ST_COUNT
} scan_state_t;


// Scans complete input and appends all tokens to token_list.
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt);

// Returns the character class for a character.
char_class_t classify_char(int ch);

#endif /* AUTOMATA_H */
