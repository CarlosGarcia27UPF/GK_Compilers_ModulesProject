/*
 * =============================================================================
 * lang_spec.c
 * Implementation of the language specification loader and query functions.
 *
 * Parses a .txt grammar file (see language1.txt for format) and populates
 * a lang_spec_t structure with terminals, non-terminals, rules, and the
 * ACTION / GOTO tables.
 *
 * Author: [Team]
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lang_spec.h"

/* ---- Internal helpers ---- */

/* Read the next non-blank, non-comment line from f into buf.
 * Returns buf on success, NULL at end-of-file. */
static char *read_line(FILE *f, char *buf, int size)
{
    while (fgets(buf, size, f)) {
        char *nl = strchr(buf, '\n');
        if (nl) *nl = '\0';
        char *cr = strchr(buf, '\r');
        if (cr) *cr = '\0';
        if (buf[0] != '\0' && buf[0] != LS_COMMENT_CHAR) return buf;
    }
    return NULL;
}

/* Parse "name LEXEME:val / CATEGORY:val / EOF" into terminal_t t. */
static void parse_terminal_line(const char *line, terminal_t *t)
{
    char spec[LS_MAX_NAME];
    spec[0] = '\0';
    sscanf(line, LS_SSCANF_NAME_FMT " " LS_SSCANF_NAME_FMT, t->name, spec);
    if (strncmp(spec, LS_LEXEME_PREFIX, sizeof(LS_LEXEME_PREFIX) - 1) == 0) {
        t->match_type = MATCH_LEXEME;
        strncpy(t->match_value, spec + sizeof(LS_LEXEME_PREFIX) - 1, LS_MAX_NAME - 1);
        t->match_value[LS_MAX_NAME - 1] = '\0';
    } else if (strncmp(spec, LS_CATEGORY_PREFIX, sizeof(LS_CATEGORY_PREFIX) - 1) == 0) {
        t->match_type = MATCH_CATEGORY;
        strncpy(t->match_value, spec + sizeof(LS_CATEGORY_PREFIX) - 1, LS_MAX_NAME - 1);
        t->match_value[LS_MAX_NAME - 1] = '\0';
    } else {
        t->match_type = MATCH_EOF;
        strncpy(t->match_value, MATCH_EOF_STRING, LS_MAX_NAME - 1);
        t->match_value[LS_MAX_NAME - 1] = '\0';
    }
}

/* Parse an ACTION cell string ("E", "S5", "R3", "A") into a. */
static void parse_action_cell(const char *cell, action_t *a)
{
    if (cell[0] == LS_ACTION_SHIFT_CHAR) {
        a->type  = ACTION_SHIFT;  a->value = atoi(cell + 1);
    } else if (cell[0] == LS_ACTION_REDUCE_CHAR) {
        a->type  = ACTION_REDUCE; a->value = atoi(cell + 1);
    } else if (cell[0] == LS_ACTION_ACCEPT_CHAR) {
        a->type  = ACTION_ACCEPT; a->value = 0;
    } else {
        a->type  = ACTION_ERROR;  a->value = 0;
    }
}

/* ---- Section loaders ---- */

/* Load num_terminals terminal lines. Returns 0 on success, -1 on error. */
static int ls_load_terminals(lang_spec_t *ls, FILE *f)
{
    char buf[LS_LINEBUF_SIZE];
    for (int i = 0; i < ls->dfa.num_terminals; i++) {
        if (!read_line(f, buf, LS_LINEBUF_SIZE)) return -1;
        parse_terminal_line(buf, &ls->terminals[i]);
    }
    return 0;
}

/* Load num_nonterminals non-terminal name lines. Returns 0 or -1. */
static int ls_load_nonterminals(lang_spec_t *ls, FILE *f)
{
    char buf[LS_LINEBUF_SIZE];
    for (int i = 0; i < ls->dfa.num_nonterminals; i++) {
        if (!read_line(f, buf, LS_LINEBUF_SIZE)) return -1;
        sscanf(buf, LS_SSCANF_NAME_FMT, ls->nonterminals[i].name);
    }
    return 0;
}

/* Load num_rules rule lines ("lhs_idx rhs_len desc"). Returns 0 or -1. */
static int ls_load_rules(lang_spec_t *ls, FILE *f)
{
    char buf[LS_LINEBUF_SIZE];
    for (int i = 0; i < ls->num_rules; i++) {
        if (!read_line(f, buf, LS_LINEBUF_SIZE)) return -1;
        rule_t *r = &ls->rules[i];
        char *ptr = buf;
        r->lhs_index = (int)strtol(ptr, &ptr, LS_DECIMAL_BASE);
        r->rhs_len   = (int)strtol(ptr, &ptr, LS_DECIMAL_BASE);
        while (*ptr == LS_SPACE_CHAR) ptr++;
        strncpy(r->desc, ptr, LS_MAX_DESC - 1);
        r->desc[LS_MAX_DESC - 1] = '\0';
    }
    return 0;
}

/* Parse one ACTION row from buf into the DFA's action_table[state]. */
static void ls_parse_action_row(lang_spec_t *ls, int state, char *buf)
{
    char tmp[LS_LINEBUF_SIZE];
    strncpy(tmp, buf, LS_LINEBUF_SIZE - 1);
    tmp[LS_LINEBUF_SIZE - 1] = '\0';
    char *tok = strtok(tmp, LS_WHITESPACE_DELIMS);
    for (int j = 0; j < ls->dfa.num_terminals && tok; j++) {
        action_t a;
        parse_action_cell(tok, &a);
        dfa_set_action(&ls->dfa, state, j, a);
        tok = strtok(NULL, LS_WHITESPACE_DELIMS);
    }
}

