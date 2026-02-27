/*
 * =============================================================================
 * token_loader.c
 * Implementation of the .cscn token file reader.
 *
 * Reads tokens in <lexeme, CATEGORY> format, maps each to a terminal index
 * using the loaded language spec, skips unrecognised tokens, and appends an
 * EOF sentinel at the end of the list.
 *
 * Author: [Team]
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token_loader.h"

/* ---- Helpers ---- */

/* Trim leading and trailing whitespace from str in-place. */
static void trim_whitespace(char *str)
{
    int len = (int)strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) str[--len] = '\0';
    int start = 0;
    while (str[start] && isspace((unsigned char)str[start])) start++;
    if (start) memmove(str, str + start, len - start + 1);
}

/* Find the EOF terminal index in ls. Returns MATCH_NOTFOUND if none. */
static int find_eof_terminal(const lang_spec_t *ls)
{
    return ls_map_token(ls, NULL, NULL);
}

/* Grow the dynamic token array when it is full. Returns 0 or -1. */
static int ptl_grow(parser_token_list_t *ptl)
{
    int new_cap = ptl->capacity * PTL_GROWTH_FACTOR;
    parser_token_t *tmp = realloc(ptl->tokens,
                                  (size_t)new_cap * sizeof(parser_token_t));
    if (!tmp) return -1;
    ptl->tokens   = tmp;
    ptl->capacity = new_cap;
    return 0;
}

/* Append one token to the list. Returns 0 or -1. */
static int ptl_append(parser_token_list_t *ptl, const char *lexeme,
                      int term_index)
{
    if (ptl->count >= ptl->capacity && ptl_grow(ptl) != 0) return -1;
    parser_token_t *t = &ptl->tokens[ptl->count++];
    strncpy(t->lexeme, lexeme, PTL_MAX_LEXEME_LEN - 1);
    t->lexeme[PTL_MAX_LEXEME_LEN - 1] = '\0';
    t->term_index = term_index;
    return 0;
}

/* ---- Token parsing from a single <lexeme, CATEGORY> pair ---- */

/* Read characters into buf until stop_char or EOF. Returns chars read. */
static int read_until(FILE *f, char *buf, int maxlen, char stop_char)
{
    int i = 0, c;
    while ((c = fgetc(f)) != EOF && c != stop_char) {
        if (i < maxlen - 1) buf[i++] = (char)c;
    }
    buf[i] = '\0';
    return i;
}

/* Parse one <lexeme, CATEGORY> token from f.  Called after '<' was consumed.
 * Returns 1 on success, 0 on EOF/parse failure. */
static int parse_one_token(FILE *f, char *lexeme, char *category)
{
    char lex_buf[PTL_MAX_LEXEME_LEN];
    char cat_buf[PTL_MAX_LEXEME_LEN];

    /* Read lexeme up to ',' */
    if (read_until(f, lex_buf, PTL_MAX_LEXEME_LEN, TOK_DELIM_CHAR) == 0) return 0;
    trim_whitespace(lex_buf);

    /* Read category up to '>' */
    if (read_until(f, cat_buf, PTL_MAX_LEXEME_LEN, TOK_END_CHAR) == 0) return 0;
    trim_whitespace(cat_buf);

    strncpy(lexeme,   lex_buf, PTL_MAX_LEXEME_LEN - 1);
    lexeme[PTL_MAX_LEXEME_LEN - 1] = '\0';
    strncpy(category, cat_buf, PTL_MAX_LEXEME_LEN - 1);
    category[PTL_MAX_LEXEME_LEN - 1] = '\0';
    return 1;
}

/* ---- Public functions ---- */

/* Initialise an empty token list. */
void ptl_init(parser_token_list_t *ptl)
{
    ptl->tokens   = malloc((size_t)PTL_INIT_CAPACITY * sizeof(parser_token_t));
    if (!ptl->tokens) {
        fprintf(stderr, "ptl_init: out of memory\n");
        ptl->capacity = 0;
    } else {
        ptl->capacity = PTL_INIT_CAPACITY;
    }
    ptl->count = 0;
}

/* Free heap memory owned by ptl. */
void ptl_free(parser_token_list_t *ptl)
{
    free(ptl->tokens);
    ptl->tokens   = NULL;
    ptl->count    = 0;
    ptl->capacity = 0;
}

/* Load all tokens from a .cscn file.
 * Returns 0 on success, -1 if the file cannot be opened. */
int tok_load(parser_token_list_t *ptl, const lang_spec_t *ls,
             const char *filename)
{
    FILE *f = fopen(filename, TOK_FOPEN_READ_MODE);
    if (!f) {
        fprintf(stderr, "tok_load: cannot open '%s'\n", filename);
        return -1;
    }

    int c;
    char lexeme[PTL_MAX_LEXEME_LEN], category[PTL_MAX_LEXEME_LEN];

    while ((c = fgetc(f)) != EOF) {
        if (c != TOK_START_CHAR) continue;           /* Scan for opening '<' */
        if (!parse_one_token(f, lexeme, category)) continue;
        int idx = ls_map_token(ls, lexeme, category);
        if (idx == MATCH_NOTFOUND) continue; /* Token not in grammar */
#if TRACE_TOKEN_LOADER
        fprintf(stderr, "[TOKEN_LOADER] loaded '%s' cat='%s' idx=%d\n",
                lexeme, category, idx);
#endif
        ptl_append(ptl, lexeme, idx);
    }

    /* Append EOF sentinel */
    int eof_idx = find_eof_terminal(ls);
    if (eof_idx != MATCH_NOTFOUND) {
        ptl_append(ptl, EOF_LEXEME, eof_idx);
    }

    fclose(f);
    return 0;
}
