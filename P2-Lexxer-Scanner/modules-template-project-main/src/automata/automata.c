/**
 * @file automata.c
 * @brief Implementation of Automata Module (Scanner Engine)
 * 
 * ONLY module that consumes input via cs_get().
 * All lexical decisions are made here.
 */

#include "automata.h"
#include <string.h>

/* ======================= Helper Functions ======================= */

void lexeme_append(char* lexeme, char c, int* len) {
    if (*len < MAX_LEXEME_LEN - 1) {
        lexeme[*len] = c;
        (*len)++;
        lexeme[*len] = '\0';
    }
}

void automata_skip_whitespace(CharStream* cs) {
    while (!cs_is_eof(cs)) {
        char c = cs_peek(cs);
        if (lang_is_whitespace(c)) {
            cs_get(cs);  /* Consume whitespace */
        } else {
            break;
        }
    }
}

/* ======================= Main Scanner ======================= */

ScanResult automata_scan_next(CharStream* cs, Token* token) {
    if (cs == NULL || token == NULL) {
        return SCAN_ERROR;
    }
    
    /* Skip whitespace first */
    automata_skip_whitespace(cs);
    
    /* Check for EOF */
    if (cs_is_eof(cs)) {
        *token = token_create("", CAT_EOF, cs_get_line(cs));
        return SCAN_EOF;
    }
    
    /* Get lookahead to select automaton */
    char c = cs_peek(cs);
    
    /* Select and run appropriate automaton based on first character */
    if (lang_is_digit(c)) {
        return automata_scan_number(cs, token);
    }
    else if (lang_is_letter(c)) {
        return automata_scan_identifier(cs, token);
    }
    else if (lang_is_quote(c)) {
        return automata_scan_literal(cs, token);
    }
    else if (lang_is_operator(c)) {
        return automata_scan_operator(cs, token);
    }
    else if (lang_is_special_char(c)) {
        return automata_scan_special_char(cs, token);
    }
    else {
        /* Non-recognized character(s) */
        return automata_scan_non_recognized(cs, token);
    }
}

/* ======================= NUMBER Automaton ======================= */
/**
 * DFA for: [0-9]+
 * 
 * Transition Table:
 * ┌────────┬─────────┐
 * │ State  │  digit  │
 * ├────────┼─────────┤
 * │   S0   │   S1    │
 * │   S1   │   S1    │
 * └────────┴─────────┘
 * 
 * Accepting: {S1}
 */
ScanResult automata_scan_number(CharStream* cs, Token* token) {
    char lexeme[MAX_LEXEME_LEN] = "";
    int len = 0;
    int line = cs_get_line(cs);
    NumberState state = NUMBER_S0;
    
    /* DFA loop with lookahead */
    while (!cs_is_eof(cs)) {
        char c = cs_peek(cs);
        
        /* Check if char is in alphabet Σ = {0-9} */
        if (!lang_is_digit(c)) {
            break;  /* Lookahead stop */
        }
        
        /* Consume and append */
        lexeme_append(lexeme, cs_get(cs), &len);
        
        /* Apply transition δ */
        switch (state) {
            case NUMBER_S0:
                state = NUMBER_S1;  /* δ(S0, digit) = S1 */
                break;
            case NUMBER_S1:
                /* δ(S1, digit) = S1, stay */
                break;
        }
    }
    
    /* Check accepting state */
    if (state == NUMBER_S1) {
        *token = token_create(lexeme, CAT_NUMBER, line);
        return SCAN_OK;
    }
    
    /* Should not happen if automaton was selected correctly */
    *token = token_create(lexeme, CAT_ERROR, line);
    return SCAN_ERROR;
}

