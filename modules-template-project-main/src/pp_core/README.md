# PP_CORE Module Documentation

## Overview
The `pp_core` (Preprocessor Core) module is the heart of the C preprocessor. It orchestrates the main preprocessing pipeline, processing input source code line by line and coordinating with other modules (comments, directives, macros) to produce preprocessed output.

## Files
- **pp_core.h** - Public interface for the preprocessing engine
- **pp_core.c** - Core preprocessing implementation
- **pp_context.h** - Context structure that maintains preprocessing state
- **CMakeLists.txt** - Build configuration for the pp_core module

## Core Data Structures

### `pp_context_t` (in pp_context.h)

The preprocessing context maintains all state during a preprocessing run.

```c
typedef struct {
    cli_options_t opt;           // Command-line options
    
    const char *current_file;    // Currently processing file
    int current_line;            // Current line number (1-based)
    
    int error_count;             // Number of errors encountered
    
    // Future extensions:
    // macro_table_t macros;     // Macro definitions
    // if_stack_t ifs;           // #ifdef/#endif nesting stack
} pp_context_t;
```

**Field Descriptions:**

#### `cli_options_t opt`
- **Type:** Structure containing parsed command-line flags
- **Purpose:** Controls which preprocessing features are enabled
- **Source:** Copied from CLI parser in main()
- **Fields:**
  - `do_comments`: Enable comment removal
  - `do_directives`: Enable directive processing
  - `do_help`: Help mode (not used during processing)

#### `const char *current_file`
- **Type:** Pointer to filename string
- **Purpose:** Track which file is being processed
- **Usage:** Error reporting, include path resolution
- **Lifecycle:** Points to string owned by caller (usually argv)

#### `int current_line`
- **Type:** Integer line counter
- **Purpose:** Track current position in source file
- **Indexing:** 1-based (human-readable line numbers)
- **Updates:** Incremented after each `\n` character
- **Usage:** Error messages, #line directive support

#### `int error_count`
- **Type:** Integer error accumulator
- **Purpose:** Track total errors during preprocessing
- **Behavior:** Incremented when errors occur
- **Usage:** Determines exit status of program
- **Return Value:** Main returns non-zero if error_count > 0

#### Future Fields (Commented Out)
```c
// macro_table_t macros;  // Will store #define'd macros
// if_stack_t ifs;        // Will track #ifdef/#endif nesting
```

These indicate planned features not yet implemented.

---

## Core Functions

### `int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output, const char *base_dir)`

**Purpose:** Main preprocessing engine that processes input and generates output.

**Parameters:**
- `ctx`: Preprocessing context (state, options, error tracking)
- `input`: Buffer containing source code to process
- `output`: Buffer to receive preprocessed code
- `base_dir`: Base directory for resolving includes (currently unused)

**Returns:**
- `0`: Success - preprocessing completed without errors
- `1`: Invalid arguments (NULL pointers)
- `2`: Buffer append failed during line processing
- `3`: Buffer append failed for last line

**Algorithm Overview:**

The function implements a **line-based processing model**:

```
For each line in input:
    1. Identify line boundaries (terminated by \n)
    2. Update line counter
    3. Process line (currently: copy unchanged)
    4. Append processed line to output
Handle last line if file doesn't end with \n
```

**Detailed Implementation:**

```c
int pp_run(pp_context_t *ctx, const buffer_t *input, 
           buffer_t *output, const char *base_dir)
{
    (void) base_dir;  // Unused parameter (future: for #include)
    long i = 0;
    long line_start = 0;

    // Validate inputs
    if (!ctx || !input || !output || !input->data) {
        return 1;
    }

    // Initialize line counter (0-based during processing)
    ctx->current_line = 0;

    // Main processing loop
    while (i < input->len) {
        if (input->data[i] == '\n') {
            // Found end of line
            long line_len = (i - line_start) + 1;  // Include \n
            ctx->current_line++;
            
            // Process and copy line
            if (buffer_append_n(output, input->data + line_start, 
                               line_len) != 0) {
                ctx->error_count++;
                return 2;
            }
            
            i++;
            line_start = i;  // Start of next line
        } else {
            i++;
        }
    }

    // Handle last line without \n
    if (line_start < input->len) {
        long line_len = input->len - line_start;
        ctx->current_line++;
        
        if (buffer_append_n(output, input->data + line_start, 
                           line_len) != 0) {
            ctx->error_count++;
            return 3;
        }
    }

    return 0;
}
```

---

## Algorithm Deep Dive

### Line Detection Strategy

The algorithm uses **character-by-character scanning** to find line boundaries:

**Variables:**
- `i`: Current character index (reading position)
- `line_start`: Index where current line begins
- `line_len`: Number of characters in current line

