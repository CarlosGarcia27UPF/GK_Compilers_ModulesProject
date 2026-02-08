/**
 * @file char_stream.c
 * @brief Implementation of Character Stream Module
 * 
 * Cursor-only input handling. Never classifies characters.
 */

#include "char_stream.h"
#include <string.h>

/* ======================= Initialization ======================= */

void cs_init_string(CharStream* cs, const char* source, int length) {
    if (cs == NULL) return;
    
    cs->source = source;
    cs->file = NULL;
    cs->position = 0;
    cs->length = length;
    cs->line = 1;
    cs->column = 1;
    cs->is_file = false;
    cs->lookahead_char = '\0';
    cs->has_lookahead = false;
}

void cs_init_file(CharStream* cs, FILE* file) {
    if (cs == NULL) return;
    
    cs->source = NULL;
    cs->file = file;
    cs->position = 0;
    cs->length = -1;  /* Unknown for file */
    cs->line = 1;
    cs->column = 1;
    cs->is_file = true;
    cs->lookahead_char = '\0';
    cs->has_lookahead = false;
}

void cs_close(CharStream* cs) {
    if (cs == NULL) return;
    
    /* Note: We don't close the file here - caller's responsibility */
    cs->source = NULL;
    cs->file = NULL;
    cs->position = 0;
    cs->length = 0;
}

/* ======================= Internal Helper ======================= */

/**
 * @brief Read next character from source (internal)
 */
static char cs_read_next(CharStream* cs) {
    if (cs == NULL) return '\0';
    
    if (cs->is_file) {
        /* File input */
        if (cs->file == NULL) return '\0';
        int c = fgetc(cs->file);
        if (c == EOF) return '\0';
        return (char)c;
    } else {
        /* String input */
        if (cs->source == NULL || cs->position >= cs->length) {
            return '\0';
        }
        return cs->source[cs->position];
    }
}

/* ======================= Core Functions ======================= */

char cs_peek(CharStream* cs) {
    if (cs == NULL) return '\0';
    
    /* Use cached lookahead if available */
    if (cs->has_lookahead) {
        return cs->lookahead_char;
    }
    
    /* Read and cache the next character */
    if (cs->is_file) {
        if (cs->file == NULL) return '\0';
        int c = fgetc(cs->file);
        if (c == EOF) {
            cs->lookahead_char = '\0';
        } else {
            cs->lookahead_char = (char)c;
            /* Put it back for later consumption */
            ungetc(c, cs->file);
        }
    } else {
        if (cs->source == NULL || cs->position >= cs->length) {
            cs->lookahead_char = '\0';
        } else {
            cs->lookahead_char = cs->source[cs->position];
        }
    }
    
    cs->has_lookahead = true;
    return cs->lookahead_char;
}

char cs_get(CharStream* cs) {
    if (cs == NULL) return '\0';
    
    char c;
    
    /* Use cached lookahead if available */
    if (cs->has_lookahead) {
        c = cs->lookahead_char;
        cs->has_lookahead = false;
        
        /* For file, we already ungot it, so read again */
        if (cs->is_file && cs->file != NULL) {
            fgetc(cs->file);  /* Consume the character */
        }
    } else {
        c = cs_read_next(cs);
    }
    
    /* Update position for string input */
    if (!cs->is_file) {
        cs->position++;
    }
    
    /* Update line and column tracking */
    if (c == '\n') {
        cs->line++;
        cs->column = 1;
    } else if (c != '\0') {
        cs->column++;
    }
    
    return c;
}

bool cs_is_eof(CharStream* cs) {
    if (cs == NULL) return true;
    return cs_peek(cs) == '\0';
}

/* ======================= Position Tracking ======================= */

int cs_get_line(const CharStream* cs) {
    if (cs == NULL) return 0;
    return cs->line;
}

int cs_get_column(const CharStream* cs) {
    if (cs == NULL) return 0;
    return cs->column;
}

int cs_get_position(const CharStream* cs) {
    if (cs == NULL) return 0;
    return cs->position;
}
