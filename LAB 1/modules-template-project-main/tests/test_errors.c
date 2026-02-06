/*
 * tests/test_errors.c
 *
 * Use this as a template to create new test modules for each module in the project.
 */

#include "test_modules.h"
#include "../src/errors/errors.h"
#include "../src/buffer/buffer.h"

// We perform pure testing here, so no external logging file needed for this module's logic verification
// We rely on assertions and buffer inspection.

void test_reporting() {
    printf("Testing error reporting functionality...\n");

    buffer_t error_buf;
    buffer_init(&error_buf);

    errors_init();
    errors_set_buffer(&error_buf);

    assert(get_error_count() == 0);

    // Test simple error
    error(42, "This is a test error");
    assert(get_error_count() == 1);
    
    // Check buffer content
    const char *expected = "Error on line 42: This is a test error\n";
    assert(strstr(error_buf.data, expected) != NULL);

    // Test formatted error
    error(100, "Validation failed for %s", "variable_x");
    assert(get_error_count() == 2);
    
    // Check buffer content again
    const char *expected2 = "Error on line 100: Validation failed for variable_x\n";
    assert(strstr(error_buf.data, expected2) != NULL);
    
    buffer_free(&error_buf);

    printf("Error reporting tests passed!\n");
}

int main(void) {
    printf("Starting test_errors main...\n");
    printf("Running tests...\n");

    printf("=== Test Run: Errors Module ===\n");
    
    test_reporting();
    
    printf("=== Test Run Finished ===\n");
    
    return 0;
}
