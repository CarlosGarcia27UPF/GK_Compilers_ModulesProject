/*
 * -----------------------------------------------------------------------------
 * test_scanner.h
 *
 * Test header for the scanner module. Contains test constants and
 * expected values for scanner testing.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef TEST_SCANNER_H
#define TEST_SCANNER_H

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/lang_spec/lang_spec.h"
#include "../src/char_stream/char_stream.h"
#include "../src/token/token.h"
#include "../src/token_list/token_list.h"
#include "../src/automata/automata.h"
#include "../src/out_writer/out_writer.h"
#include "../src/error/error.h"
#include "../src/logger/logger.h"
#include "../src/counter/counter.h"

/* Test input file path */
#define TEST_INPUT_FILE "/tmp/scanner_test_input.c"

/* Test output file path (expected) */
#define TEST_OUTPUT_FILE "/tmp/scanner_test_input.cscn"

/* Number of expected tokens for the basic test input */
#define TEST_BASIC_EXPECTED_TOKENS 17

#endif /* TEST_SCANNER_H */
