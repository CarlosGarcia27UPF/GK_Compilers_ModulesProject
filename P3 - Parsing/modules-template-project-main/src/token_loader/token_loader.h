/*
 * =============================================================================
 * token_loader.h
 * Reads .cscn scanner output files and builds a flat token list for the parser.
 *
 * .cscn format: <lexeme, CATEGORY>  tokens on one or more lines.
 * Each token is mapped to a terminal index from the language spec; tokens
 * with no matching terminal (e.g. whitespace categories) are silently skipped.
 * An EOF sentinel is appended automatically.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef TOKEN_LOADER_H
#define TOKEN_LOADER_H

#include "../lang_spec/lang_spec.h"

/* ---- Trace flag ---- */
#define TRACE_TOKEN_LOADER 0  /* Set to 1 to enable token_loader debug trace output */

/* ---- Constants ---- */
#define PTL_MAX_LEXEME_LEN  128  /* Max characters in one lexeme string  */
#define PTL_INIT_CAPACITY    32  /* Initial dynamic-array capacity        */
#define PTL_GROWTH_FACTOR     2  /* Multiplicative factor when growing the token array */
#define EOF_LEXEME          "$"  /* Lexeme string appended for the EOF sentinel token */
#define TOK_FOPEN_READ_MODE "r"  /* fopen mode for reading .cscn token files          */

/* ---- .cscn token format characters ---- */
#define TOK_START_CHAR   '<'  /* Opening delimiter of each token in .cscn file (e.g. <3, CAT>)  */
#define TOK_DELIM_CHAR   ','  /* Separator between lexeme and category fields in a token         */
#define TOK_END_CHAR     '>'  /* Closing delimiter of each token in .cscn file                  */

/* ---- Data structures ---- */

/* One parsed input token ready for the SRA engine */
typedef struct {
    char lexeme[PTL_MAX_LEXEME_LEN]; /* Original lexeme string              */
    int  term_index;                 /* Index into lang_spec terminals array */
} parser_token_t;

/* Dynamic array of parser_token_t */
typedef struct {
    parser_token_t *tokens;   /* Heap-allocated array of tokens  */
    int             count;    /* Number of valid tokens          */
    int             capacity; /* Current allocated capacity      */
} parser_token_list_t;

/* ---- Function prototypes ---- */

/* Initialise an empty token list (allocates initial storage). */
void ptl_init(parser_token_list_t *ptl);

/* Release all memory owned by the token list. */
void ptl_free(parser_token_list_t *ptl);

/* Load tokens from a .cscn file into ptl, using ls to map each token to a
 * terminal index.  Returns 0 on success, -1 on error. */
int  tok_load(parser_token_list_t *ptl, const lang_spec_t *ls,
              const char *filename);

#endif /* TOKEN_LOADER_H */
