#include <stdio.h>
#include <string.h>

#include "../src/macros/macros.h"
#include "../src/buffer/buffer.h"

int main(void)
{
    macro_table_t table;
    buffer_t output;
    
    macros_init(&table);
    buffer_init(&output);

    /* Test 1: Add and retrieve a macro */
    macros_define(&table, "MAX", "10");
    const char *val = macros_get(&table, "MAX", 3);
    
    if (val && strcmp(val, "10") == 0) {
        printf("[PASS] Macro add and get works\n");
    } else {
        printf("[FAIL] Macro add and get failed\n");
        return 1;
    }

    /* Test 2: Expand line with macro */
    if (macros_expand_line(&table, "int a = MAX;\n", 13, &output) == 0) {
        printf("[PASS] Macro expansion succeeded\n");
        printf("Output: %.*s", (int)output.len, output.data);
    } else {
        printf("[FAIL] Macro expansion failed\n");
        return 1;
    }

    macros_free(&table);
    buffer_free(&output);

    return 0;
}
