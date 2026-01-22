// Test file for comprehensive preprocessing
#define VERSION 2
#define NAME "TestApp"

#ifdef VERSION
/* This comment should be removed */
int version = VERSION;
#endif

#include "test_header.h"

// Undefined macro should not cause issues
int undefined_value = UNDEFINED_MACRO;

int main() {
    return 0;
}
