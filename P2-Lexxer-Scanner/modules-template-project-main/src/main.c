/*
 * -----------------------------------------------------------------------------
 * main.c
 *
 * Scanner driver (Practice 2 — Lexical Analysis).
 * This is the driver that orchestrates the scanner pipeline.
 *
 * Usage: ./scanner <input.c>
 *
 * Steps:
 *   1. Parse command-line arguments.
 *   2. Open the input file via char_stream.
 *   3. Run the automata scanner to produce the token list.
 *   4. Write the token list to the .cscn output file.
 *   5. (Future hook) Call the parser with the in-memory token list.
 *   6. Clean up resources.
 *
 * Team Member: Emmanuel Kwabena Cooper Acheampong
 * -----------------------------------------------------------------------------
 */

#include "./main.h"

FILE* ofile = NULL; // Output handler used by template modules.

// Prints CLI usage.
static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <input.c>\n", prog_name);
}

// Orchestrates scanner execution for one input file.
static int run_scanner(const char *input_filename) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    FILE *debug_out = NULL;
    char output_filename[MAX_FILENAME_BUF];
    int result;
#ifdef COUNTCONFIG
    FILE *count_trace_dest = stdout;
    FILE *count_summary_dest = stdout;
    int close_count_trace_dest = 0;
    int close_count_summary_dest = 0;
    int count_trace_enabled = 1;
    char count_filename[MAX_FILENAME_BUF];
#endif

    if (input_filename == NULL) {
        return ERR_FILE_OPEN;
    }

    // Initialize subsystems.
    counter_init(&cnt);
    tl_init(&tokens);

    // Build output filename: input.c -> input.cscn.
    ow_build_output_filename(input_filename, output_filename, MAX_FILENAME_BUF);

#ifdef COUNTCONFIG
    if (COUNTOUT == COUNTOUT_OUT && COUNTFILE == COUNTFILE_DBGCNT) {
        ow_build_count_filename(input_filename, count_filename,
                                MAX_FILENAME_BUF);
        count_trace_dest = fopen(count_filename, "w");
        if (count_trace_dest == NULL) {
            count_trace_dest = stdout;
        } else {
            close_count_trace_dest = 1;
        }
    }

    if (COUNTOUT == COUNTOUT_OUT && COUNTFILE == COUNTFILE_OUTPUT) {
        // Writing live traces here would be overwritten by token-file write mode.
        count_trace_enabled = 0;
    }

    counter_set_trace(&cnt, count_trace_dest, count_trace_enabled);
#endif

    // Initialize logger destination.
    if (DEBUG_FLAG == DEBUG_ON) {
        debug_out = fopen(output_filename, "w");
        if (debug_out == NULL) {
#ifdef COUNTCONFIG
            if (close_count_trace_dest) {
                fclose(count_trace_dest);
            }
#endif
            tl_free(&tokens);
            return ERR_FILE_OUTPUT;
        }
        logger_init(&lg, debug_out, input_filename);
    } else {
        logger_init(&lg, stdout, input_filename);
    }

    // Open input file.
    if (cs_open(&cs, input_filename) != 0) {
        err_report(logger_get_dest(&lg), ERR_FILE_OPEN, ERR_STEP_DRIVER,
                   0, input_filename);
        if (debug_out != NULL) {
            fclose(debug_out);
        }
#ifdef COUNTCONFIG
        if (close_count_trace_dest) {
            fclose(count_trace_dest);
        }
#endif
        tl_free(&tokens);
        return ERR_FILE_OPEN;
    }

    logger_write(&lg, "Scanning: %s\n", input_filename);

    // Run scanner.
    result = automata_scan(&cs, &tokens, &lg, &cnt);

    // Close input stream.
    cs_close(&cs);

    if (debug_out != NULL) {
        fflush(debug_out);
    }

    // Write token file.
    if (ow_write_token_file_mode(&tokens, output_filename,
                                 (DEBUG_FLAG == DEBUG_ON)) != 0) {
        err_report(logger_get_dest(&lg), ERR_FILE_OUTPUT, ERR_STEP_DRIVER,
                   0, output_filename);
        if (debug_out != NULL) {
            fclose(debug_out);
            debug_out = NULL;
        }
#ifdef COUNTCONFIG
        if (close_count_trace_dest) {
            fclose(count_trace_dest);
        }
#endif
        tl_free(&tokens);
        return ERR_FILE_OUTPUT;
    }

    // Token writer appends using a separate FILE handle; reposition to EOF before
    // writing more debug messages through the logger handle.
    if (debug_out != NULL) {
        fseek(debug_out, 0, SEEK_END);
    }

    logger_write(&lg, "Output written to: %s\n", output_filename);
    logger_write(&lg, "Tokens found: %d\n", tl_count(&tokens));
#ifdef COUNTCONFIG
    if (COUNTOUT == COUNTOUT_OUT &&
        COUNTFILE == COUNTFILE_DBGCNT &&
        close_count_trace_dest) {
        logger_write(&lg, "Count output written to: %s\n", count_filename);
    }
#endif

    if (debug_out != NULL) {
        fclose(debug_out);
        debug_out = NULL;
        logger_init(&lg, stdout, input_filename);
    }

#ifdef COUNTCONFIG
    if (COUNTOUT == COUNTOUT_OUT) {
        if (COUNTFILE == COUNTFILE_DBGCNT) {
            count_summary_dest = count_trace_dest;
        } else {
            count_summary_dest = fopen(output_filename, "a");
            if (count_summary_dest == NULL) {
                count_summary_dest = stdout;
            } else {
                close_count_summary_dest = 1;
            }
        }
    }

    counter_print(&cnt, count_summary_dest, "run_scanner", 0);

    if (close_count_summary_dest) {
        fclose(count_summary_dest);
        count_summary_dest = NULL;
    }
    if (close_count_trace_dest) {
        fclose(count_trace_dest);
    }
#endif

    // Future hook: parser can consume the in-memory token list here.
    // Clean up.
    tl_free(&tokens);
    logger_close(&lg);

    return result;
}

// Entry point wrapper.
int main(int argc, char *argv[]) {
    int result;

    ofile = stdout;

    if (argc < MIN_ARGS) {
        print_usage(argv[0]);
        return ERR_FILE_OPEN;
    }

    result = run_scanner(argv[ARG_INPUT_FILE]);

    return result;
}
