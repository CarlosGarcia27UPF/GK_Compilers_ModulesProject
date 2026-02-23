/*
 * =============================================================================
 * test_dfa.c
 * Unit tests for the dfa module.
 *
 * Tests dfa_init, dfa_set_action, dfa_get_action, dfa_set_goto, dfa_get_goto,
 * and bounds checking for out-of-range indices.
 *
 * Author: [Team]
 * =============================================================================
 */

#include "../tests/test_dfa.h"

FILE *ofile = NULL; /* Required by utils_files.c */

/* Test that dfa_init zeroes all fields. */
void test_dfa_init(void)
{
    dfa_t dfa;
    dfa_init(&dfa);
    assert(dfa.num_states       == 0);
    assert(dfa.num_terminals    == 0);
    assert(dfa.num_nonterminals == 0);
    /* After init, any get should return ERROR / -1 (all counts are 0) */
    action_t a = dfa_get_action(&dfa, 0, 0);
    assert(a.type == ACTION_ERROR);
    assert(dfa_get_goto(&dfa, 0, 0) == -1);
    fprintf(ofile, "test_dfa_init: PASSED\n");
}

/* Test setting and getting ACTION entries. */
void test_dfa_set_get_action(void)
{
    dfa_t dfa;
    dfa_init(&dfa);
    dfa.num_states    = 3;
    dfa.num_terminals = 2;

    action_t shift_act = {ACTION_SHIFT, 2};
    dfa_set_action(&dfa, 0, 1, shift_act);

    action_t got = dfa_get_action(&dfa, 0, 1);
    assert(got.type  == ACTION_SHIFT);
    assert(got.value == 2);

    action_t reduce_act = {ACTION_REDUCE, 3};
    dfa_set_action(&dfa, 1, 0, reduce_act);

    got = dfa_get_action(&dfa, 1, 0);
    assert(got.type  == ACTION_REDUCE);
    assert(got.value == 3);

    /* Unset cell should be ERROR */
    got = dfa_get_action(&dfa, 0, 0);
    assert(got.type == ACTION_ERROR);

    fprintf(ofile, "test_dfa_set_get_action: PASSED\n");
}

/* Test setting and getting GOTO entries. */
void test_dfa_set_get_goto(void)
{
    dfa_t dfa;
    dfa_init(&dfa);
    dfa.num_states       = 4;
    dfa.num_nonterminals = 3;

    dfa_set_goto(&dfa, 0, 1, 3);
    assert(dfa_get_goto(&dfa, 0, 1) == 3);

    dfa_set_goto(&dfa, 2, 0, 1);
    assert(dfa_get_goto(&dfa, 2, 0) == 1);

    /* Unset goto cell should be 0 (zeroed by dfa_init) */
    assert(dfa_get_goto(&dfa, 1, 2) == 0);

    fprintf(ofile, "test_dfa_set_get_goto: PASSED\n");
}

/* Test that out-of-range indices return safe defaults. */
void test_dfa_bounds(void)
{
    dfa_t dfa;
    dfa_init(&dfa);
    dfa.num_states       = 2;
    dfa.num_terminals    = 2;
    dfa.num_nonterminals = 2;

    /* Out-of-range state */
    action_t a = dfa_get_action(&dfa, 5, 0);
    assert(a.type == ACTION_ERROR);

    /* Out-of-range terminal */
    a = dfa_get_action(&dfa, 0, 5);
    assert(a.type == ACTION_ERROR);

    /* Negative indices */
    a = dfa_get_action(&dfa, -1, 0);
    assert(a.type == ACTION_ERROR);

    assert(dfa_get_goto(&dfa, 5, 0) == -1);
    assert(dfa_get_goto(&dfa, 0, 5) == -1);
    assert(dfa_get_goto(&dfa, -1, 0) == -1);

    fprintf(ofile, "test_dfa_bounds: PASSED\n");
}

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    ofile = stdout;
    ofile = set_output_test_file(DFA_TEST_LOG);

    fprintf(ofile, "=== test_dfa starting ===\n");

    test_dfa_init();
    test_dfa_set_get_action();
    test_dfa_set_get_goto();
    test_dfa_bounds();

    fprintf(ofile, "=== test_dfa: ALL PASSED ===\n");
    printf("test_dfa: ALL PASSED\n");

    fclose(ofile);
    return 0;
}
