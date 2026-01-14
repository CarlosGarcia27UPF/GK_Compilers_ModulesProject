#include "./main.h"

FILE* ofile = NULL; // The output handler for the project run (same variable name as in modules)

int main(int argc, char *argv[]) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning

    ofile = stdout; // Default output to stdout
    ofile = set_output_test_file(PROJOUTFILENAME);

    fprintf(ofile, "=== P1PP Preprocessor - Module Initialization ===\n\n");

    // Initialize all modules for TM1
    cli_init();
    io_init();
    comments_init();
    directives_init();
    macros_init();
    errors_init();
    tokens_init();

    fprintf(ofile, "\n=== All modules initialized successfully! ===\n");
    printf("All modules executed successfully!\n\n");

    fclose(ofile); 
    return 0;
}