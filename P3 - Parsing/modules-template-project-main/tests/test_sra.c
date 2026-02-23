/*
 * =============================================================================
 * test_sra.c
 * Integration tests for the SRA engine.
 *
 * Runs the SRA engine on sample .cscn files and asserts the expected result.
 * File paths are passed as command-line arguments:
 *   argv[1] : .cscn input file
 *   argv[2] : language spec file
 * An optional third argument selects the expected result:
 *   argv[3] : "accept" (default) or "error"
 *
 * Author: [Team]
 * =============================================================================
 */

#include "../tests/test_sra.h"

FILE *ofile = NULL; /* Required by utils_files.c */

/* Run sra_run on cscn_file with lang_file and assert ACCEPT (result == 1). */
void test_sra_accept(const char *cscn_file, const char *lang_file)
{
    lang_spec_t         ls;
    parser_token_list_t tokens;
    parser_stack_t      stack;

    int rc = ls_load(&ls, lang_file);
    assert(rc == 0);

    ptl_init(&tokens);
    rc = tok_load(&tokens, &ls, cscn_file);
    assert(rc == 0);
    assert(tokens.count > 0);

    int result = sra_run(&ls, &tokens, &stack, NULL);
    assert(result == 1);

    ptl_free(&tokens);
    ls_free(&ls);

    fprintf(ofile, "test_sra_accept(%s): PASSED\n", cscn_file);
}

/* Run sra_run on cscn_file with lang_file and assert ERROR (result == 0). */
void test_sra_error(const char *cscn_file, const char *lang_file)
{
    lang_spec_t         ls;
    parser_token_list_t tokens;
    parser_stack_t      stack;

    int rc = ls_load(&ls, lang_file);
    assert(rc == 0);

    ptl_init(&tokens);
    rc = tok_load(&tokens, &ls, cscn_file);
    assert(rc == 0);

    int result = sra_run(&ls, &tokens, &stack, NULL);
    assert(result == 0);

    ptl_free(&tokens);
    ls_free(&ls);

    fprintf(ofile, "test_sra_error(%s): PASSED\n", cscn_file);
}

int main(int argc, char *argv[])
{
    ofile = stdout;
    ofile = set_output_test_file(SRA_TEST_LOG);

    const char *cscn_file    = (argc >= 2) ? argv[1] : "./samples/sample1.cscn";
    const char *lang_file    = (argc >= 3) ? argv[2] : "./data/language1.txt";
    const char *expect_mode  = (argc >= 4) ? argv[3] : SRA_EXPECT_ACCEPT;

    fprintf(ofile, "=== test_sra starting ===\n");
    fprintf(ofile, "  cscn: %s\n", cscn_file);
    fprintf(ofile, "  lang: %s\n", lang_file);
    fprintf(ofile, "  mode: %s\n", expect_mode);

    if (strcmp(expect_mode, SRA_EXPECT_ERROR) == 0) {
        test_sra_error(cscn_file, lang_file);
    } else {
        test_sra_accept(cscn_file, lang_file);
    }

    fprintf(ofile, "=== test_sra: ALL PASSED ===\n");
    printf("test_sra: ALL PASSED\n");

    fclose(ofile);
    return 0;
}
