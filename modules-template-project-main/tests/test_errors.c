/*
 * tests/test_errors.c
 *
 * Use this as a template to create new test modules for each module in the project.
 */

#include "test_modules.h"
#include "../src/errors/errors.h"

FILE *ofile = NULL; // file handler to send the module's output (to a file or stdout)

void test_reporting() {
    printf("Testing error reporting functionality...\n");
    if (ofile) fprintf(ofile, "Testing error reporting functionality...\n");

    error_reset_count();
    assert(error_get_count() == 0);

    // Test simple error
    error_report("test.c", 42, "This is a test error");
    assert(error_get_count() == 1);

    // Test formatted error
    error_report("test.c", 100, "Validation failed for %s", "variable_x");
    assert(error_get_count() == 2);

    printf("Error reporting tests passed!\n");
    if (ofile) fprintf(ofile, "Error reporting tests passed!\n");
}

int main(void) {
    printf("Starting test_errors main...\n");
    ofile = stdout; // Default output to stdout
    printf("Setting output file...\n");
    ofile = set_output_test_file("test_errors.log");
    
    if (ofile == NULL) {
        printf("Failed to open output file!\n");
        return 1;
    }
    printf("Output file set. Running tests...\n");

    fprintf(ofile, "=== Test Run: Errors Module ===\n");
    
    test_reporting();
    
    fprintf(ofile, "=== Test Run Finished ===\n");
    
    if (ofile && ofile != stdout) fclose(ofile); 
    return 0;
}
