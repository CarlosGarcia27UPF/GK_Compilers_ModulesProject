/*
 * =============================================================================
 * main.c
 * Entry point for the p3_parser Shift-Reduce Automaton parser.
 *
 * Workflow:
 *   1. Open output log.
 *   2. Parse CLI arguments into g_config.
 *   3. Load the grammar specification into g_lang.
 *   4. Load tokens from the .cscn scanner-output file.
 *   5. Open the debug output file.
 *   6. Run the SRA engine.
 *   7. Clean up and report result.
 *
 * Author: [Team]
 * =============================================================================
 */

#include "./main.h"

FILE        *ofile    = NULL; /* Shared output log */
config_t     g_config;        /* CLI arguments      */
lang_spec_t  g_lang;          /* Language spec      */

int main(int argc, char *argv[])
{
    parser_token_list_t tokens;
    parser_stack_t      stack;
    FILE               *dbg_file;
    int                 result;

    ofile = stdout;
    ofile = set_output_test_file(PROJOUTFILENAME);

    if (process_arguments(argc, argv, &g_config) != 0) {
        fclose(ofile);
        return 1;
    }

    if (ls_load(&g_lang, g_config.lang_file) != 0) {
        fprintf(ofile, "Failed to load language spec: %s\n", g_config.lang_file);
        fclose(ofile);
        return 1;
    }

    ptl_init(&tokens);
    if (tok_load(&tokens, &g_lang, g_config.input_file) != 0) {
        fprintf(ofile, "Failed to load tokens from: %s\n", g_config.input_file);
        ptl_free(&tokens);
        ls_free(&g_lang);
        fclose(ofile);
        return 1;
    }

    dbg_file = ow_open_dbg(g_config.input_file);
    result   = sra_run(&g_lang, &tokens, &stack, dbg_file);
    ow_close_dbg(dbg_file);

    ptl_free(&tokens);
    ls_free(&g_lang);

    fprintf(ofile, "Parse result: %s\n", result ? "ACCEPTED" : "ERROR");
    fclose(ofile);
    return result ? 0 : 1;
}
