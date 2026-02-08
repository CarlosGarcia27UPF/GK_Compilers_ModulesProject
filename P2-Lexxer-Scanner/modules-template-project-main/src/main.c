#include "./main.h"
#include "./driver/driver.h"

FILE* ofile = NULL; // The output handler for the project run

int main(int argc, char *argv[]) {
    ofile = stdout;

    /* P2 Scanner mode */
    if (argc >= 2) {
        /* Run scanner on input file */
        const char* input_file = argv[1];
        const char* output_file = (argc >= 3) ? argv[2] : NULL;
        
        printf("=== P2 Scanner/Lexer ===\n");
        printf("Input: %s\n", input_file);
        if (output_file) {
            printf("Output: %s\n", output_file);
        }
        printf("\n");
        
        int result = driver_run_file(input_file, output_file);
        
        printf("\nScanner finished with exit code: %d\n", result);
        return result;
    }
    
    /* Demo mode - run on example string */
    printf("=== P2 Scanner/Lexer Demo ===\n");
    printf("Usage: %s <input_file> [output_file]\n\n", argv[0]);
    
    printf("Running demo with example code...\n\n");
    
    const char* demo_code = 
        "if (x > 5) {\n"
        "    return 42;\n"
        "}\n"
        "while (count) {\n"
        "    x = x + 1;\n"
        "}\n"
        "\"hello world\"\n"
        "@#$ invalid\n";
    
    printf("--- Source Code ---\n%s\n--- End Source ---\n\n", demo_code);
    
    int result = driver_run_string(demo_code, stdout);
    
    printf("\nDemo finished with exit code: %d\n", result);
    return result;
}