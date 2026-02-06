#include <stdio.h>
#include <string.h>

#include "../src/macros/macros.h"
#include "../src/directives/directives.h"
#include "../src/buffer/buffer.h"
#include "../src/comments/comments.h"
#include "../src/errors/errors.h"

/* Global output file required by modules */
FILE* ofile = NULL;

/*
 * This test simulates PP_CORE behavior:
 *  - Processes input line by line
 *  - Dispatches directives or macros
 *  - Accumulates output
 */

int main(void)
{
    /* Initialize global state */
    macro_table_t macros;
    ifdef_stack_t ifdef_stack;
    buffer_t output;
    comment_state_t comment_state;

    macros_init(&macros);
    ifdef_stack_init(&ifdef_stack);
    buffer_init(&output);
    comments_state_init(&comment_state);
    errors_init();

    const char *current_file = "test_process_line.c";
    const char *base_dir = ".";

    /* -------------------------
     * Input lines (line by line)
     * ------------------------- */
    const char *lines[] = {
        "#define MAX 10\n",
        "#define FLAG\n",
        "int a = MAX;\n",
        "#ifdef FLAG\n",
        "int b = MAX;\n",
        "#endif\n",
        "int c = MAX;\n"
    };

    int num_lines = sizeof(lines) / sizeof(lines[0]);

    /* -------------------------
     * Simulate PP_CORE loop
     * ------------------------- */
    for (int i = 0; i < num_lines; i++) {
        const char *line = lines[i];
        long len = strlen(line);
        int line_num = i + 1;

        /* Directive line? */
        if (line[0] == '#') {
            directives_process_line(
                line,
                len,
                base_dir,
                current_file,
                line_num,
                &macros,
                &ifdef_stack,
                0,                      /* do_comments = false */
                &comment_state,
                &output);
        } else {
            /* Normal code line */
            if (ifdef_should_include(&ifdef_stack)) {
                macros_expand_line(
                    &macros,
                    line,
                    (int)len,
                    &output
                );
            }
        }
    }

    /* -------------------------
     * Print ACTUAL output
     * ------------------------- */
    printf("=== ACTUAL OUTPUT ===\n");
    printf("%.*s\n", (int)output.len, output.data);

    /* -------------------------
     * Expected output (tokenizer removes most whitespace)
     * ------------------------- */
    const char *expected =
        "inta=10;intb=10;intc=10;";

    printf("=== EXPECTED OUTPUT ===\n");
    printf("%s\n", expected);

    /* -------------------------
     * Compare
     * ------------------------- */
    printf("Actual length: %ld, Expected length: %ld\n", output.len, strlen(expected));
    if (output.len == (long)strlen(expected) &&
        strncmp(output.data, expected, output.len) == 0) {
        printf("[TEST PASSED]\n");
    } else {
        printf("[TEST FAILED]\n");
        printf("Hex dump of actual:\n");
        for (long i = 0; i < output.len && i < 50; i++) {
            printf("%02x ", (unsigned char)output.data[i]);
        }
        printf("\n");
    }

    /* Cleanup */
    buffer_free(&output);
    macros_free(&macros);

    return 0;
}
