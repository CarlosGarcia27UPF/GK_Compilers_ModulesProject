/**
 * @file automata.h
 * @brief Automata Module (Scanner Engine)
 * 
 * ===============================================
 * AUTOMATA - SCANNER ENGINE
 * ===============================================
 * 
 * ONLY module allowed to consume input (cs_get).
 * All lexical decisions live here.
 * 
 * This module implements all DFAs:
 * - NUMBER:        [0-9]+
 * - IDENTIFIER:    [A-Za-z][A-Za-z0-9]*
 * - KEYWORD:       (post-processing of IDENTIFIER)
 * - LITERAL:       "[^"]*"
 * - OPERATOR:      = | > | + | *
 * - SPECIALCHAR:   ( | ) | ; | { | } | [ | ] | ,
 * - NONRECOGNIZED: (any invalid char)+
 * 
 * Called by: driver
 * Calls into: char_stream, lang_spec, token, token_list, error, counter, logger
 * 
 * DESIGN PRINCIPLE:
 * Each automaton uses:
 * - cs_peek() for lookahead (no consume)
 * - cs_get() for consuming characters
 * - Maximal munch: consume longest possible match
 * - Lookahead stops automaton when next char doesn't belong to token
 */

#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "../char_stream/char_stream.h"
#include "../lang_spec/lang_spec.h"
#include "../token/token.h"
#include <stdbool.h>

/* ======================= Automata States ======================= */

/**
 * NUMBER Automaton States
 * RE: [0-9]+
 * 
 * S0: Initial (not accepting) - no digits yet
 * S1: Accepting - at least one digit
 * 
 * δ(S0, digit) = S1
 * δ(S1, digit) = S1
 */
typedef enum {
    NUMBER_S0,  /* Initial */
    NUMBER_S1   /* Accepting */
} NumberState;

/**
 * IDENTIFIER Automaton States
 * RE: [A-Za-z][A-Za-z0-9]*
 * 
 * S0: Initial (not accepting)
 * S1: Accepting - inside identifier
 * 
 * δ(S0, letter) = S1
 * δ(S1, letter|digit) = S1
 */
typedef enum {
    IDENT_S0,   /* Initial */
    IDENT_S1    /* Accepting */
} IdentState;

/**
 * LITERAL Automaton States
 * RE: "[^"]*"
 * 
 * S0: Initial - waiting for opening "
 * S1: Inside literal - consuming content
 * S2: Accepting - closing " found
 * Serr: Error - unterminated literal
 * 
 * δ(S0, ") = S1
 * δ(S1, ") = S2
 * δ(S1, other) = S1
 * δ(S1, newline|EOF) = Serr
 */
typedef enum {
    LITERAL_S0,     /* Initial */
    LITERAL_S1,     /* Inside */
    LITERAL_S2,     /* Accepting */
    LITERAL_SERR    /* Error */
} LiteralState;

/* ======================= Scan Result ======================= */

/**
 * @brief Result of a scan operation
 */
typedef enum {
    SCAN_OK,        /* Token successfully scanned */
    SCAN_ERROR,     /* Error during scanning */
    SCAN_EOF        /* End of file reached */
} ScanResult;

/* ======================= Main Scanner Function ======================= */

/**
 * @brief Scan the next token from input
 * 
 * This is the main entry point. It:
 * 1. Skips whitespace
 * 2. Uses lookahead to select automaton
 * 3. Runs the selected automaton
 * 4. Returns the token
 * 
 * @param cs Character stream
 * @param token Output token
 * @return ScanResult
 */
ScanResult automata_scan_next(CharStream* cs, Token* token);

/* ======================= Individual Automata ======================= */

/**
 * @brief NUMBER automaton: [0-9]+
 * 
 * States: S0 (initial), S1 (accepting)
 * Transitions:
 *   δ(S0, digit) = S1
 *   δ(S1, digit) = S1
 * Stop: when lookahead is not a digit
 */
ScanResult automata_scan_number(CharStream* cs, Token* token);

/**
 * @brief IDENTIFIER automaton: [A-Za-z][A-Za-z0-9]*
 * Also handles KEYWORD post-processing
 * 
 * States: S0 (initial), S1 (accepting)
 * Transitions:
 *   δ(S0, letter) = S1
 *   δ(S1, letter|digit) = S1
 * Stop: when lookahead is not letter/digit
 * Post-process: check if lexeme is keyword
 */
ScanResult automata_scan_identifier(CharStream* cs, Token* token);

/**
 * @brief LITERAL automaton: "[^"]*"
 * 
 * States: S0, S1, S2 (accepting), Serr
 * Transitions:
 *   δ(S0, ") = S1
 *   δ(S1, ") = S2
 *   δ(S1, other) = S1
 *   δ(S1, newline|EOF) = Serr
 */
ScanResult automata_scan_literal(CharStream* cs, Token* token);

/**
 * @brief OPERATOR automaton: = | > | + | *
 * 
 * Single character token
 * Consumes exactly one operator character
 */
ScanResult automata_scan_operator(CharStream* cs, Token* token);

/**
 * @brief SPECIALCHAR automaton: ( ) ; { } [ ] ,
 * 
 * Single character token
 * Consumes exactly one special character
 */
ScanResult automata_scan_special_char(CharStream* cs, Token* token);

/**
 * @brief NONRECOGNIZED automaton: groups invalid chars
 * 
 * Consumes consecutive non-recognized characters
 * Stops when valid token start is found
 */
ScanResult automata_scan_non_recognized(CharStream* cs, Token* token);

/* ======================= Helper Functions ======================= */

/**
 * @brief Skip whitespace characters
 * @param cs Character stream
 */
void automata_skip_whitespace(CharStream* cs);

/**
 * @brief Append character to lexeme buffer
 * @param lexeme Buffer
 * @param c Character to append
 * @param len Current length (will be updated)
 */
void lexeme_append(char* lexeme, char c, int* len);

#endif /* AUTOMATA_H */
