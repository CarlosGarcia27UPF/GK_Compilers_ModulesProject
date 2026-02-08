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
#include "../error_mod/error_mod.h"
#include "../logger/logger.h"
#include "../counter/counter.h"

// Character class indices for transition matrix columns.
typedef enum {
    CC_LETTER   = 0,   // [A-Za-z]
    CC_DIGIT    = 1,   // [0-9]
    CC_QUOTE    = 2,   // "
    CC_OPERATOR = 3,   // = > + *
    CC_SPECIAL  = 4,   // ( ) ; { } [ ] ,
    CC_SPACE    = 5,   // whitespace
    CC_NEWLINE  = 6,   // \n
    CC_EOF      = 7,   // end of file
    CC_OTHER    = 8,   // any other character
    CC_COUNT    = 9    // number of classes
} char_class_t;

// Scanner states used by DFA rows.
typedef enum {
    ST_START       = 0,  // initial / idle
    ST_IN_NUMBER   = 1,  // reading number
    ST_IN_IDENT    = 2,  // reading identifier
    ST_IN_LITERAL  = 3,  // reading literal body
    ST_ACCEPT_OP   = 4,  // accepted operator
    ST_ACCEPT_SC   = 5,  // accepted special char
    ST_IN_NONREC   = 6,  // reading non-recognized chars
    ST_LIT_END     = 7,  // accepted closing quote
    ST_ERROR       = 8,  // scanner error
    ST_STOP        = 9,  // stop marker
    ST_COUNT       = 10  // number of states
} scan_state_t;

// Scans complete input and appends all tokens to token_list.
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt);

// Returns the character class for a character.
char_class_t classify_char(int ch);

#endif /* AUTOMATA_H */
