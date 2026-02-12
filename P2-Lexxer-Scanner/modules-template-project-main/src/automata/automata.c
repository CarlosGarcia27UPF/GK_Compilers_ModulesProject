/*
 * -----------------------------------------------------------------------------
 * automata.c
 *
 * Scanner engine implementation using multiple specialized automata.
 * Each token type (number, identifier, literal, operator, etc.) has its own
 * DFA with its own transition matrix. A dispatcher function routes to the
 * appropriate automata based on the first character's classification.
 *
 * Design:
 *   - MULTIPLE functions, one per token type: automata_scan_number(),
 *     automata_scan_identifier(), automata_scan_literal(), etc.
 *   - scanner_next_token() dispatcher that routes based on first char.
 *   - Character-by-character lookahead within each automata.
 *   - Keywords are reclassified after identifier acceptance.
 *   - Whitespace is consumed in the dispatcher.
 *   - Unterminated literals emit one error + NONRECOGNIZED token.
 *   - Grouped non-recognized chars emit one error per group.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "automata.h"
#include "../lang_spec/lang_spec.h"

// ============================================================================
// NUMBER AUTOMATA
// ============================================================================
// States: START, IN_NUMBER
// Transition matrix for number recognition [0-9]+

typedef enum {
    ST_NUM_START = 0,
    ST_NUM_IN    = 1,
    ST_NUM_COUNT = 2
} num_state_t;

static const num_state_t T_number[ST_NUM_COUNT][CC_COUNT] = {
    // ST_NUM_START: first digit must be present
    //               LETTER    DIGIT       QUOTE    OPERATOR SPECIAL SPACE   NEWLINE EOF     OTHER
    [ST_NUM_START] = {ST_NUM_IN, ST_NUM_IN, ST_NUM_IN, ST_NUM_IN, ST_NUM_IN, ST_NUM_IN, ST_NUM_IN, ST_NUM_IN, ST_NUM_IN},

    // ST_NUM_IN: accumulate digits only
    [ST_NUM_IN]    = {-1,        ST_NUM_IN, -1,       -1,       -1,       -1,       -1,       -1,       -1       },
};

// ============================================================================
// IDENTIFIER AUTOMATA
// ============================================================================
// States: START, IN_IDENT
// Transition matrix for identifier recognition [A-Za-z][A-Za-z0-9]*

typedef enum {
    ST_IDENT_START = 0,
    ST_IDENT_IN    = 1,
    ST_IDENT_COUNT = 2
} ident_state_t;

static const ident_state_t T_identifier[ST_IDENT_COUNT][CC_COUNT] = {
    // ST_IDENT_START: first letter must be present
    //                 LETTER      DIGIT      QUOTE  OPERATOR SPECIAL SPACE  NEWLINE EOF    OTHER
    [ST_IDENT_START] = {ST_IDENT_IN, ST_IDENT_IN, -1,    -1,      -1,     -1,    -1,     -1,    -1   },

    // ST_IDENT_IN: accumulate letters/digits
    [ST_IDENT_IN]    = {ST_IDENT_IN, ST_IDENT_IN, -1,    -1,      -1,     -1,    -1,     -1,    -1   },
};

// ============================================================================
// LITERAL AUTOMATA
// ============================================================================
// States: IN_LITERAL, LIT_END, ERROR
// Transition matrix for literal recognition "..."

typedef enum {
    ST_LIT_IN    = 0,
    ST_LIT_END   = 1,
    ST_LIT_ERROR = 2,
    ST_LIT_COUNT = 3
} lit_state_t;

static const lit_state_t T_literal[ST_LIT_COUNT][CC_COUNT] = {
    // ST_LIT_IN: accumulate any char except newline/EOF
    //            LETTER       DIGIT        QUOTE       OPERATOR     SPECIAL      SPACE        NEWLINE  EOF      OTHER
    [ST_LIT_IN]    = {ST_LIT_IN,   ST_LIT_IN,   ST_LIT_END, ST_LIT_IN,   ST_LIT_IN,   ST_LIT_IN,   -1,      -1,      ST_LIT_IN},

    // ST_LIT_END: closing quote consumed, accept
    [ST_LIT_END]   = {-1,          -1,          -1,         -1,          -1,          -1,          -1,      -1,      -1       },

    // ST_LIT_ERROR: error state
    [ST_LIT_ERROR] = {-1,          -1,          -1,         -1,          -1,          -1,          -1,      -1,      -1       },
};

// ============================================================================
// NONRECOGNIZED AUTOMATA
// ============================================================================
// States: IN_NONREC
// Transition matrix for grouping consecutive non-recognized chars

typedef enum {
    ST_NONREC_IN    = 0,
    ST_NONREC_COUNT = 1
} nonrec_state_t;

static const nonrec_state_t T_nonrec[ST_NONREC_COUNT][CC_COUNT] = {
    // ST_NONREC_IN: accumulate non-recognized chars
    //               LETTER   DIGIT   QUOTE   OPERATOR SPECIAL SPACE  NEWLINE EOF    OTHER
    [ST_NONREC_IN] = {-1,      -1,     -1,     -1,      -1,     -1,    -1,     -1,    ST_NONREC_IN},
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Maps one character to a DFA class.
char_class_t classify_char(int ch) {
    if (ch == CS_EOF) {
        return CC_EOF;
    }
    if (ls_is_letter((char)ch)) {
        return CC_LETTER;
    }
    if (ls_is_digit((char)ch)) {
        return CC_DIGIT;
    }
    if (ls_is_quote((char)ch)) {
        return CC_QUOTE;
    }
    if (ls_is_operator((char)ch)) {
        return CC_OPERATOR;
    }
    if (ls_is_special_char((char)ch)) {
        return CC_SPECIAL;
    }
    if ((char)ch == WS_NL) {
        return CC_NEWLINE;
    }
    if (ls_is_whitespace((char)ch)) {
        return CC_SPACE;
    }
    return CC_OTHER;
}

// Appends one character to the token buffer with bounds check.
static void add_char_to_lexeme(char *buf, int *len, int ch) {
    if (*len < MAX_LEXEME_LEN - 1) {
        buf[*len] = (char)ch;
        (*len)++;
        buf[*len] = '\0';
    }
}

// Reports grouped non-recognized lexeme.
static void report_nonrecognized(logger_t *lg, int line, const char *lexeme) {
    err_report(logger_get_dest(lg), ERR_NONRECOGNIZED, ERR_STEP_SCANNER,
               line, lexeme);
}

// Reports unterminated literal.
static void report_unterminated_literal(logger_t *lg, int line,
                                        const char *lexeme) {
    err_report(logger_get_dest(lg), ERR_UNTERMINATED_LIT, ERR_STEP_SCANNER,
               line, lexeme);
}

// ============================================================================
// INDIVIDUAL AUTOMATA FUNCTIONS
// ============================================================================

// NUMBER AUTOMATA: recognizes [0-9]+
static int automata_scan_number(char_stream_t *cs, token_list_t *tokens,
                                 logger_t *lg, counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;
    char_class_t cls;

    buf[0] = '\0';

    while (1) {
        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);

        cls = classify_char(ch);
        CNT_COMP(cnt, 1);

        // Check transition: -1 means stop
        if (T_number[ST_NUM_IN][cls] == (num_state_t)-1) {
            break;
        }

        ch = cs_get(cs);
        CNT_IO(cnt, 1);
        CNT_GEN(cnt, 1);
        add_char_to_lexeme(buf, &buf_len, ch);
    }

    // Emit number token
    token_t tok;
    token_init(&tok, buf, CAT_NUMBER, tok_line, tok_col);
    tl_add(tokens, &tok);
    return 1;
}

// IDENTIFIER AUTOMATA: recognizes [A-Za-z][A-Za-z0-9]*
static int automata_scan_identifier(char_stream_t *cs, token_list_t *tokens,
                                     logger_t *lg, counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;
    char_class_t cls;
    ident_state_t state = ST_IDENT_START;

    buf[0] = '\0';

    while (1) {
        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);

        cls = classify_char(ch);
        CNT_COMP(cnt, 1);

        // Check transition: -1 means stop
        if (state == ST_IDENT_START && T_identifier[ST_IDENT_START][cls] == (ident_state_t)-1) {
            break;
        }
        if (state == ST_IDENT_IN && T_identifier[ST_IDENT_IN][cls] == (ident_state_t)-1) {
            break;
        }

        ch = cs_get(cs);
        CNT_IO(cnt, 1);
        CNT_GEN(cnt, 1);
        add_char_to_lexeme(buf, &buf_len, ch);

        state = (state == ST_IDENT_START) ? ST_IDENT_IN : ST_IDENT_IN;
    }

    // Determine category: keyword or identifier
    token_category_t cat = ls_is_keyword(buf) ? CAT_KEYWORD : CAT_IDENTIFIER;

    // Emit token
    token_t tok;
    token_init(&tok, buf, cat, tok_line, tok_col);
    tl_add(tokens, &tok);
    return 1;
}

// LITERAL AUTOMATA: recognizes "..."
static int automata_scan_literal(char_stream_t *cs, token_list_t *tokens,
                                  logger_t *lg, counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;
    char_class_t cls;
    lit_state_t state = ST_LIT_IN;

    buf[0] = '\0';

    // Consume opening quote
    ch = cs_get(cs);
    CNT_IO(cnt, 1);
    CNT_GEN(cnt, 1);
    add_char_to_lexeme(buf, &buf_len, ch);

    while (1) {
        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);

        cls = classify_char(ch);
        CNT_COMP(cnt, 1);

        // Check for error conditions (newline or EOF)
        if (cls == CC_NEWLINE || cls == CC_EOF) {
            // Unterminated literal
            report_unterminated_literal(lg, tok_line, buf);
            token_t tok;
            token_init(&tok, buf, CAT_NONRECOGNIZED, tok_line, tok_col);
            tl_add(tokens, &tok);
            return 1;
        }

        // Check transition
        if (T_literal[ST_LIT_IN][cls] == (lit_state_t)-1) {
            break;
        }

        ch = cs_get(cs);
        CNT_IO(cnt, 1);
        CNT_GEN(cnt, 1);
        add_char_to_lexeme(buf, &buf_len, ch);

        // Check if we reached closing quote
        if (cls == CC_QUOTE) {
            state = ST_LIT_END;
            break;
        }
    }

    // Emit literal token
    token_t tok;
    token_init(&tok, buf, CAT_LITERAL, tok_line, tok_col);
    tl_add(tokens, &tok);
    return 1;
}

// OPERATOR AUTOMATA: recognizes single-char operators
static int automata_scan_operator(char_stream_t *cs, token_list_t *tokens,
                                   logger_t *lg, counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;

    buf[0] = '\0';

    // Consume single operator character
    ch = cs_get(cs);
    CNT_IO(cnt, 1);
    CNT_GEN(cnt, 1);
    add_char_to_lexeme(buf, &buf_len, ch);

    // Emit operator token
    token_t tok;
    token_init(&tok, buf, CAT_OPERATOR, tok_line, tok_col);
    tl_add(tokens, &tok);
    return 1;
}

// SPECIAL CHARACTER AUTOMATA: recognizes single-char special tokens
static int automata_scan_specialchar(char_stream_t *cs, token_list_t *tokens,
                                      logger_t *lg, counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;

    buf[0] = '\0';

    // Consume single special character
    ch = cs_get(cs);
    CNT_IO(cnt, 1);
    CNT_GEN(cnt, 1);
    add_char_to_lexeme(buf, &buf_len, ch);

    // Emit special char token
    token_t tok;
    token_init(&tok, buf, CAT_SPECIALCHAR, tok_line, tok_col);
    tl_add(tokens, &tok);
    return 1;
}

// NONRECOGNIZED AUTOMATA: groups consecutive non-recognized chars
static int automata_scan_nonrecognized(char_stream_t *cs, token_list_t *tokens,
                                        logger_t *lg, counter_t *cnt) {
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;
    char_class_t cls;

    buf[0] = '\0';

    while (1) {
        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);

        cls = classify_char(ch);
        CNT_COMP(cnt, 1);

        // Check transition: -1 means stop
        if (T_nonrec[ST_NONREC_IN][cls] == (nonrec_state_t)-1) {
            break;
        }

        ch = cs_get(cs);
        CNT_IO(cnt, 1);
        CNT_GEN(cnt, 1);
        add_char_to_lexeme(buf, &buf_len, ch);
    }

    // Report error for grouped non-recognized characters
    report_nonrecognized(lg, tok_line, buf);

    // Emit nonrecognized token
    token_t tok;
    token_init(&tok, buf, CAT_NONRECOGNIZED, tok_line, tok_col);
    tl_add(tokens, &tok);
    return 1;
}

// ============================================================================
// DISPATCHER: SCANNER_NEXT_TOKEN
// ============================================================================

// Main scanner dispatcher: routes to appropriate automata based on first char
static int scanner_next_token(char_stream_t *cs, token_list_t *tokens,
                              logger_t *lg, counter_t *cnt) {
    int ch;
    char_class_t cls;

    // Skip whitespace and newlines
    while (1) {
        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);

        if (ch == CS_EOF) {
            return 0;  // End of file
        }

        cls = classify_char(ch);
        CNT_COMP(cnt, 1);

        if (cls != CC_SPACE && cls != CC_NEWLINE) {
            break;  // Found first non-whitespace character
        }

        cs_get(cs);
        CNT_IO(cnt, 1);
    }

    // Dispatch based on character classification
    switch (cls) {
        case CC_LETTER:
            return automata_scan_identifier(cs, tokens, lg, cnt);

        case CC_DIGIT:
            return automata_scan_number(cs, tokens, lg, cnt);

        case CC_QUOTE:
            return automata_scan_literal(cs, tokens, lg, cnt);

        case CC_OPERATOR:
            return automata_scan_operator(cs, tokens, lg, cnt);

        case CC_SPECIAL:
            return automata_scan_specialchar(cs, tokens, lg, cnt);

        default:
            // CC_OTHER or any unrecognized character class
            return automata_scan_nonrecognized(cs, tokens, lg, cnt);
    }
}

// ============================================================================
// PUBLIC INTERFACE
// ============================================================================

// Scanner loop until EOF.
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt) {
    while (scanner_next_token(cs, tokens, lg, cnt)) {
        // Continue scanning.
    }
    return 0;
}