**Line Boundary Detection:**
```
Line 1: "int x;\n"     line_start=0,  i=7 (at \n), len=8
Line 2: "return x;\n"  line_start=8,  i=17 (at \n), len=10
Line 3: "}"            line_start=18, i=19 (EOF), len=1
```

**Why This Approach?**
1. **Simple:** Easy to understand and debug
2. **Portable:** Works with any line ending in the buffer
3. **Flexible:** Easy to add per-line processing later
4. **Accurate:** Preserves exact line structure including `\n`

### Line Counting Philosophy

**Human-Readable Line Numbers:**
```c
ctx->current_line = 0;  // Start at 0

// After first \n encountered:
ctx->current_line++;    // Now = 1

// After second \n:
ctx->current_line++;    // Now = 2
```

This matches how editors and compilers report line numbers (1-based).

### Last Line Handling

**Problem:** C files may or may not end with `\n`

**Solution:** After main loop, check for remaining characters:

```c
if (line_start < input->len) {
    // There's content after the last \n (or no \n at all)
    long line_len = input->len - line_start;
    ctx->current_line++;
    // Process this last line
}
```

**Examples:**

File with final newline:
```c
"int x;\n"
```
- Main loop processes entire line
- `line_start` equals `input->len` after loop
- Last line handler skipped (line_start NOT < input->len)

File without final newline:
```c
"int x;"
```
- Main loop never finds `\n`
- `line_start` = 0, `input->len` = 6
- Last line handler processes "int x;"

### Error Handling

**Buffer Append Failures:**
```c
if (buffer_append_n(output, ...) != 0) {
    ctx->error_count++;  // Track error
    return 2;            // Distinct error code
}
```

**Why Different Return Codes?**
- Return `2`: Error during main loop
- Return `3`: Error during last line
- Helps diagnose where failure occurred

**Error Recovery:**
- Currently: Immediate abort on first error
- Future: Could continue processing to find more errors

---

## Processing Pipeline (Current & Future)

### Current Implementation (Pass-Through)

```
┌─────────┐
│  Input  │
│  Line   │
└────┬────┘
     │
     ↓
┌─────────────────┐
│  Copy Directly  │ ← Currently just copies
│  to Output      │
└────┬────────────┘
     │
     ↓
┌─────────┐
│ Output  │
│  Line   │
└─────────┘
```

### Future Implementation (Full Preprocessing)

```
┌─────────┐
│  Input  │
│  Line   │
└────┬────┘
     │
     ↓
┌─────────────────┐
│ Token Scanner   │ ← Break line into tokens
└────┬────────────┘
     │
     ↓
┌─────────────────┐
│ Comment Handler │ ← if (opt.do_comments) remove
└────┬────────────┘
     │
     ↓
┌──────────────────┐
│ Directive Check  │ ← if (opt.do_directives)
│  #include        │    - Process includes
│  #define         │    - Store macros
│  #ifdef          │    - Track conditionals
└────┬─────────────┘
     │
     ↓
┌──────────────────┐
│ Macro Expansion  │ ← Replace macro uses
└────┬─────────────┘
     │
     ↓
┌─────────┐
│ Output  │
│  Line   │
└─────────┘
```

---

## Integration Points

### Dependencies
- **cli/cli.h**: For `cli_options_t` structure
- **spec/pp_spec.h**: For constants (future use)
- **buffer/buffer.h**: For `buffer_t` operations

### Used By
- **main.c**: Calls `pp_run()` after reading input

**Example from main.c:**
```c
pp_context_t ctx;
ctx.opt = opt;                    // From CLI parser
ctx.current_file = in_path;       // Input filename
ctx.current_line = 0;             // Will be updated
ctx.error_count = 0;              // No errors yet

// Run preprocessor
pp_run(&ctx, &in, &out, ".");

// Check results
if (ctx.error_count > 0) {
    fprintf(stderr, "%d errors during preprocessing\n", 
            ctx.error_count);
    return 1;
}
```

---

## Design Patterns and Principles

### 1. **Context Object Pattern**
- `pp_context_t` centralizes all state
- Passed to all processing functions
- Easy to extend with new fields
- Enables stateful processing

### 2. **Line-Based Processing**
- Natural unit for source code
- Matches how humans read code
- Simplifies error reporting (line numbers)
- Easy to parallelize in future (independent lines)

### 3. **Pipeline Architecture**
- pp_core orchestrates the flow
- Other modules (comments, directives, macros) plug in
- Each stage can be enabled/disabled via options
- Clear separation of concerns

### 4. **Error Accumulation**
- Don't stop at first error
- Collect multiple errors for better user feedback
- Error count determines exit status

### 5. **Fail-Fast Validation**
- Check all inputs upfront
- Prevent crashes from NULL pointers
- Clear error codes for debugging

---

## Future Extensions

The codebase shows clear extension points:

