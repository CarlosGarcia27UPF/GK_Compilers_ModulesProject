/*
 * =============================================================================
 * test_lang_spec.c
 * Unit tests for the lang_spec module.
 *
 * Tests ls_load, ls_map_token, and ls_get_action against language1.txt.
 * The path to the lang spec file is passed as argv[1].
 *
 * Author: [Team]
 * =============================================================================
 */

#include <string.h>
#include "../tests/test_lang_spec.h"

FILE *ofile = NULL; /* Required by utils_files.c */

/* Test that the spec file loads and has the expected counts. */
void test_ls_load(const char *lang_file)
{
    lang_spec_t ls;
    int rc = ls_load(&ls, lang_file);
    assert(rc == 0);
    assert(ls.dfa.num_terminals    == LANG1_EXPECTED_TERMINALS);
    assert(ls.dfa.num_nonterminals == LANG1_EXPECTED_NONTERMINALS);
    assert(ls.num_rules            == LANG1_EXPECTED_RULES);
    assert(ls.dfa.num_states       == LANG1_EXPECTED_STATES);
    fprintf(ofile, "test_ls_load: PASSED (lang=%s)\n", ls.lang_name);
    ls_free(&ls);
}

/* Test terminal mapping: lexeme "+" should map to terminal 0. */
void test_ls_map_token(const lang_spec_t *ls)
{
    int idx_plus = ls_map_token(ls, "+", "CAT_OPERATOR");
    assert(idx_plus == 0); /* terminal 0 is '+' (LEXEME:+) */

    int idx_num = ls_map_token(ls, "42", "CAT_NUMBER");
    assert(idx_num == 4); /* terminal 4 is NUM (CATEGORY:CAT_NUMBER) */

    int idx_eof = ls_map_token(ls, NULL, NULL);
    assert(idx_eof == 5); /* terminal 5 is '$' (EOF) */

    int idx_miss = ls_map_token(ls, "xyz", "CAT_UNKNOWN");
    assert(idx_miss == MATCH_NOTFOUND);

    fprintf(ofile, "test_ls_map_token: PASSED\n");
}

/* Test action table: state 0 + terminal NUM(4) should be SHIFT to state 6. */
void test_ls_get_action(const lang_spec_t *ls)
{
    action_t a = ls_get_action(ls, 0, 4); /* state=0, terminal=NUM */
    assert(a.type  == ACTION_SHIFT);
    assert(a.value == 6);

    action_t b = ls_get_action(ls, 1, 5); /* state=1, terminal=$ -> ACCEPT */
    assert(b.type  == ACTION_ACCEPT);

    action_t c = ls_get_action(ls, 0, 0); /* state=0, terminal=+ -> ERROR */
    assert(c.type  == ACTION_ERROR);

    fprintf(ofile, "test_ls_get_action: PASSED\n");
}

int main(int argc, char *argv[])
{
    ofile = stdout;
    ofile = set_output_test_file(LANG_SPEC_TEST_LOG);

    const char *lang_file = (argc >= 2) ? argv[1] : "./data/language1.txt";

    fprintf(ofile, "=== test_lang_spec starting ===\n");

    test_ls_load(lang_file);

    lang_spec_t ls;
    ls_load(&ls, lang_file);
    test_ls_map_token(&ls);
    test_ls_get_action(&ls);
    ls_free(&ls);

    fprintf(ofile, "=== test_lang_spec: ALL PASSED ===\n");
    printf("test_lang_spec: ALL PASSED\n");

    fclose(ofile);
    return 0;
}