/* ======================= IDENTIFIER Automaton ======================= */
/**
 * DFA for: [A-Za-z][A-Za-z0-9]*
 * 
 * Transition Table:
 * ┌────────┬─────────┬─────────┐
 * │ State  │ letter  │  digit  │
 * ├────────┼─────────┼─────────┤
 * │   S0   │   S1    │    -    │
 * │   S1   │   S1    │   S1    │
 * └────────┴─────────┴─────────┘
 * 
 * Accepting: {S1}
 * Post-processing: check if lexeme is keyword
 */
ScanResult automata_scan_identifier(CharStream* cs, Token* token) {
    char lexeme[MAX_LEXEME_LEN] = "";
    int len = 0;
    int line = cs_get_line(cs);
    IdentState state = IDENT_S0;
    
    /* DFA loop with lookahead */
    while (!cs_is_eof(cs)) {
        char c = cs_peek(cs);
        
        /* State-dependent transitions */
        if (state == IDENT_S0) {
            /* From S0: only letter allowed */
            if (!lang_is_letter(c)) {
                break;
            }
            lexeme_append(lexeme, cs_get(cs), &len);
            state = IDENT_S1;  /* δ(S0, letter) = S1 */
        }
        else if (state == IDENT_S1) {
            /* From S1: letter or digit allowed */
            if (!lang_is_letter_or_digit(c)) {
                break;  /* Lookahead stop */
            }
            lexeme_append(lexeme, cs_get(cs), &len);
            /* δ(S1, letter|digit) = S1, stay */
        }
    }
    
    /* Check accepting state */
    if (state == IDENT_S1) {
        /* POST-PROCESSING: Check if it's a keyword */
        if (lang_is_keyword(lexeme)) {
            *token = token_create(lexeme, CAT_KEYWORD, line);
            token->keyword = lang_get_keyword_type(lexeme);
        } else {
            *token = token_create(lexeme, CAT_IDENTIFIER, line);
        }
        return SCAN_OK;
    }
    
    *token = token_create(lexeme, CAT_ERROR, line);
    return SCAN_ERROR;
}

/* ======================= LITERAL Automaton ======================= */
/**
 * DFA for: "[^"]*"
 * 
 * Transition Table:
 * ┌────────┬─────────┬─────────┬────────────┐
 * │ State  │    "    │  other  │  \n / EOF  │
 * ├────────┼─────────┼─────────┼────────────┤
 * │   S0   │   S1    │    -    │     -      │
 * │   S1   │   S2    │   S1    │   Serr     │
 * └────────┴─────────┴─────────┴────────────┘
 * 
 * Accepting: {S2}
 */
ScanResult automata_scan_literal(CharStream* cs, Token* token) {
    char lexeme[MAX_LEXEME_LEN] = "";
    int len = 0;
    int line = cs_get_line(cs);
    LiteralState state = LITERAL_S0;
    
    /* DFA loop */
    while (!cs_is_eof(cs)) {
        char c = cs_peek(cs);
        
        switch (state) {
            case LITERAL_S0:
                /* Expect opening quote */
                if (c == '"') {
                    lexeme_append(lexeme, cs_get(cs), &len);
                    state = LITERAL_S1;  /* δ(S0, ") = S1 */
                } else {
                    /* Should not happen */
                    *token = token_create(lexeme, CAT_ERROR, line);
                    return SCAN_ERROR;
                }
                break;
                
            case LITERAL_S1:
                /* Inside literal */
                if (c == '"') {
                    /* Closing quote - δ(S1, ") = S2 */
                    lexeme_append(lexeme, cs_get(cs), &len);
                    state = LITERAL_S2;
                    goto done;  /* S2 is accepting, stop */
                }
                else if (c == '\n' || c == '\0') {
                    /* Unterminated literal - δ(S1, \n|EOF) = Serr */
                    state = LITERAL_SERR;
                    goto done;
                }
                else {
                    /* Regular content - δ(S1, other) = S1 */
                    lexeme_append(lexeme, cs_get(cs), &len);
                }
                break;
                
            case LITERAL_S2:
            case LITERAL_SERR:
                goto done;
        }
    }
    
    /* If we reach here with S1, it's unterminated */
    if (state == LITERAL_S1) {
        state = LITERAL_SERR;
    }
    
done:
    if (state == LITERAL_S2) {
        *token = token_create(lexeme, CAT_LITERAL, line);
        return SCAN_OK;
    }
    else if (state == LITERAL_SERR) {
        /* Error: unterminated literal */
        *token = token_create(lexeme, CAT_NONRECOGNIZED, line);
        /* TODO: Report error via error module */
        return SCAN_ERROR;
    }
    
    *token = token_create(lexeme, CAT_ERROR, line);
    return SCAN_ERROR;
}

