/*
 * =============================================================================
 * test_lang_spec.h
 * Constants and declarations for lang_spec unit tests.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef TEST_LANG_SPEC_H
#define TEST_LANG_SPEC_H

#include "../tests/test_modules.h"
#include "../src/lang_spec/lang_spec.h"

/* Output log for this test module */
#define LANG_SPEC_TEST_LOG  "./test_lang_spec.log"

/* Expected values from language1.txt for assertion checks */
#define LANG1_EXPECTED_TERMINALS    6
#define LANG1_EXPECTED_NONTERMINALS 4
#define LANG1_EXPECTED_RULES        7
#define LANG1_EXPECTED_STATES      13

/* Function prototypes */
void test_ls_load(const char *lang_file);
void test_ls_map_token(const lang_spec_t *ls);
void test_ls_get_action(const lang_spec_t *ls);

#endif /* TEST_LANG_SPEC_H */
