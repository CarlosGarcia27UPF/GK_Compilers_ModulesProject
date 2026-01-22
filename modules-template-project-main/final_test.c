/* Final comprehensive test file */
// Testing all features

#define ARRAY_SIZE 100
#define APP_NAME "FinalTest"

#include "input-example.h"

int main() {
    // Comment should be removed
    int arr[ARRAY_SIZE];  /* ARRAY_SIZE should become 100 */
    
    #ifdef DEBUG
    printf("Debug: MAX is %d\n", MAX);  // MAX should become 100
    #endif
    
    /* Multi-line comment
       should be replaced
       by spaces */
    
    return 0;
}
