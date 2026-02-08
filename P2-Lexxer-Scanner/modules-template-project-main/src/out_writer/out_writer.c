/*
 * -----------------------------------------------------------------------------
 * out_writer.c
 *
 * Output writer implementation. Builds the .cscn filename and writes
 * the formatted token list, respecting the OUTFORMAT setting.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "out_writer.h"
#include "../lang_spec/lang_spec.h"
#include <stdio.h>

// Internal helper to print one token.

// Builds "<input>scn". Example: example.c -> example.cscn.
void ow_build_output_filename(const char *input_filename, char *output_buf,
                              int buf_len) {
    int i = 0;

    if (input_filename == NULL || output_buf == NULL || buf_len <= 0) {
        return;
    }

    // Copy input filename.
    while (input_filename[i] != '\0' && i < buf_len - 1) {
        output_buf[i] = input_filename[i];
        i++;
    }
    // Append scn suffix.
    {
        const char *suffix = SCN_SUFFIX;
        int s = 0;
        while (suffix[s] != '\0' && i < buf_len - 1) {
            output_buf[i] = suffix[s];
            i++;
            s++;
        }
    }
    output_buf[i] = '\0';
}

// Builds "<input>dbgcnt". Example: example.c -> example.cdbgcnt.
void ow_build_count_filename(const char *input_filename, char *output_buf,
                             int buf_len) {
    int i = 0;

    if (input_filename == NULL || output_buf == NULL || buf_len <= 0) {
        return;
    }

    while (input_filename[i] != '\0' && i < buf_len - 1) {
        output_buf[i] = input_filename[i];
        i++;
    }
    {
        const char *suffix = DBGCNT_SUFFIX;
        int s = 0;
        while (suffix[s] != '\0' && i < buf_len - 1) {
            output_buf[i] = suffix[s];
            i++;
            s++;
        }
    }
    output_buf[i] = '\0';
}

// Writes token as <lexeme, CATEGORY>.
static void write_token_formatted(FILE *fp, const token_t *tok) {
    const char *cat_name = ls_get_category_name(tok->category);
    fprintf(fp, "%c%s%c %s%c", TOK_FMT_OPEN, tok->lexeme, TOK_FMT_SEP,
            cat_name, TOK_FMT_CLOSE);
}

// Writes complete token output file in configured mode.
int ow_write_token_file_mode(const token_list_t *tokens,
                             const char *output_filename, int append_mode) {
    FILE *fp;
    int i;
    int count;
    int current_line;
    int first_on_line;
    const token_t *tok;
    const char *open_mode = append_mode ? "a" : "w";

    if (tokens == NULL || output_filename == NULL) {
        return -1;
    }

    fp = fopen(output_filename, open_mode);
    if (fp == NULL) {
        return -1;
    }

    count = tl_count(tokens);
    if (count == 0) {
        fclose(fp);
        return 0;
    }

    current_line = -1;
    first_on_line = 1;

    for (i = 0; i < count; i++) {
        tok = tl_get(tokens, i);
        if (tok == NULL) {
            continue;
        }

        if (tok->line != current_line) {
            // Start a new output line for a new source line number.
            if (current_line != -1) {
                fprintf(fp, "\n");
#if OUTFORMAT == OUTFORMAT_DEBUG
                fprintf(fp, "\n");
#endif
            }
#if OUTFORMAT == OUTFORMAT_DEBUG
            fprintf(fp, "%d ", tok->line);
#endif
            current_line = tok->line;
            first_on_line = 1;
        }

        if (!first_on_line) {
            fprintf(fp, " ");
        }
        write_token_formatted(fp, tok);
        first_on_line = 0;
    }

    // Finish output line. DEBUG adds an extra separator line.
    fprintf(fp, "\n");
#if OUTFORMAT == OUTFORMAT_DEBUG
    fprintf(fp, "\n");
#endif

    fclose(fp);
    return 0;
}

int ow_write_token_file(const token_list_t *tokens, const char *output_filename) {
    return ow_write_token_file_mode(tokens, output_filename, 0);
}
