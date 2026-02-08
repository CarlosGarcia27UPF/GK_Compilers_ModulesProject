/*
 * -----------------------------------------------------------------------------
 * automata.c
 *
 * Scanner engine implementation. Uses a single DFA loop driven entirely
 * by the transition matrix T[state][class]. No external dispatch by
 * first character — all decisions happen inside the automaton.
 *
 * Design:
 *   - ONE function scanner_next_token() recognises each token.
 *   - Maximal munch via last_accept_state + last_accept_pos rollback.
 *   - Keywords are reclassified char-by-char after identifier acceptance.
 *   - Whitespace is consumed inside the DFA (START + WS → START).
 *   - Unterminated literals emit one error + NONRECOGNIZED token.
 *   - Grouped non-recognized chars emit one error per group.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "automata.h"
#include "../lang_spec/lang_spec.h"

// Transition matrix:
// rows = current state, columns = character class, value = next state.
// ST_STOP means "do not consume; emit token from last_accept_state".

static const scan_state_t T[ST_COUNT][CC_COUNT] = {
    // ST_START: branch by first character class.
    //           LETTER         DIGIT          QUOTE          OPERATOR       SPECIAL        SPACE          NEWLINE        EOF            OTHER
    [ST_START]    = {ST_IN_IDENT,  ST_IN_NUMBER,  ST_IN_LITERAL, ST_ACCEPT_OP,  ST_ACCEPT_SC,  ST_START,      ST_START,      ST_STOP,       ST_IN_NONREC },

    // ST_IN_NUMBER: accumulate digits only.
    [ST_IN_NUMBER]= {ST_STOP,      ST_IN_NUMBER,  ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    // ST_IN_IDENT: accumulate letters/digits.
    [ST_IN_IDENT] = {ST_IN_IDENT,  ST_IN_IDENT,   ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    // ST_IN_LITERAL: accept any char except newline/EOF terminates with error.
    [ST_IN_LITERAL]={ST_IN_LITERAL,ST_IN_LITERAL,  ST_LIT_END,    ST_IN_LITERAL, ST_IN_LITERAL, ST_IN_LITERAL, ST_ERROR,      ST_ERROR,      ST_IN_LITERAL},

    // ST_ACCEPT_OP: single-character operator.
    [ST_ACCEPT_OP]= {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    // ST_ACCEPT_SC: single-character special token.
    [ST_ACCEPT_SC]= {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    // ST_IN_NONREC: group consecutive non-recognized characters.
    [ST_IN_NONREC]= {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_IN_NONREC },

    // ST_LIT_END: closing quote consumed.
    [ST_LIT_END]  = {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    // ST_ERROR: terminal failure.
    [ST_ERROR]    = {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },

    // ST_STOP: marker state, not an active DFA state.
    [ST_STOP]     = {ST_STOP,      ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP,       ST_STOP      },
};

// Returns 1 when state is accepting.
static int is_accepting(scan_state_t st) {
    switch (st) {
        case ST_IN_NUMBER:
        case ST_IN_IDENT:
        case ST_IN_NONREC:
        case ST_ACCEPT_OP:
        case ST_ACCEPT_SC:
        case ST_LIT_END:
            return 1;
        default:
            return 0;
    }
}

// Maps accepting state to token category.
static token_category_t accept_category(scan_state_t st) {
    switch (st) {
        case ST_IN_NUMBER:  return CAT_NUMBER;
        case ST_IN_IDENT:   return CAT_IDENTIFIER; // Reclassified as keyword later.
        case ST_ACCEPT_OP:  return CAT_OPERATOR;
        case ST_ACCEPT_SC:  return CAT_SPECIALCHAR;
        case ST_LIT_END:    return CAT_LITERAL;
        case ST_IN_NONREC:  return CAT_NONRECOGNIZED;
        default:            return CAT_NONRECOGNIZED;
    }
}

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

// Scans one token with the DFA. Returns 1 when a token is emitted, 0 on EOF.
static int scanner_next_token(char_stream_t *cs, token_list_t *tokens,
                              logger_t *lg, counter_t *cnt) {
    scan_state_t state = ST_START;
    scan_state_t last_accept_state = ST_STOP; // ST_STOP means no accept yet.
    char buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line = cs_line(cs);
    int tok_col = cs_col(cs);
    int ch;
    char_class_t cls;
    scan_state_t next;

    buf[0] = '\0';

    while (1) {
        // Defensive check: ST_STOP should never be a current state.
        if (state == ST_STOP) {
            // internal error: force recovery by consuming 1 char
            char fallback[2];
            token_t tok;
            int fb_line = cs_line(cs);
            int fb_col  = cs_col(cs);
            ch = cs_get(cs);
            CNT_IO(cnt, 1);
            fallback[0] = (char)ch;
            fallback[1] = '\0';
            report_nonrecognized(lg, fb_line, fallback);
            token_init(&tok, fallback, CAT_NONRECOGNIZED, fb_line, fb_col);
            tl_add(tokens, &tok);
            return 1;  // continue scanning
        }

        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);

        cls = classify_char(ch);
        CNT_COMP(cnt, 1);

        next = T[state][cls];

        // Handle STOP/ERROR transitions.
        if (next == ST_STOP || next == ST_ERROR) {
            if (next == ST_ERROR && state == ST_IN_LITERAL) {
                // Unterminated literal: exactly one error + one token.
                report_unterminated_literal(lg, tok_line, buf);
                {
                    token_t tok;
                    token_init(&tok, buf, CAT_NONRECOGNIZED, tok_line, tok_col);
                    tl_add(tokens, &tok);
                }
                return 1;
            }

            if (last_accept_state != ST_STOP) {
                // Emit token from the last accepting state.
                token_category_t cat = accept_category(last_accept_state);
                token_t tok;

                // Reclassify accepted identifiers as keywords.
                if (cat == CAT_IDENTIFIER && ls_is_keyword(buf)) {
                    cat = CAT_KEYWORD;
                }

                token_init(&tok, buf, cat, tok_line, tok_col);
                tl_add(tokens, &tok);

                // One error for one grouped non-recognized token.
                if (cat == CAT_NONRECOGNIZED) {
                    report_nonrecognized(lg, tok_line, buf);
                }
                return 1;
            }

            // EOF reached with no pending token.
            if (state == ST_START && cls == CC_EOF) {
                return 0;
            }

            // Defensive fallback: consume one char as NONRECOGNIZED and continue.
            {
                char fallback[2];
                token_t tok;
                tok_line = cs_line(cs);
                tok_col  = cs_col(cs);
                ch = cs_get(cs);
                CNT_IO(cnt, 1);
                fallback[0] = (char)ch;
                fallback[1] = '\0';
                report_nonrecognized(lg, tok_line, fallback);
                token_init(&tok, fallback, CAT_NONRECOGNIZED, tok_line, tok_col);
                tl_add(tokens, &tok);
            }
            return 1;
        }

        // Skip whitespace while staying in START.
        if (state == ST_START && next == ST_START) {
            cs_get(cs);
            CNT_IO(cnt, 1);
            continue;
        }

        // Normal transition: consume one character.
        if (state == ST_START) {
            // Track source position of first token character.
            tok_line = cs_line(cs);
            tok_col = cs_col(cs);
        }

        ch = cs_get(cs);
        CNT_IO(cnt, 1);
        CNT_GEN(cnt, 1);
        add_char_to_lexeme(buf, &buf_len, ch);

        state = next;

        // Keep last accepting state for maximal munch.
        if (is_accepting(state)) {
            last_accept_state = state;
        }
    }
}

// Scanner loop until EOF.
int automata_scan(char_stream_t *cs, token_list_t *tokens, logger_t *lg,
                  counter_t *cnt) {
    while (scanner_next_token(cs, tokens, lg, cnt)) {
        // Continue scanning.
    }
    return 0;
}
