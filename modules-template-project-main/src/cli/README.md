# CLI Module Documentation

## Overview
The CLI (Command-Line Interface) module is responsible for parsing command-line arguments and providing help information to users. It serves as the entry point for user interaction with the C preprocessor.

## Files
- **cli.h** - Header file with structure definitions and function declarations
- **cli.c** - Implementation of CLI parsing and help functionality
- **CMakeLists.txt** - Build configuration for the CLI module

## Core Data Structures

### `cli_options_t`
```c
typedef struct {
    int do_comments;    // -c flag
    int do_directives;  // -d flag
    int do_help;        // -help flag
} cli_options_t;
```

This structure holds the parsed command-line options as boolean flags (0 or 1).

**Fields:**
- `do_comments`: When set (1), enables comment removal from C source files
- `do_directives`: When set (1), enables directive processing (#include, #define, #ifdef, etc.) and macro expansion
- `do_help`: When set (1), displays help information instead of processing files

## Functions

### `cli_options_t cli_parse(int argc, char **argv)`

**Purpose:** Parses command-line arguments and returns a populated `cli_options_t` structure.

**Parameters:**
- `argc`: Argument count from main()
- `argv`: Argument vector from main()

**Returns:** A `cli_options_t` structure with flags set based on provided arguments

**Behavior:**
1. **Initialization:** All flags start at 0 (disabled)
2. **Default behavior:** If NO flags are provided, `-c` is automatically enabled (comment removal)
3. **Flag detection:** Scans all arguments looking for recognized flags
4. **Flag processing:**
   - `-help`: Sets `do_help` flag
   - `-all`: Sets both `do_comments` AND `do_directives` flags
   - `-c`: Sets `do_comments` flag
   - `-d`: Sets `do_directives` flag
   - Other arguments: Ignored (assumed to be filenames)

**Example Usage:**
```c
// No flags: default to -c
$ pp myfile.c
// Result: do_comments=1, do_directives=0, do_help=0

// Explicit -c flag
$ pp -c myfile.c
// Result: do_comments=1, do_directives=0, do_help=0

// Both flags
$ pp -c -d myfile.c
// Result: do_comments=1, do_directives=1, do_help=0

// All flag (shortcut)
$ pp -all myfile.c
// Result: do_comments=1, do_directives=1, do_help=0

// Help flag
$ pp -help
// Result: do_comments=0, do_directives=0, do_help=1
```

**Implementation Details:**

The function uses a two-pass approach:

**Pass 1:** Detect if any flags exist
```c
int has_any_flag = 0;
for (int i = 1; i < argc; i++) {
    if (argv[i] != NULL && argv[i][0] == '-') {
        has_any_flag = 1;
        break;
    }
}
```

**Pass 2:** Parse each flag
```c
for (int i = 1; i < argc; i++) {
    if (is_flag(argv[i], PP_FLAG_HELP)) {
        opt.do_help = 1;
    } else if (is_flag(argv[i], PP_FLAG_ALL)) {
        opt.do_comments = 1;
        opt.do_directives = 1;
    }
    // ... etc
}
```

### `static int is_flag(const char *arg, const char *flag)`

**Purpose:** Helper function that safely compares a command-line argument with a flag string.

**Parameters:**
- `arg`: The command-line argument to check
- `flag`: The expected flag string (e.g., "-c", "-help")

**Returns:** 
- `1` if arg matches flag exactly
- `0` otherwise

**Safety:** Checks for NULL before comparison to prevent crashes

**Implementation:**
```c
return (arg != NULL) && (strcmp(arg, flag) == 0);
```

### `void cli_print_help(const char *progname)`

**Purpose:** Prints usage information and available command-line options to stdout.

**Parameters:**
- `progname`: Name of the program (typically `argv[0]`). If NULL, defaults to "pp"

**Output Format:**
```
Usage:
  pp [options] <file.c|file.h>

Options:
  -c     Remove comments (default if no flags)
  -d     Process directives (#include, #define, #ifdef/#endif) + macro expansion
  -all   Equivalent to -c -d
  -help  Show this help
```

**When Called:**
- When `-help` flag is provided
- When no input file is provided (error case in main.c)

## Design Patterns and Principles

### 1. **Separation of Concerns**
The CLI module only handles argument parsing - it doesn't perform file I/O or preprocessing logic.

### 2. **Default Behavior**
Smart defaults improve user experience: running `pp myfile.c` without flags does the most common operation (comment removal).

### 3. **Flag Accumulation**
Multiple flags can be combined. `-c -d` has the same effect as `-all`.

### 4. **Null Safety**
All string operations check for NULL pointers before dereferencing.

### 5. **Static Helper Functions**
`is_flag()` is marked `static` to limit its scope to this compilation unit, preventing name collisions.

## Integration with Other Modules

### Dependencies
- **spec/pp_spec.h**: Imports flag constant definitions (`PP_FLAG_C`, `PP_FLAG_D`, etc.)

### Used By
- **main.c**: Calls `cli_parse()` at program startup to determine operation mode
- **pp_core/pp_context.h**: Stores `cli_options_t` in the preprocessing context

## Flag Definitions (from spec/pp_spec.h)

```c
#define PP_FLAG_C    "-c"      // Comment removal
#define PP_FLAG_D    "-d"      // Directive processing
#define PP_FLAG_ALL  "-all"    // Both operations
#define PP_FLAG_HELP "-help"   // Help display
```

## Common Usage Patterns

### In main.c
```c
#include "cli/cli.h"

int main(int argc, char **argv)
{
    // Parse command line
    cli_options_t opt = cli_parse(argc, argv);
    
    // Check if user wants help
    if (opt.do_help) {
        cli_print_help(argv[0]);
        return 0;
    }
    
    // Use flags to configure preprocessing context
    pp_context_t ctx;
    ctx.opt = opt;
    // ... continue with file processing
}
```

## Error Handling

The CLI module itself doesn't perform validation of:
- File existence (handled by IO module)
- Flag combinations (all combinations are valid)
- Argument count (handled by main.c)

## Future Extensions

The structure allows easy addition of new flags:
1. Add constant to `spec/pp_spec.h`
2. Add field to `cli_options_t` structure
3. Add case in `cli_parse()` function
4. Update `cli_print_help()` documentation

Example for adding a `-v` (verbose) flag:
```c
// In cli.h
typedef struct {
    int do_comments;
    int do_directives;
    int do_help;
    int verbose;        // NEW
} cli_options_t;

// In cli.c
} else if (is_flag(a, "-v")) {
    opt.verbose = 1;
}

// In help text
printf("  -v     Enable verbose output\n");
```

## Summary

The CLI module provides a clean, maintainable interface for command-line argument parsing. Its design prioritizes:
- **Simplicity**: Easy to understand and extend
- **Safety**: NULL checks prevent crashes
- **Usability**: Smart defaults and helpful error messages
- **Modularity**: No dependencies on I/O or preprocessing logic

This makes it easy to test independently and modify without affecting other parts of the codebase.
