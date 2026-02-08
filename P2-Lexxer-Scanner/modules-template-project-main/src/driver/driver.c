/**
 * @file driver.c
 * @brief Driver Module Implementation (Main / Orchestrator)
 * 
 * Wrapper only. Coordinates all modules.
 */

#include "driver.h"
#include "../char_stream/char_stream.h"
#include "../automata/automata.h"
#include "../token/token.h"
#include "../token_list/token_list.h"
#include "../lang_spec/lang_spec.h"
#include "../logger/logger.h"
#include "../error/error.h"
#include "../counter/counter.h"
#include "../out_writer/out_writer.h"
#include <stdlib.h>
#include <string.h>

/* ======================= Driver Implementation ======================= */

int driver_run_string(const char* source, FILE* output) {
    if (source == NULL) {
        fprintf(stderr, "Error: NULL source\n");
        return 1;
    }
    
    /* Initialize modules */
    logger_init(output, DEBUG_MODE);
    error_init();
    counter_init(COUNTCONFIG);
    out_writer_init(output != NULL ? output : stdout, 
                   DEBUG_MODE ? OUT_MODE_DEBUG : OUT_MODE_RELEASE);
    
    logger_info("Scanner starting...");
    
    /* Initialize character stream */
    CharStream cs;
    cs_init_string(&cs, source, strlen(source));
    
    /* Initialize token list */
    TokenList tokens;
    token_list_init(&tokens);
    
    /* Main scanning loop */
    out_writer_header();
    
    while (!cs_is_eof(&cs)) {
        Token token;
        ScanResult result = automata_scan_next(&cs, &token);
        
        /* Add token to list */
        token_list_add(&tokens, token);
        
        /* Update counters */
        counter_increment(CNT_TOKENS_TOTAL);
        switch (token.category) {
            case CAT_NUMBER:        counter_increment(CNT_NUMBERS); break;
            case CAT_IDENTIFIER:    counter_increment(CNT_IDENTIFIERS); break;
            case CAT_KEYWORD:       counter_increment(CNT_KEYWORDS); break;
            case CAT_LITERAL:       counter_increment(CNT_LITERALS); break;
            case CAT_OPERATOR:      counter_increment(CNT_OPERATORS); break;
            case CAT_SPECIALCHAR:   counter_increment(CNT_SPECIALCHARS); break;
            case CAT_NONRECOGNIZED: 
                counter_increment(CNT_NONRECOGNIZED); 
                counter_increment(CNT_ERRORS);
                break;
            default: break;
        }
        
        /* Check for errors */
        if (result == SCAN_ERROR) {
            error_report(ERR_NON_RECOGNIZED_CHAR, token.line, token.lexeme);
        }
        
        /* Check for EOF */
        if (token.category == CAT_EOF) {
            break;
        }
    }
    
    /* Write output */
    out_writer_write(&tokens);
    out_writer_footer();
    
    /* Report summary */
    logger_info("Scanning complete. %d tokens found.", token_list_count(&tokens));
    
    /* Cleanup */
    int exit_code = error_has_errors() ? 1 : 0;
    
    token_list_free(&tokens);
    cs_close(&cs);
    counter_close();
    error_close();
    out_writer_close();
    logger_close();
    
    return exit_code;
}

int driver_run_file(const char* input_filename, const char* output_filename) {
    if (input_filename == NULL) {
        fprintf(stderr, "Error: No input file specified\n");
        return 1;
    }
    
    /* Open input file */
    FILE* input = fopen(input_filename, "r");
    if (input == NULL) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_filename);
        return 1;
    }
    
    /* Get file size */
    fseek(input, 0, SEEK_END);
    long size = ftell(input);
    fseek(input, 0, SEEK_SET);
    
    /* Read file into buffer */
    char* source = (char*)malloc(size + 1);
    if (source == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(input);
        return 1;
    }
    
    size_t read_size = fread(source, 1, size, input);
    source[read_size] = '\0';
    fclose(input);
    
    /* Open output file if specified */
    FILE* output = NULL;
    if (output_filename != NULL) {
        output = fopen(output_filename, "w");
        if (output == NULL) {
            fprintf(stderr, "Error: Cannot open output file '%s'\n", output_filename);
            free(source);
            return 1;
        }
    }
    
    /* Run scanner */
    int result = driver_run_string(source, output);
    
    /* Cleanup */
    free(source);
    if (output != NULL) {
        fclose(output);
    }
    
    return result;
}

void driver_print_usage(const char* program_name) {
    printf("Usage: %s <input_file> [output_file]\n", program_name);
    printf("\n");
    printf("Arguments:\n");
    printf("  input_file    Source code file to scan\n");
    printf("  output_file   Output file for tokens (optional, default: stdout)\n");
    printf("\n");
    printf("Compile-time flags:\n");
    printf("  DEBUG_MODE=%d (0=Release, 1=Debug)\n", DEBUG_MODE);
    printf("  COUNTCONFIG=%d (0=Disabled, 1=Enabled)\n", COUNTCONFIG);
}