/* ======================= OPERATOR Automaton ======================= */
/**
 * DFA for operators (supports 1-2 char operators):
 * Single: = > < + - * /
 * Double: == >= <= != ++ -- 
 * 
 * Uses lookahead for maximal munch
 */
ScanResult automata_scan_operator(CharStream* cs, Token* token) {
    int line = cs_get_line(cs);
    char c1 = cs_peek(cs);
    
    if (!lang_is_operator(c1)) {
        *token = token_create("", CAT_ERROR, line);
        return SCAN_ERROR;
    }
    
    /* Consume first character */
    cs_get(cs);
    
    /* Check for two-character operators */
    char c2 = cs_peek(cs);
    char lexeme[3] = {c1, '\0', '\0'};
    
    /* Possible two-char operators: == >= <= != ++ -- */
    if ((c1 == '=' && c2 == '=') ||
        (c1 == '>' && c2 == '=') ||
        (c1 == '<' && c2 == '=') ||
        (c1 == '!' && c2 == '=') ||
        (c1 == '+' && c2 == '+') ||
        (c1 == '-' && c2 == '-')) {
        lexeme[1] = cs_get(cs);  /* Consume second char */
    }
    
    *token = token_create(lexeme, CAT_OPERATOR, line);
    return SCAN_OK;
}

/* ======================= SPECIALCHAR Automaton ======================= */
/**
 * DFA for: ( | ) | ; | { | } | [ | ] | ,
 * Single character - consume exactly one
 */
ScanResult automata_scan_special_char(CharStream* cs, Token* token) {
    int line = cs_get_line(cs);
    char c = cs_peek(cs);
    
    if (!lang_is_special_char(c)) {
        *token = token_create("", CAT_ERROR, line);
        return SCAN_ERROR;
    }
    
    char lexeme[2] = {cs_get(cs), '\0'};
    *token = token_create(lexeme, CAT_SPECIALCHAR, line);
    return SCAN_OK;
}

/* ======================= NONRECOGNIZED Automaton ======================= */
/**
 * DFA for: (non-valid char)+
 * Groups consecutive non-recognized characters
 * 
 * Transition Table:
 * ┌────────┬─────────────┐
 * │ State  │  non-valid  │
 * ├────────┼─────────────┤
 * │   S0   │     S1      │
 * │   S1   │     S1      │
 * └────────┴─────────────┘
 * 
 * Stop: when valid token start or whitespace or EOF
 */
ScanResult automata_scan_non_recognized(CharStream* cs, Token* token) {
    char lexeme[MAX_LEXEME_LEN] = "";
    int len = 0;
    int line = cs_get_line(cs);
    
    /* Consume all consecutive non-recognized chars */
    while (!cs_is_eof(cs)) {
        char c = cs_peek(cs);
        
        /* Stop if valid token start found */
        if (lang_is_valid_token_start(c)) {
            break;
        }
        
        /* Consume non-recognized char */
        lexeme_append(lexeme, cs_get(cs), &len);
    }
    
    if (len > 0) {
        *token = token_create(lexeme, CAT_NONRECOGNIZED, line);
        /* TODO: Report error via error module */
        return SCAN_OK;  /* Token created, but it's an error token */
    }
    
    *token = token_create("", CAT_ERROR, line);
    return SCAN_ERROR;
}
