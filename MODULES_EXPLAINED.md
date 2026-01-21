# C Preprocessor - Module Architecture Documentation

## Overview
This document provides a comprehensive explanation of the five core modules in the C preprocessor project: **buffer**, **cli**, **io**, **pp_core**, and **spec**. It explains how they work together to create a functional C preprocessor.

---

## Table of Contents
1. [Project Architecture](#project-architecture)
2. [Module Summaries](#module-summaries)
3. [Data Flow](#data-flow)
4. [Module Interactions](#module-interactions)
5. [Key Design Patterns](#key-design-patterns)
6. [Complete Processing Pipeline](#complete-processing-pipeline)
7. [Quick Reference](#quick-reference)

---

## Project Architecture

The C preprocessor is organized into modular components, each with a specific responsibility:

```
┌─────────────────────────────────────────────────────────────┐
│                      C PREPROCESSOR                          │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
   ┌────────┐           ┌─────────┐          ┌──────────┐
   │  SPEC  │◄──────────┤   CLI   │          │    IO    │
   └────────┘           └────┬────┘          └────┬─────┘
        │                    │                     │
        │                    │                ┌────┴─────┐
        │                    │                │  BUFFER  │
        │                    └──────────┬─────┤  (Core)  │
        │                               │     └────┬─────┘
        └───────────────┐               │          │
                        ▼               ▼          ▼
                   ┌─────────────────────────────────┐
                   │      PP_CORE                    │
                   │  (Preprocessing Engine)         │
                   └─────────────────────────────────┘
                               │
                    ┌──────────┼──────────┐
                    ▼          ▼          ▼
              ┌─────────┐  ┌──────┐  ┌────────┐
              │ TOKENS  │  │MACROS│  │DIRECTS │
              └─────────┘  └──────┘  └────────┘
                      (Future Modules)
```

---

## Module Summaries

### 1. **buffer** - Dynamic String Buffer
**Location:** `src/buffer/`
**Files:** `buffer.h`, `buffer.c`

**Purpose:** Provides a dynamic, resizable string buffer for efficient text manipulation.

**Core Data Structure:**
```c
typedef struct {
    char *data;    // Dynamically allocated character array
    long len;      // Current length (excluding null terminator)
    long cap;      // Total allocated capacity
} buffer_t;
```

**Main Functions:**
- `buffer_init()` - Initialize buffer with initial capacity
- `buffer_free()` - Release all allocated memory
- `buffer_append_char()` - Append single character
- `buffer_append_n()` - Append n bytes from string
- `buffer_append_str()` - Append null-terminated string

**Key Features:**
- **Automatic Growth:** Exponential growth strategy (doubles capacity)
- **Null Termination:** Always maintains C-string compatibility
- **Efficient:** Amortized O(1) append operations
- **Safe:** Defensive input validation

**Growth Strategy:**
- Initial capacity: 64 bytes
- Growth: Doubles when capacity exceeded
- Example: 64 → 128 → 256 → 512 → 1024...

**Why It's Critical:**
- Foundation for all text operations
- Used by IO, PP_CORE, and other modules
- Eliminates manual memory management
- Prevents buffer overflow errors

**Dependencies:** Standard C library only
**Used By:** io, pp_core, main.c

📖 **[Full buffer Documentation](modules-template-project-main/src/buffer/README.md)**

---

### 2. **spec** - Specifications and Constants
**Location:** `src/spec/`
**Files:** `pp_spec.h`

**Purpose:** Central repository for constants, configuration values, and specifications.

**What It Defines:**
- Command-line flag strings (`"-c"`, `"-d"`, `"-all"`, `"-help"`)
- System limits (`PP_MAX_IF_DEPTH = 64`)
- Configuration parameters

**Why It Exists:**
- **Single Source of Truth:** Change a flag in one place
- **Maintainability:** No magic numbers scattered in code
- **Consistency:** Same values everywhere

**Key Constants:**
```c
#define PP_FLAG_C    "-c"      // Comment removal
#define PP_FLAG_D    "-d"      // Directive processing
#define PP_FLAG_ALL  "-all"    // Both operations
#define PP_FLAG_HELP "-help"   // Help display

#define PP_MAX_IF_DEPTH 64     // Max #ifdef nesting
```

**Dependencies:** None (foundation module)
**Used By:** cli, pp_core

📖 **[Full spec Documentation](src/spec/README.md)**

---

### 3. **cli** - Command-Line Interface
**Location:** `src/cli/`
**Files:** `cli.h`, `cli.c`

**Purpose:** Parses command-line arguments and provides user help.

**Core Data Structure:**
```c
typedef struct {
    int do_comments;    // -c flag
    int do_directives;  // -d flag
    int do_help;        // -help flag
} cli_options_t;
```

**Main Functions:**
- `cli_parse()` - Parses argc/argv into options structure
- `cli_print_help()` - Displays usage information

**Key Behavior:**
- **Default:** If no flags provided, assumes `-c` (comment removal)
- **Flag Combining:** `-c -d` or `-all` both work
- **Smart Parsing:** Ignores non-flag arguments (filenames)

**Example:**
```bash
$ pp -c myfile.c          # Remove comments only
$ pp -d myfile.c          # Process directives only
$ pp -all myfile.c        # Both operations
$ pp myfile.c             # Default: same as -c
$ pp -help                # Show help
```

**Dependencies:** spec (for flag constants)
**Used By:** main.c, pp_core (via context)

📖 **[Full cli Documentation](src/cli/README.md)**

---

### 4. **io** - Input/Output Operations
**Location:** `src/io/`
**Files:** `io.h`, `io.c`

**Purpose:** Handles all file reading and writing operations.

**Main Functions:**
- `io_read_file(path, buffer)` - Read file into memory buffer
- `io_write_file(path, buffer)` - Write buffer to file
- `io_make_output_name(input, output)` - Generate output filename

**Key Features:**
- **Binary Mode:** Preserves exact byte content
- **Chunked Reading:** 4KB chunks for efficiency
- **Smart Naming:** `example.c` → `example_pp.c`
- **Error Handling:** Distinct codes for different failures

**Example:**
```c
buffer_t content;
buffer_init(&content);

// Read source file
io_read_file("mycode.c", &content);

// Generate output name
buffer_t outname;
io_make_output_name("mycode.c", &outname);
// outname now contains "mycode_pp.c"

// Write processed content
io_write_file(outname.data, &content);
```

**Dependencies:** buffer (for buffer_t)
**Used By:** main.c

📖 **[Full io Documentation](src/io/README.md)**

---

### 5. **pp_core** - Preprocessing Engine
**Location:** `src/pp_core/`
**Files:** `pp_core.h`, `pp_core.c`, `pp_context.h`

**Purpose:** Main preprocessing logic - orchestrates the transformation pipeline.

**Core Data Structures:**
```c
// Maintains state during preprocessing
typedef struct {
    cli_options_t opt;           // What to do
    const char *current_file;    // Where we are
    int current_line;            // Which line
    int error_count;             // How many errors
    // Future: macro_table_t macros;
    // Future: if_stack_t ifs;
} pp_context_t;
```

**Main Function:**
```c
int pp_run(pp_context_t *ctx, 
           const buffer_t *input, 
           buffer_t *output,
           const char *base_dir);
```

**Current Algorithm:**
1. Scan input character by character
2. Identify line boundaries (`\n` characters)
3. Track line numbers (1-based, human-readable)
4. Copy each line to output (currently pass-through)
5. Handle last line even without `\n`

**Future Pipeline:**
```
Input Line
    ↓
Tokenize (break into tokens)
    ↓
Remove Comments (if -c)
    ↓
Process Directives (if -d)
    ├→ #include (insert file)
    ├→ #define (store macro)
    ├→ #ifdef (track conditional)
    └→ #undef (remove macro)
    ↓
Expand Macros (if -d)
    ↓
Output Line
```

**Dependencies:** cli (for options), spec (for constants), buffer
**Used By:** main.c

📖 **[Full pp_core Documentation](src/pp_core/README.md)**

---

## Data Flow

### Complete Processing Flow

```
┌──────────────────────────────────────────────────────────────┐
│ 1. USER INPUT                                                 │
└──────────────────────────────────────────────────────────────┘
                            │
           $ pp -c -d myfile.c
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│ 2. CLI PARSING (cli module)                                   │
│    Parse: argc, argv                                          │
│    Output: cli_options_t {do_comments=1, do_directives=1}    │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│ 3. FILE INPUT (io module)                                     │
│    io_read_file("myfile.c", &input_buffer)                   │
│    Output: buffer_t with file contents                       │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│ 4. CONTEXT SETUP (pp_core)                                    │
│    pp_context_t ctx = {                                       │
│        .opt = cli_options,                                    │
│        .current_file = "myfile.c",                           │
│        .current_line = 0,                                     │
│        .error_count = 0                                       │
│    }                                                          │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│ 5. PREPROCESSING (pp_core)                                    │
│    pp_run(&ctx, &input_buffer, &output_buffer, ".")         │
│                                                               │
│    For each line:                                            │
│      - Track line number                                     │
│      - Process based on options                              │
│      - Write to output buffer                                │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│ 6. OUTPUT FILENAME (io module)                                │
│    io_make_output_name("myfile.c", &outname)                │
│    Result: "myfile_pp.c"                                     │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│ 7. FILE OUTPUT (io module)                                    │
│    io_write_file("myfile_pp.c", &output_buffer)             │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│ 8. CLEANUP & EXIT                                             │
│    buffer_free() all buffers                                  │
│    return (ctx.error_count > 0) ? 1 : 0;                     │
└──────────────────────────────────────────────────────────────┘
```

---

## Module Interactions

### Dependency Graph

```
        buffer (no dependencies - foundation)
          │
          ├──► io (depends on: buffer)
          │
          ├──► pp_core (depends on: buffer, cli, spec)
          │
          └──► main.c (depends on: buffer, cli, io, pp_core)

        spec (no dependencies - configuration)
          │
          ├──► cli (depends on: spec)
          │
          └──► pp_core (depends on: spec)
```

### Interaction Patterns

#### 1. **main.c orchestrates everything:**

```c
int main(int argc, char **argv) {
    // Step 1: Parse CLI
    cli_options_t opt = cli_parse(argc, argv);
    
    // Step 2: Get input filename
    const char *in_path = get_input_path(argc, argv);
    
    // Step 3: Read input file
    buffer_t in;
    io_read_file(in_path, &in);
    
    // Step 4: Setup context
    pp_context_t ctx;
    ctx.opt = opt;
    ctx.current_file = in_path;
    ctx.error_count = 0;
    
    // Step 5: Process
    buffer_t out;
    pp_run(&ctx, &in, &out, ".");
    
    // Step 6: Write output
    buffer_t out_name;
    io_make_output_name(in_path, &out_name);
    io_write_file(out_name.data, &out);
    
    // Step 7: Cleanup
    buffer_free(&in);
    buffer_free(&out);
    buffer_free(&out_name);
    
    return (ctx.error_count > 0) ? 1 : 0;
}
```

#### 2. **spec provides constants to all:**

```c
// In cli.c
if (is_flag(arg, PP_FLAG_C)) { ... }

// In pp_core.c (future)
if (depth >= PP_MAX_IF_DEPTH) { ... }
```

#### 3. **cli output feeds pp_core:**

```c
cli_options_t opt = cli_parse(argc, argv);
// opt is copied into pp_context_t
ctx.opt = opt;
// pp_core uses ctx.opt to decide what to process
```

#### 4. **io operates independently:**

```c
// IO doesn't know about preprocessing
// Just reads and writes buffers
io_read_file(path, &buffer);   // Read
io_write_file(path, &buffer);  // Write
```

---

## Key Design Patterns

### 1. **Separation of Concerns**
Each module has ONE clear responsibility:
- **spec:** Configuration
- **cli:** User interface
- **io:** File operations
- **pp_core:** Preprocessing logic

### 2. **Context Object Pattern**
`pp_context_t` carries all state:
- Options (what to do)
- Location (where we are)
- Errors (what went wrong)
- Future: macros, includes, conditionals

### 3. **Buffer-Based Processing**
`buffer_t` decouples I/O from processing:
- Read file → buffer
- Process buffer → buffer
- Write buffer → file
- Enables testing without filesystem

### 4. **Single Source of Truth**
Constants in `spec/pp_spec.h`:
- Change flag string once
- Updates everywhere automatically

### 5. **Pipeline Architecture**
Data flows through stages:
```
Input → Read → Parse → Process → Write → Output
```

### 6. **Error Accumulation**
Don't stop at first error:
```c
ctx->error_count++;  // Track errors
return (error_count > 0) ? 1 : 0;  // Exit with error status
```

---

## Complete Processing Pipeline

### Current Implementation (Simplified)

```
┌────────────┐
│  myfile.c  │
└─────┬──────┘
      │ io_read_file()
      ▼
┌────────────────┐
│  Input Buffer  │
│  "int x;\n     │
│   return x;\n" │
└────────┬───────┘
         │ pp_run()
         ▼
┌─────────────────────┐
│   For each line:    │
│   1. Find \n        │
│   2. Count line     │
│   3. Copy to output │
└────────┬────────────┘
         │
         ▼
┌──────────────────┐
│  Output Buffer   │
│  "int x;\n       │
│   return x;\n"   │
└────────┬─────────┘
         │ io_write_file()
         ▼
┌──────────────┐
│myfile_pp.c   │
└──────────────┘
```

### Future Implementation (Full Preprocessing)

```
┌────────────┐
│  myfile.c  │
└─────┬──────┘
      │
      ▼
┌───────────────────┐
│  Input Buffer     │
│  "// comment\n    │
│   #define MAX 10\n│
│   int x = MAX;\n" │
└────────┬──────────┘
         │
         ▼
┌─────────────────────────────┐
│  pp_run() - Line Loop       │
│                             │
│  Line 1: "// comment\n"     │
│    ├─ Remove comment (opt)  │
│    └─ Output: "\n"          │
│                             │
│  Line 2: "#define MAX 10\n" │
│    ├─ Detect directive      │
│    ├─ Store macro: MAX=10   │
│    └─ Output: (nothing)     │
│                             │
│  Line 3: "int x = MAX;\n"   │
│    ├─ Expand macro MAX→10   │
│    └─ Output: "int x = 10;\n"
└────────┬────────────────────┘
         │
         ▼
┌──────────────────┐
│  Output Buffer   │
│  "\n             │
│   int x = 10;\n" │
└────────┬─────────┘
         │
         ▼
┌──────────────┐
│myfile_pp.c   │
└──────────────┘
```

---

## Quick Reference

### Module Responsibilities

| Module | What It Does | Key Functions |
|--------|-------------|---------------|
| **buffer** | Dynamic string management | `buffer_init()`, `buffer_free()`, `buffer_append_*()` |
| **spec** | Defines constants | (header-only) |
| **cli** | Parses arguments | `cli_parse()`, `cli_print_help()` |
| **io** | File operations | `io_read_file()`, `io_write_file()`, `io_make_output_name()` |
| **pp_core** | Preprocessing | `pp_run()` |

### Data Structures

| Structure | Purpose | Module |
|-----------|---------|--------|
| `cli_options_t` | Command-line flags | cli |
| `buffer_t` | Dynamic string buffer | buffer |
| `pp_context_t` | Preprocessing state | pp_core |

### Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `PP_FLAG_C` | `"-c"` | Comment removal flag |
| `PP_FLAG_D` | `"-d"` | Directive processing flag |
| `PP_FLAG_ALL` | `"-all"` | Combined operations flag |
| `PP_FLAG_HELP` | `"-help"` | Help display flag |
| `PP_MAX_IF_DEPTH` | `64` | Max #ifdef nesting |

### Function Return Codes

| Function | 0 | 1 | 2 | 3 |
|----------|---|---|---|---|
| `buffer_append_*()` | Success | NULL/invalid args | - | - |
| `io_read_file()` | Success | Can't open | Buffer fail | - |
| `io_write_file()` | Success | Can't open | - | - |
| `pp_run()` | Success | Invalid args | Buffer fail (loop) | Buffer fail (last) |

---

## Testing Strategy

Each module can be tested independently:

### Testing cli
```c
void test_cli() {
    char *args[] = {"pp", "-c", "file.c"};
    cli_options_t opt = cli_parse(3, args);
    assert(opt.do_comments == 1);
    assert(opt.do_directives == 0);
}
```

### Testing io
```c
void test_io() {
    buffer_t buf;
    buffer_init(&buf);
    buffer_append_str(&buf, "test");
    io_write_file("test.txt", &buf);
    
    buffer_t read;
    buffer_init(&read);
    io_read_file("test.txt", &read);
    assert(strcmp(read.data, "test") == 0);
}
```

### Testing pp_core
```c
void test_pp_core() {
    pp_context_t ctx = {0};
    buffer_t in, out;
    buffer_init(&in);
    buffer_init(&out);
    
    buffer_append_str(&in, "int x;\n");
    pp_run(&ctx, &in, &out, ".");
    
    assert(ctx.current_line == 1);
    assert(out.len == 7);
}
```

---

## Future Development

### Planned Features

1. **Comment Removal** (cli: `-c`)
   - Single-line `//` comments
   - Multi-line `/* */` comments
   - Nested comment handling

2. **Directive Processing** (cli: `-d`)
   - `#include` - File inclusion
   - `#define` - Macro definitions
   - `#ifdef/#ifndef` - Conditional compilation
   - `#if/#elif/#else/#endif` - Complex conditionals

3. **Macro Expansion**
   - Object-like macros: `#define MAX 100`
   - Function-like macros: `#define SQR(x) ((x)*(x))`
   - Predefined macros: `__FILE__`, `__LINE__`

4. **Error Handling**
   - Detailed error messages
   - Line/column reporting
   - Multiple error collection

### Extension Points

Each module has clear extension points:

**In spec:**
```c
#define PP_MAX_MACROS 1024
#define PP_MAX_INCLUDE_DEPTH 32
```

**In cli:**
```c
cli_options_t opt = {
    .do_comments = 0,
    .do_directives = 0,
    .verbose = 0,        // NEW
    .warnings = 1,        // NEW
};
```

**In pp_context:**
```c
typedef struct {
    // ... existing fields ...
    macro_table_t macros;     // NEW: Store #defines
    if_stack_t ifs;           // NEW: Track #ifdef nesting
    include_stack_t includes; // NEW: Track #include chain
} pp_context_t;
```

---

## Summary

This C preprocessor is built from five well-designed modules:

1. **buffer** - The foundation (dynamic string management)
2. **spec** - The configuration (constants and limits)
3. **cli** - The interface (user interaction)
4. **io** - The transport (file operations)
5. **pp_core** - The engine (processing logic)

Each module:
- Has a single, clear responsibility
- Operates independently
- Uses well-defined interfaces
- Is easy to test and extend

Together they form a clean, maintainable architecture ready for feature expansion.

---

## Further Reading

For detailed information on each module:
- 📖 [Buffer Module Documentation](modules-template-project-main/src/buffer/README.md)
- 📖 [CLI Module Documentation](modules-template-project-main/src/cli/README.md)
- 📖 [IO Module Documentation](modules-template-project-main/src/io/README.md)
- 📖 [PP_CORE Module Documentation](modules-template-project-main/src/pp_core/README.md)
- 📖 [SPEC Module Documentation](modules-template-project-main/src/spec/README.md)
