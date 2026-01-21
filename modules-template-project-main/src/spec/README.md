# SPEC Module Documentation

## Overview
The `spec` (Specification) module defines constants, configuration values, and specifications that are used throughout the C preprocessor. It serves as a central location for magic numbers, string constants, and configuration parameters, promoting maintainability and consistency.

## Files
- **pp_spec.h** - Header file with preprocessor specifications and constants

**Note:** This module is header-only - no `.c` implementation file is needed since it only contains `#define` declarations.

---

## Constants Defined

### Flag Definitions

These constants define the command-line flags accepted by the preprocessor.

```c
#define PP_FLAG_C    "-c"
#define PP_FLAG_D    "-d"
#define PP_FLAG_ALL  "-all"
#define PP_FLAG_HELP "-help"
```

#### `PP_FLAG_C` = `"-c"`
**Purpose:** Comment removal flag

**Usage:** When provided, the preprocessor removes C-style comments from source code
- Single-line comments: `// comment`
- Multi-line comments: `/* comment */`

**Example:**
```bash
$ pp -c example.c
# Removes all comments from example.c
```

**Processing Behavior:**
- Scans for `//` and `/*` patterns
- Removes comment text while preserving line structure
- Handles edge cases (strings containing //, nested comments)

---

#### `PP_FLAG_D` = `"-d"`
**Purpose:** Directive processing flag

**Usage:** When provided, the preprocessor processes C preprocessor directives
- `#include` - File inclusion
- `#define` - Macro definition
- `#undef` - Macro removal
- `#ifdef` / `#ifndef` - Conditional compilation
- `#if` / `#elif` / `#else` / `#endif` - Conditional blocks
- `#error` / `#warning` - Diagnostic messages
- `#pragma` - Compiler-specific directives
- `#line` - Line number control

**Example:**
```bash
$ pp -d example.c
# Processes all directives, expands macros
```

**Processing Behavior:**
- Recognizes lines starting with `#`
- Parses directive type and arguments
- Executes appropriate action (include file, define macro, etc.)
- Expands macros in subsequent code
- Tracks conditional compilation state

---

#### `PP_FLAG_ALL` = `"-all"`
**Purpose:** Combined processing flag (shortcut)

**Usage:** Equivalent to providing both `-c` and `-d` flags

**Example:**
```bash
$ pp -all example.c
# Same as: pp -c -d example.c
```

**Rationale:**
- Common use case: users usually want complete preprocessing
- Reduces typing for typical usage
- Makes CLI more user-friendly

**Implementation in cli.c:**
```c
else if (is_flag(a, PP_FLAG_ALL)) {
    opt.do_comments = 1;
    opt.do_directives = 1;
}
```

---

#### `PP_FLAG_HELP` = `"-help"`
**Purpose:** Help display flag

**Usage:** When provided, displays usage information instead of processing files

**Example:**
```bash
$ pp -help
```

**Output:**
```
Usage:
  pp [options] <file.c|file.h>

Options:
  -c     Remove comments (default if no flags)
  -d     Process directives (#include, #define, #ifdef/#endif) + macro expansion
  -all   Equivalent to -c -d
  -help  Show this help
```

**Behavior:**
- Overrides all other operations
- Exits immediately after displaying help
- Returns success exit code (0)

---

### Limit Definitions

```c
#define PP_MAX_IF_DEPTH 64
```

#### `PP_MAX_IF_DEPTH` = `64`
**Purpose:** Maximum nesting depth for conditional compilation directives

**Usage:** Limits how many `#ifdef` / `#ifndef` / `#if` blocks can be nested

**Example of Nesting:**
```c
#ifdef A              // depth 1
  #ifdef B            // depth 2
    #ifdef C          // depth 3
      // code
    #endif
  #endif
#endif
```

**Why Limit Nesting?**
1. **Prevent Stack Overflow:** Deeply nested conditionals consume stack space
2. **Detect Errors:** Extremely deep nesting usually indicates a bug
3. **Improve Readability:** Deep nesting is hard to understand
4. **Performance:** Fixed-size arrays are faster than dynamic allocation

**Value Choice - Why 64?**
- **Practical:** Real-world code rarely exceeds 10 levels
- **Safe:** 64 provides generous headroom
- **Efficient:** Small enough for stack allocation
- **Standard:** GCC uses similar limits

**How It's Used (Future Implementation):**
```c
typedef struct {
    int depth;
    int active[PP_MAX_IF_DEPTH];  // Fixed-size array
} if_stack_t;

void push_if_condition(if_stack_t *stack, int condition) {
    if (stack->depth >= PP_MAX_IF_DEPTH) {
        error("Maximum #if nesting depth exceeded");
        return;
    }
    stack->active[stack->depth++] = condition;
}
```

**Error Handling:**
When depth exceeds 64:
```
Error: Maximum #if nesting depth (64) exceeded
This usually indicates unbalanced #ifdef/#endif pairs
```

---

## Design Patterns and Principles

### 1. **Single Source of Truth**
All flag strings defined once in `pp_spec.h`
- Change `-c` to `--comments`? Edit one place
- Add new flag? Add one `#define`
- No string literals scattered in code

**Without spec.h:**
```c
// cli.c
if (strcmp(arg, "-c") == 0) { ... }

// help.c  
printf("  -c     Remove comments\n");

// docs.txt
Use -c to remove comments
```
Changing the flag requires editing 3+ files!

**With spec.h:**
```c
// cli.c
if (strcmp(arg, PP_FLAG_C) == 0) { ... }

// help.c
printf("  %s     Remove comments\n", PP_FLAG_C);
```
Change one `#define`, everything updates.

### 2. **Semantic Naming**
Names describe purpose, not value
- `PP_FLAG_HELP` (what it means)
- Not `MINUS_HELP_STRING` (how it's represented)

### 3. **Namespace Prefixing**
All constants start with `PP_`
- Prevents collisions with system headers
- Groups related constants visually
- Makes origin clear in code

### 4. **Header-Only Design**
No implementation file needed
- `#define` declarations don't need linking
- Included everywhere, no linker step
- Faster compilation (no separate compilation unit)

### 5. **Compile-Time Configuration**
Values are preprocessor macros
- No runtime overhead
- Compiler can optimize (constant folding)
- Values baked into binary

---

## Usage Throughout Codebase

### In CLI Module (cli.c)
```c
#include "../spec/pp_spec.h"

cli_options_t cli_parse(int argc, char **argv) {
    // ... 
    if (is_flag(a, PP_FLAG_HELP)) {
        opt.do_help = 1;
    } else if (is_flag(a, PP_FLAG_ALL)) {
        opt.do_comments = 1;
        opt.do_directives = 1;
    } else if (is_flag(a, PP_FLAG_C)) {
        opt.do_comments = 1;
    } else if (is_flag(a, PP_FLAG_D)) {
        opt.do_directives = 1;
    }
    // ...
}

void cli_print_help(const char *progname) {
    printf("  %s     Remove comments\n", PP_FLAG_C);
    printf("  %s     Process directives\n", PP_FLAG_D);
    printf("  %s   Equivalent to %s %s\n", 
           PP_FLAG_ALL, PP_FLAG_C, PP_FLAG_D);
    printf("  %s  Show this help\n", PP_FLAG_HELP);
}
```

### In Context Module (pp_context.h)
```c
#include "spec/pp_spec.h"

typedef struct {
    // ...
    // Future: if_stack with depth limit
    // int if_depth;
    // int if_stack[PP_MAX_IF_DEPTH];
} pp_context_t;
```

### In Future Directive Module
```c
#include "spec/pp_spec.h"

int directive_process_ifdef(pp_context_t *ctx, const char *condition) {
    if (ctx->if_depth >= PP_MAX_IF_DEPTH) {
        error(ctx, "Maximum #if nesting depth (%d) exceeded", 
              PP_MAX_IF_DEPTH);
        return -1;
    }
    // ... process ifdef ...
}
```

---

## Extending the Spec Module

Adding new specifications is straightforward:

### Example: Add Warning Limit
```c
#define PP_MAX_WARNINGS 100
```

Usage:
```c
typedef struct {
    int warning_count;
    // ...
} pp_context_t;

void emit_warning(pp_context_t *ctx, const char *msg) {
    if (ctx->warning_count >= PP_MAX_WARNINGS) {
        fprintf(stderr, "Too many warnings, suppressing further output\n");
        return;
    }
    ctx->warning_count++;
    fprintf(stderr, "Warning: %s\n", msg);
}
```

### Example: Add Macro Limit
```c
#define PP_MAX_MACROS 1024
```

Usage:
```c
typedef struct {
    char *names[PP_MAX_MACROS];
    char *values[PP_MAX_MACROS];
    int count;
} macro_table_t;
```

### Example: Add Include Depth Limit
```c
#define PP_MAX_INCLUDE_DEPTH 32
```

Prevents infinite include loops:
```c
// a.h
#include "b.h"

// b.h  
#include "a.h"  // Oops! Infinite loop
```

---

## Alternative Approaches Considered

### Why Not Enum?
```c
// Could use enum instead of #define
enum {
    PP_MAX_IF_DEPTH = 64,
    PP_MAX_INCLUDE_DEPTH = 32,
};
```

**Advantages of enum:**
- Type safety
- Debugger support

**Advantages of #define (current choice):**
- Works in preprocessor `#if` expressions
- Can be used in array size declarations (C89 compatible)
- Traditional for configuration constants

### Why Not const Variables?
```c
// Could use const instead of #define
static const char *PP_FLAG_C = "-c";
static const int PP_MAX_IF_DEPTH = 64;
```

**Advantages of const:**
- Type checking
- Linker visibility

**Advantages of #define (current choice):**
- True compile-time constants
- No runtime memory allocation
- Can be used in `#if` directives
- Traditional for string constants

### Why Not Config File?
```c
// Could load from pp.conf
max_if_depth=64
flag_comments=-c
```

**Advantages of config file:**
- Change without recompiling
- Per-user customization

**Advantages of #define (current choice):**
- No parsing overhead
- No file I/O at startup
- No failure modes (missing file, parse errors)
- Standard practice for C projects

---

## Testing Considerations

### Testing Limits
```c
void test_if_depth_limit() {
    pp_context_t ctx = {0};
    
    // Try to exceed limit
    for (int i = 0; i < PP_MAX_IF_DEPTH + 1; i++) {
        int result = push_ifdef(&ctx);
        if (i < PP_MAX_IF_DEPTH) {
            assert(result == 0);  // Should succeed
        } else {
            assert(result != 0);  // Should fail
        }
    }
}
```

### Testing Flag Recognition
```c
void test_all_flags_recognized() {
    cli_options_t opt;
    
    char *args[] = {"pp", PP_FLAG_C, "file.c"};
    opt = cli_parse(3, args);
    assert(opt.do_comments == 1);
    
    char *args2[] = {"pp", PP_FLAG_D, "file.c"};
    opt = cli_parse(3, args2);
    assert(opt.do_directives == 1);
}
```

---

## Version Control and Changes

When modifying `pp_spec.h`:

### Breaking Changes
Changing flag strings breaks existing scripts:
```c
// OLD
#define PP_FLAG_C "-c"

// NEW (BREAKING!)
#define PP_FLAG_C "--comments"
```

Users with scripts like `pp -c file.c` will break!

**Solution:** Maintain compatibility or version the preprocessor.

### Non-Breaking Changes
Adding new constants is safe:
```c
// Adding PP_MAX_WARNINGS doesn't break existing code
#define PP_MAX_WARNINGS 100
```

### Deprecation Strategy
```c
// Deprecated - use PP_MAX_IF_DEPTH instead
#define MAX_IF_DEPTH PP_MAX_IF_DEPTH

#warning "MAX_IF_DEPTH is deprecated, use PP_MAX_IF_DEPTH"
```

---

## Summary

The `spec` module provides centralized configuration and constants:

**Constants Defined:**
- **Flag Strings:** `-c`, `-d`, `-all`, `-help`
- **Limits:** `PP_MAX_IF_DEPTH` = 64

**Key Benefits:**
- **Maintainability:** Single source of truth
- **Consistency:** Same values everywhere
- **Flexibility:** Easy to change
- **Clarity:** Semantic names over magic numbers

**Design Principles:**
- Header-only design (no `.c` file)
- Preprocessor macros for compile-time constants
- Namespace prefixing (`PP_` prefix)
- Well-documented values

**Usage Pattern:**
```c
#include "spec/pp_spec.h"

// Use constants throughout code
if (strcmp(flag, PP_FLAG_C) == 0) { ... }
if (depth >= PP_MAX_IF_DEPTH) { ... }
```

The spec module may be small, but it plays a crucial role in maintaining a clean, consistent codebase. By centralizing configuration, it makes the preprocessor easier to understand, modify, and extend.