/* Load num_states ACTION rows. Returns 0 or -1. */
static int ls_load_action(lang_spec_t *ls, FILE *f)
{
    char buf[LS_LINEBUF_SIZE];
    for (int i = 0; i < ls->dfa.num_states; i++) {
        if (!read_line(f, buf, LS_LINEBUF_SIZE)) return -1;
        ls_parse_action_row(ls, i, buf);
    }
    return 0;
}

/* Parse one GOTO row from buf into the DFA's goto_table[state]. */
static void ls_parse_goto_row(lang_spec_t *ls, int state, char *buf)
{
    char tmp[LS_LINEBUF_SIZE];
    strncpy(tmp, buf, LS_LINEBUF_SIZE - 1);
    tmp[LS_LINEBUF_SIZE - 1] = '\0';
    char *tok = strtok(tmp, LS_WHITESPACE_DELIMS);
    for (int j = 0; j < ls->dfa.num_nonterminals && tok; j++) {
        dfa_set_goto(&ls->dfa, state, j, atoi(tok));
        tok = strtok(NULL, LS_WHITESPACE_DELIMS);
    }
}

/* Load num_states GOTO rows. Returns 0 or -1. */
static int ls_load_goto(lang_spec_t *ls, FILE *f)
{
    char buf[LS_LINEBUF_SIZE];
    for (int i = 0; i < ls->dfa.num_states; i++) {
        if (!read_line(f, buf, LS_LINEBUF_SIZE)) return -1;
        ls_parse_goto_row(ls, i, buf);
    }
    return 0;
}

/* ---- Public functions ---- */

/* Load a language spec from a grammar file. Returns 0 on success, -1 on error. */
int ls_load(lang_spec_t *ls, const char *filename)
{
    FILE *f = fopen(filename, LS_FOPEN_READ_MODE);
    if (!f) {
        fprintf(stderr, "ls_load: cannot open '%s'\n", filename);
        return -1;
    }
    char buf[LS_LINEBUF_SIZE];
    memset(ls, 0, sizeof(*ls));
    dfa_init(&ls->dfa);

    /* LANGUAGE <name> */
    read_line(f, buf, LS_LINEBUF_SIZE);
    sscanf(buf, "LANGUAGE " LS_SSCANF_NAME_FMT, ls->lang_name);

    /* TERMINALS <count> */
    read_line(f, buf, LS_LINEBUF_SIZE);
    sscanf(buf, "TERMINALS %d", &ls->dfa.num_terminals);
    ls_load_terminals(ls, f);

    /* NONTERMINALS <count> */
    read_line(f, buf, LS_LINEBUF_SIZE);
    sscanf(buf, "NONTERMINALS %d", &ls->dfa.num_nonterminals);
    ls_load_nonterminals(ls, f);

    /* RULES <count> */
    read_line(f, buf, LS_LINEBUF_SIZE);
    sscanf(buf, "RULES %d", &ls->num_rules);
    ls_load_rules(ls, f);

    /* STATES <count> */
    read_line(f, buf, LS_LINEBUF_SIZE);
    sscanf(buf, "STATES %d", &ls->dfa.num_states);

    read_line(f, buf, LS_LINEBUF_SIZE); /* "ACTION" header */
    ls_load_action(ls, f);

    read_line(f, buf, LS_LINEBUF_SIZE); /* "GOTO" header */
    ls_load_goto(ls, f);

    fclose(f);
#if TRACE_LANG_SPEC
    fprintf(stderr, "[LANG_SPEC] loaded '%s': %d terminals, %d non-terminals, %d rules, %d states\n",
            ls->lang_name, ls->dfa.num_terminals, ls->dfa.num_nonterminals,
            ls->num_rules, ls->dfa.num_states);
#endif
    return 0;
}

/* Zero out a lang_spec_t to free logical resources. */
void ls_free(lang_spec_t *ls)
{
    memset(ls, 0, sizeof(*ls));
}

/* Return the ACTION cell for (state, terminal index).
 * Delegates to dfa_get_action; returns ERROR entry on out-of-range inputs. */
action_t ls_get_action(const lang_spec_t *ls, int state, int term)
{
    return dfa_get_action(&ls->dfa, state, term);
}

/* Return the GOTO cell for (state, non-terminal index).
 * Delegates to dfa_get_goto; returns -1 on error. */
int ls_get_goto(const lang_spec_t *ls, int state, int nonterm)
{
    return dfa_get_goto(&ls->dfa, state, nonterm);
}

/* Find the terminal index that matches (lexeme, category).
 * Pass lexeme=NULL to locate the EOF terminal.
 * Returns MATCH_NOTFOUND if nothing matches. */
int ls_map_token(const lang_spec_t *ls, const char *lexeme,
                 const char *category)
{
    for (int i = 0; i < ls->dfa.num_terminals; i++) {
        const terminal_t *t = &ls->terminals[i];
        if (lexeme == NULL && t->match_type == MATCH_EOF)              return i;
        if (lexeme && t->match_type == MATCH_LEXEME
                   && strcmp(lexeme,   t->match_value) == 0)           return i;
        if (category && t->match_type == MATCH_CATEGORY
                     && strcmp(category, t->match_value) == 0)         return i;
    }
    return MATCH_NOTFOUND;
}
