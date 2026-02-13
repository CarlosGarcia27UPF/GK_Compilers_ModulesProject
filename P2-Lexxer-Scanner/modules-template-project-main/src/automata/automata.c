/*
 * -----------------------------------------------------------------------------
 * automata.c
 *
 * Scanner engine using parallel automata execution.
 * All 6 automata (NUMBER, IDENTIFIER, LITERAL, OPERATOR, SPECIALCHAR, 
 * NONRECOGNIZED) run simultaneously on the input stream. The automaton
 * with the longest accepting match wins (maximal munch principle).
 *
 * Design:
 *   - All automata execute in parallel (no pre-selection)
 *   - Longest match wins
 *   - Character-by-character lookahead with state tracking
 *   - Keywords are reclassified after identifier acceptance
 *   - Whitespace is consumed before token recognition
 *   - Unterminated literals emit error + NONRECOGNIZED token
 *   - Grouped non-recognized chars emit one error per group
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "automata.h"
#include "../lang_spec/lang_spec.h"

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Maps one character to a character class for automata logic.
static char_class_t classify_char(int ch) {
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
// PARALLEL AUTOMATA DISPATCHER (runs all automata, picks winner)
// ============================================================================

// Automata state tracking for parallel execution
typedef struct {
    int active;         // 1 = still running, 0 = dead/rejected
    int accepts;        // 1 = currently in accepting state
    int length;         // characters consumed so far
    token_category_t category;
} automata_state_t;

// Main scanner dispatcher: runs ALL automata in parallel, picks longest match
static int scanner_next_token(char_stream_t *cs, token_list_t *tokens,
                              logger_t *lg, counter_t *cnt) {
    int ch;
    char_class_t cls;
    char lexeme_buf[MAX_LEXEME_LEN];
    int buf_len = 0;
    int tok_line, tok_col;
    
    // Initialize all automata as active
    automata_state_t automata[6];
    automata[0] = (automata_state_t){1, 0, 0, CAT_NUMBER};        // Number
    automata[1] = (automata_state_t){1, 0, 0, CAT_IDENTIFIER};    // Identifier
    automata[2] = (automata_state_t){1, 0, 0, CAT_LITERAL};       // Literal
    automata[3] = (automata_state_t){1, 0, 0, CAT_OPERATOR};      // Operator
    automata[4] = (automata_state_t){1, 0, 0, CAT_SPECIALCHAR};   // Special
    automata[5] = (automata_state_t){1, 0, 0, CAT_NONRECOGNIZED}; // Nonrecognized
    
    int literal_state = 0; // 0 = not started, 1 = inside, 2 = closed

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

    // Save position for token
    tok_line = cs_line(cs);
    tok_col = cs_col(cs);
    
    lexeme_buf[0] = '\0';
    
    // Run all automata in parallel on the input stream
    int any_active = 1;
    int recalc_any_active;
    
    while (any_active && buf_len < MAX_LEXEME_LEN - 1) {
        ch = cs_peek(cs);
        CNT_COMP(cnt, 1);
        
        if (ch == CS_EOF) {
            cls = CC_EOF;
        } else {
            cls = classify_char(ch);
            CNT_COMP(cnt, 1);
        }
        
        recalc_any_active = 0;
        
        // ========== NUMBER AUTOMATA ==========
        // Only accepts digits
        if (automata[0].active) {
            if (cls == CC_DIGIT) {
                automata[0].accepts = 1;
                automata[0].active = 1;
            } else {
                automata[0].active = 0;  // Dies on first non-digit
            }
            if (automata[0].active) recalc_any_active = 1;
        }
        
        // ========== IDENTIFIER AUTOMATA ==========
        // letter then (letter|digit)*
        if (automata[1].active) {
            if (buf_len == 0 && cls == CC_LETTER) {
                automata[1].accepts = 1;
                automata[1].active = 1;
            } else if (buf_len > 0 && (cls == CC_LETTER || cls == CC_DIGIT)) {
                automata[1].accepts = 1;
                automata[1].active = 1;
            } else {
                automata[1].active = 0;
            }
            if (automata[1].active) recalc_any_active = 1;
        }
        
        // ========== LITERAL AUTOMATA ==========
        // "..."
        if (automata[2].active) {
            if (buf_len == 0 && cls == CC_QUOTE) {
                // Opening quote
                literal_state = 1;
                automata[2].accepts = 0;  // Not accepting until close quote
                automata[2].active = 1;
            } else if (literal_state == 1 && cls == CC_QUOTE) {
                // Closing quote
                literal_state = 2;
                automata[2].accepts = 1;
                automata[2].active = 1;   // Will stop after this char
            } else if (literal_state == 1 && cls != CC_NEWLINE && cls != CC_EOF) {
                // Content inside literal
                automata[2].accepts = 0;
                automata[2].active = 1;
            } else if (literal_state == 1 && (cls == CC_NEWLINE || cls == CC_EOF)) {
                // Unterminated literal (hit newline or EOF)
                automata[2].active = 0;
                automata[2].accepts = 1;
                automata[2].category = CAT_NONRECOGNIZED;
            } else {
                automata[2].active = 0;
            }
            if (automata[2].active) recalc_any_active = 1;
        }
        
        // ========== OPERATOR AUTOMATA ==========
        // Single character operator
        if (automata[3].active) {
            if (buf_len == 0 && cls == CC_OPERATOR) {
                automata[3].accepts = 1;
                automata[3].active = 1;  // Dies after 1 char
            } else {
                automata[3].active = 0;
            }
            if (automata[3].active) recalc_any_active = 1;
        }
        
        // ========== SPECIALCHAR AUTOMATA ==========
        // Single special character
        if (automata[4].active) {
            if (buf_len == 0 && cls == CC_SPECIAL) {
                automata[4].accepts = 1;
                automata[4].active = 1;  // Dies after 1 char
            } else {
                automata[4].active = 0;
            }
            if (automata[4].active) recalc_any_active = 1;
        }
        
        // ========== NONRECOGNIZED AUTOMATA ==========
        // CC_OTHER characters grouped together
        if (automata[5].active) {
            if (cls == CC_OTHER) {
                automata[5].accepts = 1;
                automata[5].active = 1;
            } else {
                automata[5].active = 0;
            }
            if (automata[5].active) recalc_any_active = 1;
        }
        
        // Update any_active for next iteration
        any_active = recalc_any_active;
        
        // If any automata are still active, consume the character
        if (any_active) {
            ch = cs_get(cs);  // Actually consume from stream
            CNT_IO(cnt, 1);
            CNT_GEN(cnt, 1);
            lexeme_buf[buf_len] = (char)ch;
            buf_len++;
            lexeme_buf[buf_len] = '\0';
            
            // Update all active automata lengths
            for (int i = 0; i < 6; i++) {
                if (automata[i].active) {
                    automata[i].length = buf_len;
                }
            }
            
            // Single-char automata stop after 1 char
            if (automata[3].length == 1) automata[3].active = 0;  // operator
            if (automata[4].length == 1) automata[4].active = 0;  // specialchar
            
            // Literal stops after closing quote
            if (literal_state == 2) {
                automata[2].active = 0;
            }
        }
    }
    
    // PICK THE WINNER: longest accepting automata
    int winner_idx = -1;
    int max_length = 0;
    
    for (int i = 0; i < 6; i++) {
        if (automata[i].accepts && automata[i].length > max_length) {
            max_length = automata[i].length;
            winner_idx = i;
        }
    }
    
    // Fallback: if nothing accepted, treat first char as nonrecognized
    if (winner_idx == -1) {
        if (buf_len > 0) {
            winner_idx = 5;  // Use nonrecognized for first char
            automata[5].accepts = 1;
            automata[5].length = 1;
            max_length = 1;
        } else {
            return 0;  // EOF
        }
    }
    
    // Truncate lexeme to winner's actual length
    lexeme_buf[max_length] = '\0';
    
    // Determine final category (check for keywords if identifier won)
    token_category_t final_category = automata[winner_idx].category;
    if (final_category == CAT_IDENTIFIER) {
        if (ls_is_keyword(lexeme_buf)) {
            final_category = CAT_KEYWORD;
        }
    }
    
    // Handle error reporting
    if (final_category == CAT_NONRECOGNIZED) {
        if (lexeme_buf[0] == '"') {
            report_unterminated_literal(lg, tok_line, lexeme_buf);
        } else {
            report_nonrecognized(lg, tok_line, lexeme_buf);
        }
    }
    
    // Emit the winning token
    token_t tok;
    token_init(&tok, lexeme_buf, final_category, tok_line, tok_col);
    tl_add(tokens, &tok);
    
    return 1;
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
