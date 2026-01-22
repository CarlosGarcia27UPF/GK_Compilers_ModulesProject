// This is the main program file
#include "input-example.h"

#define MIN 10

int main() {
    int array[MAX];  // Will be replaced by 100
    int start = MIN; // Will be replaced by 10
    
    #ifdef DEBUG
    printf("Debug mode is on\n");  /* This line will be included */
    #endif
    
    // Single line comment
    int x = 5; /* inline comment */ int y = 10;
    
    /*
     * Multi-line
     * comment block
     */
    
    return 0;
}
