/*
 * =============================================================================
 * lang_spec.h
 * Language specification data structures and function prototypes.
 *
 * Provides types for representing an SRA grammar: terminal and non-terminal
 * symbols, production rules, and a DFA (action/goto tables).  Functions load
 * a language spec from a text file and query it at parse time.
 *
 * The automaton tables (ACTION and GOTO) are encapsulated in the embedded
 * dfa_t field; use the dfa_* functions in dfa.h to access them directly, or
 * use the ls_get_action / ls_get_goto wrappers provided here.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef LANG_SPEC_H
#define LANG_SPEC_H

#include <stdio.h>
#include "../dfa/dfa.h"

/* ---- Trace flag ---- */
#define TRACE_LANG_SPEC 0  /* Set to 1 to enable lang_spec debug trace output */

/* ---- Capacity limits ---- */
#define LS_MAX_NAME      64   /* Max chars in a symbol name        */
#define LS_MAX_DESC     128   /* Max chars in a rule description   */
#define LS_MAX_RULES     64   /* Max number of production rules    */
#define LS_LINEBUF_SIZE 1024  /* Line-buffer size for file reading */

/* ---- Grammar file keyword prefixes ---- */
#define LS_LEXEME_PREFIX     "LEXEME:"    /* Prefix for lexeme-match terminal spec in grammar file  */
#define LS_CATEGORY_PREFIX   "CATEGORY:"  /* Prefix for category-match terminal spec in grammar file */
#define LS_FOPEN_READ_MODE   "r"          /* fopen mode for reading grammar files                   */

/* ---- Grammar file parsing helpers ---- */
#define LS_COMMENT_CHAR      '#'   /* Lines starting with this char are comments in grammar files   */
#define LS_ACTION_SHIFT_CHAR 'S'   /* Prefix char for SHIFT entries in the ACTION table (e.g. S5)   */
#define LS_ACTION_REDUCE_CHAR 'R'  /* Prefix char for REDUCE entries in the ACTION table (e.g. R3)  */
#define LS_ACTION_ACCEPT_CHAR 'A'  /* Prefix char for ACCEPT entries in the ACTION table            */
#define LS_WHITESPACE_DELIMS " \t" /* Whitespace token delimiters used when parsing table rows      */
#define LS_SPACE_CHAR        ' '   /* Space character used to skip blanks in rule lines             */
#define LS_DECIMAL_BASE      10    /* Numeric base for strtol when parsing integer values            */
/* sscanf format for reading a symbol name; width = LS_MAX_NAME - 1 = 63 */
#define LS_SSCANF_NAME_FMT   "%63s" /* Format specifier for bounded string read into LS_MAX_NAME buffer */

/* ---- Token match types (how a terminal matches an input token) ---- */
#define MATCH_LEXEME      0   /* Compare against the token's lexeme string  */
#define MATCH_CATEGORY    1   /* Compare against the token's category tag   */
#define MATCH_EOF         2   /* Matches the end-of-input sentinel          */
#define MATCH_NOTFOUND   -1   /* No terminal found for this token           */
#define MATCH_EOF_STRING "EOF" /* Keyword in grammar file for the EOF terminal */

/* ---- Data structures ---- */

/* One terminal symbol definition */
typedef struct {
    char name[LS_MAX_NAME];         /* Display name (e.g., "NUM", "+")       */
    int  match_type;                /* MATCH_LEXEME / MATCH_CATEGORY / MATCH_EOF */
    char match_value[LS_MAX_NAME];  /* Value to compare (lexeme or category)  */
} terminal_t;

/* One non-terminal symbol definition */
typedef struct {
    char name[LS_MAX_NAME];         /* Display name (e.g., "e", "t")         */
} nonterminal_t;

/* One production rule */
typedef struct {
    int  lhs_index;                 /* Index into nonterminals array          */
    int  rhs_len;                   /* Number of symbols on the RHS           */
    char desc[LS_MAX_DESC];         /* Description string (e.g., "e->e+t")    */
} rule_t;

/* Complete language specification.
 * The automaton tables are stored inside the embedded dfa field. */
typedef struct {
    char          lang_name[LS_MAX_NAME];           /* Language name                    */
    terminal_t    terminals[DFA_MAX_TERMINALS];     /* Terminal symbol definitions      */
    nonterminal_t nonterminals[DFA_MAX_NONTERMINALS]; /* Non-terminal symbol definitions */
    rule_t        rules[LS_MAX_RULES];              /* Production rules                 */
    int           num_rules;                        /* Count of production rules        */
    dfa_t         dfa;                              /* Embedded DFA (ACTION/GOTO tables) */
} lang_spec_t;

/* ---- Function prototypes ---- */

/* Load a language spec from a text file. Returns 0 on success, -1 on error. */
int      ls_load(lang_spec_t *ls, const char *filename);

/* Release resources held by a lang_spec_t (currently zeroes it out). */
void     ls_free(lang_spec_t *ls);

/* Return the ACTION table entry for (state, terminal index). */
action_t ls_get_action(const lang_spec_t *ls, int state, int term);

/* Return the GOTO table entry for (state, non-terminal index). */
int      ls_get_goto(const lang_spec_t *ls, int state, int nonterm);

/* Find the terminal index matching (lexeme, category).
 * Pass lexeme=NULL to search for the EOF terminal.
 * Returns MATCH_NOTFOUND if no terminal matches. */
int      ls_map_token(const lang_spec_t *ls, const char *lexeme,
                      const char *category);

#endif /* LANG_SPEC_H */