### 1. Comment Processing
```c
// In the line processing loop:
if (ctx->opt.do_comments) {
    line = comments_remove(line, &line_len);
}
```

### 2. Directive Processing
```c
if (ctx->opt.do_directives) {
    if (is_directive(line)) {
        directive_process(ctx, line, output);
        continue;  // Don't copy directive itself
    }
}
```

### 3. Macro Expansion
```c
if (ctx->opt.do_directives) {  // Macros need directives
    line = macro_expand(ctx, line, &line_len);
}
```

### 4. Include Processing
```c
if (is_include_directive(line)) {
    char *included_path = resolve_include(line, base_dir);
    buffer_t included_content;
    io_read_file(included_path, &included_content);
    pp_run(ctx, &included_content, output, base_dir);  // Recursive
}
```

### 5. Conditional Compilation
```c
// Track #ifdef stack
typedef struct {
    int depth;
    int active[PP_MAX_IF_DEPTH];  // Is this level active?
} if_stack_t;

// In context:
// if_stack_t ifs;

// When processing:
if (is_ifdef(line)) {
    push_if_condition(ctx->ifs, evaluate_condition(line));
}
if (!is_active_level(ctx->ifs)) {
    continue;  // Skip lines in inactive #ifdef blocks
}
```

---

## Testing Strategies

### Unit Testing pp_run()

```c
void test_empty_input() {
    pp_context_t ctx = {0};
    buffer_t in, out;
    buffer_init(&in);
    buffer_init(&out);
    
    int result = pp_run(&ctx, &in, &out, ".");
    assert(result == 0);
    assert(out.len == 0);
    assert(ctx.current_line == 0);
}

void test_single_line_with_newline() {
    pp_context_t ctx = {0};
    buffer_t in, out;
    buffer_init(&in);
    buffer_init(&out);
    
    buffer_append_str(&in, "int x;\n");
    
    int result = pp_run(&ctx, &in, &out, ".");
    assert(result == 0);
    assert(ctx.current_line == 1);
    assert(out.len == 7);  // "int x;\n"
}

void test_multiple_lines() {
    pp_context_t ctx = {0};
    buffer_t in, out;
    buffer_init(&in);
    buffer_init(&out);
    
    buffer_append_str(&in, "line 1\nline 2\nline 3\n");
    
    pp_run(&ctx, &in, &out, ".");
    assert(ctx.current_line == 3);
}

void test_last_line_no_newline() {
    pp_context_t ctx = {0};
    buffer_t in, out;
    buffer_init(&in);
    buffer_init(&out);
    
    buffer_append_str(&in, "no newline");
    
    pp_run(&ctx, &in, &out, ".");
    assert(ctx.current_line == 1);
    assert(out.len == 10);
}

void test_null_input() {
    pp_context_t ctx = {0};
    buffer_t out;
    buffer_init(&out);
    
    int result = pp_run(&ctx, NULL, &out, ".");
    assert(result == 1);  // Should fail validation
}
```

---

## Performance Characteristics

### Time Complexity
- **O(n)** where n = input size in bytes
- Single pass through input
- Each character examined once
- Line processing is O(1) per line (currently)

### Space Complexity
- **O(1)** additional space beyond input/output buffers
- No dynamic allocations during processing
- Context structure is stack-allocated
- Output buffer grows to match input size

### Scalability
- Can process arbitrarily large files
- Memory limited by buffer size, not algorithm
- Could be adapted for streaming (process chunks)

---

## Common Pitfalls and Solutions

### 1. Off-by-One Errors in Line Counting
**Problem:** Should line numbers be 0-based or 1-based?
**Solution:** Context uses 0 during processing, increments to 1-based for user-facing output

### 2. Missing Last Line
**Problem:** File without final `\n` could be ignored
**Solution:** Explicit check after main loop handles this case

### 3. Line Length Calculation
**Problem:** Should `\n` be included in line length?
**Solution:** Yes - include it to preserve exact file structure

### 4. Buffer Overflow
**Problem:** What if output buffer can't grow?
**Solution:** Check buffer_append return codes, increment error_count

---

## Summary

The `pp_core` module provides the foundation for C preprocessing:

**Current State:**
- Line-by-line input scanning
- Accurate line counting (1-based, human-readable)
- Pass-through copying (no transformations yet)
- Error tracking and reporting

**Architecture:**
- Clean separation: pp_core orchestrates, other modules transform
- Context pattern: Centralized state management
- Pipeline-ready: Easy to insert processing stages
- Extensible: Clear hooks for future features

**Key Strengths:**
- **Simple:** Easy to understand current implementation
- **Correct:** Handles edge cases (last line, empty files)
- **Robust:** Validates inputs, tracks errors
- **Extensible:** Commented placeholders show future direction

The module serves as a solid foundation for building a complete C preprocessor, with a clear path from current pass-through implementation to full preprocessing capabilities.
