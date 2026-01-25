/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module provides unit tests for the pp_core preprocessing engine.
 *
 * - `run_pp_core`: Helper to execute pp_run with a given input string.
 * - `test_comment_line`: Verifies single-line comment removal.
 * - `test_comment_block`: Verifies block comment removal across lines.
 *
 * Usage:
 *     Built and executed by the CTest runner.
 *
 * Status:
 *     Initial development - focuses on comment-removal behavior.
 * -------------------------------------------------------------------------- */

#include "pp_core/pp_core.h"
#include "pp_core/pp_context.h"
#include "buffer/buffer.h"
#include "cli/cli.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Shared output handle expected by logging modules. */
FILE *ofile = NULL;

/* Test base directory used for pp_run. */
#define TEST_BASE_DIR "."
/* Test input filename used for context. */
#define TEST_INPUT_NAME "test.c"

/* Helper: run pp_core with provided input and options. */
static void run_pp_core(const char *input_str, const cli_options_t *opt, buffer_t *out)
{
    buffer_t in;
    buffer_init(&in);
    buffer_init(out);
    buffer_append_str(&in, input_str);

    pp_context_t ctx;
    ctx.opt = *opt;
    ctx.current_file = TEST_INPUT_NAME;
    ctx.current_line = 0;

    pp_run(&ctx, &in, out, TEST_BASE_DIR);

    buffer_free(&in);
}

/* Verify single-line comment removal. */
static void test_comment_line(void)
{
    cli_options_t opt = {0};
    opt.do_comments = 1;
    opt.do_directives = 0;
    opt.do_help = 0;

    const char *input = "int x = 1; // comment\n";
    const char *expected = "int x = 1;  \n";

    buffer_t out;
    run_pp_core(input, &opt, &out);

    assert(out.data != NULL);
    assert(strcmp(out.data, expected) == 0);
    buffer_free(&out);
}

/* Verify block comment removal across lines. */
static void test_comment_block(void)
{
    cli_options_t opt = {0};
    opt.do_comments = 1;
    opt.do_directives = 0;
    opt.do_help = 0;

    const char *input = "a/*b\nc*/d\n";
    const char *expected = "a \nd\n";

    buffer_t out;
    run_pp_core(input, &opt, &out);

    assert(out.data != NULL);
    assert(strcmp(out.data, expected) == 0);
    buffer_free(&out);
}

int main(void)
{
    ofile = stdout;
    printf("=== pp_core Test Suite ===\n\n");

    test_comment_line();
    test_comment_block();

    printf("=== All pp_core tests passed! ===\n\n");
    return 0;
}
