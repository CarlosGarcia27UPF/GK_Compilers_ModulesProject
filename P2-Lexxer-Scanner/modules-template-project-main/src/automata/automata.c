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

// Transition matrix T[state][class] -> next_state
// Uses GNU C range designators: [0 ... CC_COUNT-1] = ST_STOP

static const scan_state_t T[ST_COUNT][CC_COUNT] = {

    // -------------------------
    // START
    // -------------------------
    [ST_START] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_SPACE]   = ST_START,
        [CC_NEWLINE] = ST_START,
        [CC_EOF]     = ST_STOP,

        [CC_DIGIT]   = ST_IN_NUMBER,
        [CC_QUOTE]   = ST_IN_LITERAL,

        [CC_OPERATOR] = ST_ACCEPT_OP,
        [CC_SPECIAL]  = ST_ACCEPT_SC,

        // keyword starting letters
        [CC_i] = ST_KW_I,
        [CC_e] = ST_KW_E,
        [CC_w] = ST_KW_W,
        [CC_r] = ST_KW_R,
        [CC_c] = ST_KW_C,
        [CC_v] = ST_KW_V,

        // other letters start identifier
        [CC_f] = ST_IN_IDENT,
        [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT,
        [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT,
        [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT,
        [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,

        // unknown starts nonrecognized group
        [CC_OTHER] = ST_IN_NONREC,
    },

    // -------------------------
    // NUMBER
    // -------------------------
    [ST_IN_NUMBER] = {
        [0 ... CC_COUNT-1] = ST_STOP,
        [CC_DIGIT] = ST_IN_NUMBER
    },

    // -------------------------
    // IDENTIFIER
    // -------------------------
    [ST_IN_IDENT] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        // any letter class continues identifier
        [CC_i] = ST_IN_IDENT,
        [CC_e] = ST_IN_IDENT,
        [CC_w] = ST_IN_IDENT,
        [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT,
        [CC_v] = ST_IN_IDENT,

        [CC_f] = ST_IN_IDENT,
        [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT,
        [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT,
        [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT,
        [CC_d] = ST_IN_IDENT,

        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    // -------------------------
    // LITERAL (double-quoted)
    // -------------------------
    [ST_IN_LITERAL] = {
        [0 ... CC_COUNT-1] = ST_IN_LITERAL,

        [CC_QUOTE]   = ST_LIT_END,
        [CC_NEWLINE] = ST_ERROR,
        [CC_EOF]     = ST_ERROR
    },

    [ST_LIT_END] = {
        [0 ... CC_COUNT-1] = ST_STOP
    },

    // -------------------------
    // NONRECOGNIZED GROUP
    // -------------------------
    [ST_IN_NONREC] = {
        [0 ... CC_COUNT-1] = ST_STOP,
        [CC_OTHER] = ST_IN_NONREC
    },

    // -------------------------
    // SINGLE-CHAR TOKENS
    // -------------------------
    [ST_ACCEPT_OP] = {
        [0 ... CC_COUNT-1] = ST_STOP
    },

    [ST_ACCEPT_SC] = {
        [0 ... CC_COUNT-1] = ST_STOP
    },

    // -------------------------
    // KEYWORDS: if / int
    // -------------------------
    [ST_KW_I] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_f] = ST_KW_IF,
        [CC_n] = ST_KW_IN,

        // otherwise, if it continues as identifier => fall into ident
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT,
        [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_IF] = {
        [0 ... CC_COUNT-1] = ST_STOP, // accept "if" on delimiter/operator/special/space/newline/EOF

        // if followed by letter/digit => identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_IN] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_t] = ST_KW_INT,

        // otherwise, identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_INT] = {
        [0 ... CC_COUNT-1] = ST_STOP, // accept "int"

        // if followed by letter/digit => identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    // -------------------------
    // KEYWORDS: else
    // -------------------------
    [ST_KW_E] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_l] = ST_KW_EL,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT,
        [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_EL] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_s] = ST_KW_ELS,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_ELS] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_e] = ST_KW_ELSE,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_ELSE] = {
        [0 ... CC_COUNT-1] = ST_STOP, // accept "else"

        // if followed by letter/digit => identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    // -------------------------
    // KEYWORDS: while
    // -------------------------
    [ST_KW_W] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_h] = ST_KW_WH,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_WH] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_i] = ST_KW_WHI,

        // otherwise identifier
        [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT, [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT,
        [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_WHI] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_l] = ST_KW_WHIL,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT,
        [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_WHIL] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_e] = ST_KW_WHILE,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT, [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT,
        [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_WHILE] = {
        [0 ... CC_COUNT-1] = ST_STOP, // accept "while"

        // if followed by letter/digit => identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    // -------------------------
    // KEYWORDS: return
    // -------------------------
    [ST_KW_R] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_e] = ST_KW_RE,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT, [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT,
        [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_RE] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_t] = ST_KW_RET,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_RET] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_u] = ST_KW_RETU,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_RETU] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_r] = ST_KW_RETUR,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT,
        [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_RETUR] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_n] = ST_KW_RETURN,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_RETURN] = {
        [0 ... CC_COUNT-1] = ST_STOP, // accept "return"

        // if followed by letter/digit => identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    // -------------------------
    // KEYWORDS: char
    // -------------------------
    [ST_KW_C] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_h] = ST_KW_CH,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_CH] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_a] = ST_KW_CHA,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT,
        [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_CHA] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_r] = ST_KW_CHAR,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT,
        [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_CHAR] = {
        [0 ... CC_COUNT-1] = ST_STOP, // accept "char"

        // if followed by letter/digit => identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    // -------------------------
    // KEYWORDS: void
    // -------------------------
    [ST_KW_V] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_o] = ST_KW_VO,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_VO] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_i] = ST_KW_VOI,

        // otherwise identifier
        [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT, [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT,
        [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT, [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT,
        [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT, [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT,
        [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_VOI] = {
        [0 ... CC_COUNT-1] = ST_STOP,

        [CC_d] = ST_KW_VOID,

        // otherwise identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    [ST_KW_VOID] = {
        [0 ... CC_COUNT-1] = ST_STOP, // accept "void"

        // if followed by letter/digit => identifier
        [CC_i] = ST_IN_IDENT, [CC_e] = ST_IN_IDENT, [CC_w] = ST_IN_IDENT, [CC_r] = ST_IN_IDENT,
        [CC_c] = ST_IN_IDENT, [CC_v] = ST_IN_IDENT, [CC_f] = ST_IN_IDENT, [CC_l] = ST_IN_IDENT,
        [CC_s] = ST_IN_IDENT, [CC_h] = ST_IN_IDENT, [CC_a] = ST_IN_IDENT, [CC_t] = ST_IN_IDENT,
        [CC_n] = ST_IN_IDENT, [CC_u] = ST_IN_IDENT, [CC_o] = ST_IN_IDENT, [CC_d] = ST_IN_IDENT,
        [CC_LETTER] = ST_IN_IDENT,
        [CC_DIGIT]  = ST_IN_IDENT
    },

    // -------------------------
    // ERROR + STOP
    // -------------------------
    [ST_ERROR] = {
        [0 ... CC_COUNT-1] = ST_STOP
    },

    [ST_STOP] = {
        [0 ... CC_COUNT-1] = ST_STOP
    }
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
        case ST_KW_IF:
        case ST_KW_INT:
        case ST_KW_ELSE:
        case ST_KW_WHILE:
        case ST_KW_RETURN:
        case ST_KW_CHAR:
        case ST_KW_VOID:
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

        case ST_KW_IF:
        case ST_KW_INT:
        case ST_KW_ELSE:
        case ST_KW_WHILE:
        case ST_KW_RETURN:
        case ST_KW_CHAR:
        case ST_KW_VOID:
            return CAT_KEYWORD;

        default:            return CAT_NONRECOGNIZED;
    }
}

// Maps one character to a DFA class.
char_class_t classify_char(int ch) {
    if (ch == EOF) return CC_EOF;

    if (ch == '\n') return CC_NEWLINE;
    if (ch == ' ' || ch == '\t' || ch == '\r') return CC_SPACE;

    if (ls_is_digit((char)ch)) return CC_DIGIT;
    if (ls_is_quote((char)ch)) return CC_QUOTE;
    if (ls_is_operator((char)ch)) return CC_OPERATOR;
    if (ls_is_special_char((char)ch)) return CC_SPECIAL;

    switch (ch) {
        case 'i': return CC_i;
        case 'e': return CC_e;
        case 'w': return CC_w;
        case 'r': return CC_r;
        case 'c': return CC_c;
        case 'v': return CC_v;

        case 'f': return CC_f;
        case 'l': return CC_l;
        case 's': return CC_s;
        case 'h': return CC_h;
        case 'a': return CC_a;
        case 't': return CC_t;
        case 'n': return CC_n;
        case 'u': return CC_u;
        case 'o': return CC_o;
        case 'd': return CC_d;
    }

    if (ls_is_letter((char)ch)) return CC_LETTER;

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
