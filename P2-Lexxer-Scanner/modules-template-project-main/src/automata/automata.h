/*
 * -----------------------------------------------------------------------------
 * automata.h
 *
 * Scanner engine using multiple specialized automata for lexical analysis.
 * Each token type has its own DFA with its own transition matrix.
 * A dispatcher routes to the appropriate automata based on character class.
 * This is the ONLY module that consumes input characters (cs_get).
 *
 * Individual automata implemented:
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

// Note: Individual scanner states are now localized to each automata function.

// Scans complete input and appends all tokens to token_list.
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt);

#endif /* AUTOMATA_H */
