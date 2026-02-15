/*
 * -----------------------------------------------------------------------------
 * lang_spec.h
 *
 * Language specification constants for the scanner.
 * All language-dependent information is concentrated here so that
 * the rest of the modules remain language-independent.
 *
 * Supported language (subset of C):
 *   - Keywords/types: if, else, while, return, int, char, void
 *   - Operators: = > + *
 *   - Special characters: ( ) ; { } [ ] ,
 *   - Numbers: integers [0-9]+
 *   - Identifiers: [A-Za-z][A-Za-z0-9]*  (not a keyword)
 *   - Literals: "..." (double-quoted strings)
 *   - Non-recognized: any unsupported lexeme
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef LANG_SPEC_H
#define LANG_SPEC_H

// Token category enumeration.
typedef enum {
    CAT_NUMBER       = 0,
    CAT_IDENTIFIER   = 1,
    CAT_KEYWORD      = 2,
    CAT_LITERAL      = 3,
    CAT_OPERATOR     = 4,
    CAT_SPECIALCHAR  = 5,
    CAT_NONRECOGNIZED = 6,
    CAT_COUNT        = 7   // Total number of categories.
} token_category_t;

// Category name strings.
#define CAT_NAME_NUMBER        "CAT_NUMBER"
#define CAT_NAME_IDENTIFIER    "CAT_IDENTIFIER"
#define CAT_NAME_KEYWORD       "CAT_KEYWORD"
#define CAT_NAME_LITERAL       "CAT_LITERAL"
#define CAT_NAME_OPERATOR      "CAT_OPERATOR"
#define CAT_NAME_SPECIALCHAR   "CAT_SPECIALCHAR"
#define CAT_NAME_NONRECOGNIZED "CAT_NONRECOGNIZED"

// Keywords.
#define NUM_KEYWORDS 7

#define KW_IF     "if"
#define KW_ELSE   "else"
#define KW_WHILE  "while"
#define KW_RETURN "return"
#define KW_INT    "int"
#define KW_CHAR   "char"
#define KW_VOID   "void"

// Operators.
#define OP_ASSIGN '='
#define OP_GT     '>'
#define OP_PLUS   '+'
#define OP_STAR   '*'
#define NUM_OPERATORS 4

// Special characters.
#define SC_LPAREN    '('
#define SC_RPAREN    ')'
#define SC_SEMICOLON ';'
#define SC_LBRACE    '{'
#define SC_RBRACE    '}'
#define SC_LBRACKET  '['
#define SC_RBRACKET  ']'
#define SC_COMMA     ','
#define NUM_SPECIALS 8

// Literal delimiters.
#define LIT_QUOTE '"'

// Whitespace delimiters.
#define WS_SPACE  ' '
#define WS_TAB    '\t'
#define WS_CR     '\r'
#define WS_NL     '\n'

// Lowercase letters used in DFA character-class routing.
#define CH_I_LOWER 'i'
#define CH_E_LOWER 'e'
#define CH_W_LOWER 'w'
#define CH_R_LOWER 'r'
#define CH_C_LOWER 'c'
#define CH_V_LOWER 'v'
#define CH_F_LOWER 'f'
#define CH_L_LOWER 'l'
#define CH_S_LOWER 's'
#define CH_H_LOWER 'h'
#define CH_A_LOWER 'a'
#define CH_T_LOWER 't'
#define CH_N_LOWER 'n'
#define CH_U_LOWER 'u'
#define CH_O_LOWER 'o'
#define CH_D_LOWER 'd'

// Max lexeme length.
#define MAX_LEXEME_LEN 1024

// Scanner output suffix.
#define SCN_SUFFIX "scn"

// Debug count output suffix.
#define DBGCNT_SUFFIX "dbgcnt"

// Helper declarations used by the scanner automata.

// Returns 1 for operators.
int ls_is_operator(char ch);

// Returns 1 for special characters.
int ls_is_special_char(char ch);

// Returns 1 for letters.
int ls_is_letter(char ch);

// Returns 1 for digits.
int ls_is_digit(char ch);

// Returns 1 for quote delimiter.
int ls_is_quote(char ch);

#endif /* LANG_SPEC_H */
