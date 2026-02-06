# P1PP User Manual
## C Preprocessor Practice Tool

**Version:** 1.0  
**Course:** Compilers - Practice 1  
**Last Updated:** January 2026

---

## Table of Contents
1. [Introduction](#1-introduction)
2. [Installation](#2-installation)
3. [Quick Start](#3-quick-start)
4. [Command-Line Usage](#4-command-line-usage)
5. [Features](#5-features)
6. [Examples](#6-examples)
7. [Input/Output Files](#7-inputoutput-files)
8. [Error Handling](#8-error-handling)
9. [Limitations](#9-limitations)
10. [Troubleshooting](#10-troubleshooting)

---

## 1. Introduction

**P1PP** is a modular C preprocessor tool designed to perform essential preprocessing tasks on C source files. It is part of the Compilers course practice assignments and implements a subset of standard C preprocessor functionality.

### What Does P1PP Do?

P1PP processes C source files (`.c` and `.h`) by:
- Removing comments (`//` and `/* */`)
- Processing preprocessor directives (`#include`, `#define`, `#ifdef`/`#endif`)
- Expanding macro definitions
- Generating a preprocessed output file

### Key Benefits
- **Modular Design**: Clean separation of concerns across modules
- **Educational**: Helps understand compiler preprocessing stages
- **Configurable**: Select which preprocessing operations to apply
- **Accurate**: Preserves line numbers for debugging

---

## 2. Installation

### Prerequisites

- **MSYS2** with UCRT64 toolchain
- **CMake** (version 3.10 or higher)
- **GCC** compiler
- **Git** (for version control)

### Build Instructions

1. **Clone the repository**:
   ```bash
   git clone https://github.com/CarlosGarcia27UPF/GK_Compilers_ModulesProject.git
   cd GK_Compilers_ModulesProject/modules-template-project-main
   ```

2. **Create build directory**:
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**:
   ```bash
   cmake ..
   ```

4. **Build the project**:
   ```bash
   cmake --build .
   ```

5. **Locate the executable**:
   - The executable will be in `build/` directory
   - Named `modules_template_main.exe` (Windows) or `modules_template_main` (Linux/Mac)

---

## 3. Quick Start

### Basic Usage

Process a C file with default settings (removes comments only):
```bash
./modules_template_main input-example.c
```

This creates an output file: `input-example_pp.c`

### Process with All Features

To apply all preprocessing (comments, directives, macros):
```bash
./modules_template_main -all input-example.c
```

### Get Help

Display usage information:
```bash
./modules_template_main -help
```

---

## 4. Command-Line Usage

### Syntax

```
preprocessor [OPTIONS] <input_file>
```

### Options

| Option | Description | Default |
|--------|-------------|---------|
| `-c` | Remove comments from the source file | **Yes** (if no flags) |
| `-d` | Process preprocessor directives (#include, #define, #ifdef) | No |
| `-all` | Apply all preprocessing (equivalent to `-c -d`) | No |
| `-help` | Display help message and exit | - |

### Important Notes

- **Order doesn't matter**: `-c -d` is the same as `-d -c`
- **Default behavior**: If no flags are provided, `-c` is applied automatically
- **Help overrides**: If `-help` is present, other flags are ignored
- **File required**: You must specify an input file (except with `-help`)

### Supported File Types

- `.c` files (C source files)
- `.h` files (C header files)

---

## 5. Features

### 5.1 Comment Removal (`-c`)

Removes both single-line and multi-line comments while preserving code structure.

#### What Gets Removed:
- **Single-line comments**: `// comment text`
- **Multi-line comments**: `/* comment text */`

#### Behavior:
- Comments are replaced with a single space
- All newline characters are preserved
- Empty lines are not eliminated
- Line numbers remain accurate

**Example:**
```c
// Input
int x = 5; // initialize x
/* This is a 
   multi-line comment */
int y = 10;

// Output
int x = 5;  
  
int y = 10;
```

---

### 5.2 Directive Processing (`-d`)

Handles three essential preprocessor directives.

#### 5.2.1 `#include "file"`

Includes the contents of another file.

**Syntax:**
```c
#include "filename.h"
```

**Behavior:**
- Only local includes (with quotes) are supported
- System includes (`<stdio.h>`) are not processed
- The entire content of the included file replaces the directive
- Included files are also preprocessed recursively
- Relative paths are resolved from the input file's directory

**Example:**
```c
// main.c
#include "header.h"
int main() { return 0; }

// header.h
#define MAX 100

// Output (main_pp.c)
#define MAX 100
int main() { return 0; }
```

#### 5.2.2 `#define name value`

Defines macros that can be expanded in the code.

**Syntax:**
```c
#define MACRO_NAME replacement_text
```

**Behavior:**
- Stores the macro in an internal table
- The `#define` line itself does not appear in output
- Macro names are replaced with their values in subsequent code
- Only whole-word matches are replaced (not substrings)

**Example:**
```c
// Input
#define MAX 100
#define PI 3.14159
int array[MAX];

// Output
int array[100];
```

#### 5.2.3 `#ifdef name / #endif`

Conditionally includes or excludes code blocks.

**Syntax:**
```c
#ifdef MACRO_NAME
    // code to include if MACRO_NAME is defined
#endif
```

**Behavior:**
- Code between `#ifdef` and `#endif` is included only if the macro exists
- Nested conditionals are supported
- Maximum nesting depth is defined by the implementation

**Example:**
```c
// Input
#define DEBUG
#ifdef DEBUG
    printf("Debug mode\n");
#endif
#ifdef RELEASE
    printf("Release mode\n");
#endif

// Output
    printf("Debug mode\n");
```

---

### 5.3 Macro Expansion

When directives are enabled (`-d`), macros defined with `#define` are automatically expanded.

**Rules:**
- Only complete identifiers are replaced
- String literals are not affected
- Tokens inside comments are not expanded (comments removed first)

---

## 6. Examples

### Example 1: Remove Comments Only

**Command:**
```bash
./modules_template_main input-example.c
```
or
```bash
./modules_template_main -c input-example.c
```

**Input (`input-example.c`):**
```c
// This is a comment
int main() {
    int x = 5; /* inline comment */
    return 0;
}
```

**Output (`input-example_pp.c`):**
```c
 
int main() {
    int x = 5;  
    return 0;
}
```

---

### Example 2: Process All Directives

**Command:**
```bash
./modules_template_main -all input-example.c
```

**Input (`input-example.c`):**
```c
#include "config.h"
// Main function
int main() {
    int size = MAX_SIZE;
    return 0;
}
```

**Input (`config.h`):**
```c
#define MAX_SIZE 1024
```

**Output (`input-example_pp.c`):**
```c
#define MAX_SIZE 1024
 
int main() {
    int size = 1024;
    return 0;
}
```

---

### Example 3: Conditional Compilation

**Command:**
```bash
./modules_template_main -d input-example.c
```

**Input:**
```c
#define FEATURE_ENABLED

#ifdef FEATURE_ENABLED
    void enable_feature() {
        // feature code
    }
#endif

#ifdef DISABLED_FEATURE
    void disabled_feature() {
        // this won't appear
    }
#endif
```

**Output:**
```c

    void enable_feature() {
         
    }

```

---

## 7. Input/Output Files

### Input Requirements

- **File format**: Valid C source (`.c`) or header (`.h`) file
- **Encoding**: ASCII or UTF-8
- **Path**: Can be relative or absolute

### Output Generation

**Naming Convention:**
- The preprocessor automatically generates output filenames
- Pattern: `<basename>_pp.<extension>`

**Examples:**
- `main.c` → `main_pp.c`
- `utils.h` → `utils_pp.h`
- `src/file.c` → `src/file_pp.c`

**Output Location:**
- Output file is created in the same directory as the input file

---

## 8. Error Handling

### Error Messages

P1PP reports errors with clear, informative messages that include:
- **Line number** where the error occurred
- **Error description** explaining what went wrong
- **File context** (when processing includes)

**Format:**
```
Error on line <line_number>: <error_message>
```

### Common Error Types

#### 8.1 File Errors

**File Not Found:**
```
Error on line 1: Could not open include file "missing.h"
```
**Cause:** The included file doesn't exist in the expected location  
**Solution:** Verify the filename and path are correct

**Cannot Read File:**
```
Error: Unable to read input file "input.c"
```
**Cause:** File permissions or path issues  
**Solution:** Check file permissions and verify the path

**Cannot Write Output:**
```
Error: Unable to write output file "output_pp.c"
```
**Cause:** Insufficient permissions or disk space  
**Solution:** Check write permissions for the output directory

#### 8.2 Preprocessing Errors

**Unterminated Comment:**
```
Error on line 45: Unterminated /* comment at end of file
```
**Cause:** Multi-line comment `/* ... */` is not closed  
**Solution:** Add closing `*/` to the comment block

**Invalid Directive Syntax:**
```
Error on line 12: Malformed #include directive
```
**Cause:** Incorrect syntax in preprocessor directive  
**Solution:** Verify directive follows correct format (e.g., `#include "file.h"`)

**Missing Macro Name:**
```
Error on line 8: #define missing macro name
```
**Cause:** `#define` directive without a macro name  
**Solution:** Add macro name: `#define NAME value`

**Unmatched #endif:**
```
Error on line 30: #endif without matching #ifdef
```
**Cause:** More `#endif` directives than `#ifdef` directives  
**Solution:** Remove extra `#endif` or add missing `#ifdef`

**Missing #endif:**
```
Error on line 100: Unterminated #ifdef at end of file
```
**Cause:** `#ifdef` block not closed with `#endif`  
**Solution:** Add `#endif` to close all conditional blocks

#### 8.3 Memory Errors

**Out of Memory:**
```
Error on line 150: Out of memory
```
**Cause:** System ran out of available memory during processing  
**Solution:** 
- Process smaller files
- Close other applications
- Increase available system memory

**Buffer Overflow:**
```
Error on line 75: Macro expansion failed
```
**Cause:** Macro value or expanded text exceeds internal limits  
**Solution:** Reduce macro complexity or size

### Error Behavior

**Graceful Handling:**
- P1PP continues processing after most errors
- Multiple errors are reported in a single run
- Error count is tracked throughout execution

**Exit Codes:**
- `0` - Success (no errors)
- `1` - Errors occurred (check stderr for details)

**Output on Error:**
- Errors are written to **stderr** (standard error stream)
- Partial output may be written to the output file
- The output file may be incomplete if errors occurred

---

## 9. Limitations

P1PP implements a **subset** of standard C preprocessor functionality. Understanding these limitations is important for effective use.

### 9.1 Unsupported Preprocessor Directives

The following standard C preprocessor directives are **not supported**:

| Directive | Status | Behavior |
|-----------|--------|----------|
| `#include <...>` | Not supported | Left unchanged in output |
| `#ifndef` | Not supported | Left unchanged in output |
| `#else` | Not supported | Left unchanged in output |
| `#elif` | Not supported | Left unchanged in output |
| `#undef` | Not supported | Left unchanged in output |
| `#pragma` | Not supported | Left unchanged in output |
| `#error` | Not supported | Left unchanged in output |
| `#warning` | Not supported | Left unchanged in output |
| `#line` | Not supported | Left unchanged in output |

**Note:** Unsupported directives are passed through to the output without processing or error messages.

### 9.2 Macro Limitations

**Function-Like Macros:**
- **Not Supported:** Parametrized macros like `#define FUNC(x) ((x) * 2)`
- **Behavior:** Definition is stored but parameters are not handled
- **Workaround:** Use object-like macros only

**Macro Features Not Implemented:**
- `##` (token concatenation operator)
- `#` (stringification operator)
- Variadic macros (`...`)
- Macro redefinition warnings
- Recursive macro expansion protection

**Macro Constraints:**
- **Maximum macro name length:** 128 characters
- **Maximum macro value length:** 512 characters
- Exceeding these limits may cause truncation or errors

### 9.3 Include Limitations

**System Includes:**
- Syntax `#include <stdio.h>` is **not processed**
- These directives remain in the output unchanged

**Include Behavior:**
- Only local includes with quotes are supported: `#include "file.h"`
- Relative paths are resolved from the input file's directory
- No include guard detection
- No circular include protection (may cause infinite loops)

**Path Constraints:**
- **Maximum path length:** 4096 characters
- **Maximum include filename:** 256 characters

### 9.4 Conditional Compilation Limitations

**Supported:**
- `#ifdef NAME` ... `#endif` (basic conditional inclusion)

**Not Supported:**
- `#ifndef` (inverse conditional)
- `#else` (alternative branch)
- `#elif` (else-if chain)
- Expression evaluation in conditionals
- Defined operator: `#if defined(NAME)`

**Nesting Limit:**
- **Maximum nesting depth:** 64 levels
- Exceeding this limit may cause unpredictable behavior

### 9.5 Comment Processing Limitations

**Proper Handling:**
- Single-line comments: `//`
- Multi-line comments: `/* ... */`

**Edge Cases:**
- Comments in string literals are **not** protected (they are removed)
- Comments in character literals may be incorrectly processed
- Nested `/* /* */ */` comments are not handled correctly

### 9.6 General Limitations

**No Preprocessing Symbols:**
- Predefined macros like `__FILE__`, `__LINE__`, `__DATE__` are not available

**No Operator Support:**
- The `defined` operator is not supported
- Macro expansion does not handle operators

**Single-Pass Processing:**
- The preprocessor makes a single pass through the input
- Some complex preprocessing scenarios may not work correctly

**Character Encoding:**
- Only ASCII and UTF-8 text files are supported
- Other encodings may produce incorrect results

**Line Number Preservation:**
- Original line numbers are maintained by preserving newlines
- No `#line` directives are generated in the output

---

## 10. Troubleshooting

### 10.1 Build Issues

#### Problem: CMake Configuration Fails

**Symptoms:**
```
CMake Error: Could not find CMAKE_ROOT
```

**Solutions:**
1. Verify CMake is installed:
   ```bash
   cmake --version
   ```
2. Ensure CMake version is 3.10 or higher
3. Reinstall CMake if necessary

#### Problem: Compiler Not Found

**Symptoms:**
```
CMake Error: CMAKE_C_COMPILER not set
```

**Solutions:**
1. Install GCC compiler:
   ```bash
   # On MSYS2
   pacman -S mingw-w64-ucrt-x86_64-gcc
   
   # On Ubuntu/Debian
   sudo apt-get install gcc
   
   # On macOS
   xcode-select --install
   ```

2. Verify compiler is in PATH:
   ```bash
   gcc --version
   ```

#### Problem: Build Fails with Linking Errors

**Symptoms:**
```
undefined reference to `buffer_init'
```

**Solutions:**
1. Clean and rebuild:
   ```bash
   cd build
   rm -rf *
   cmake ..
   cmake --build .
   ```

2. Verify all source files are present in `src/` directory

### 10.2 Runtime Issues

#### Problem: "Command not found"

**Symptoms:**
```bash
./modules_template_main input.c
bash: ./modules_template_main: No such file or directory
```

**Solutions:**
1. Verify you're in the correct directory (should contain the executable)
2. Check the executable name matches your platform:
   - Windows: `modules_template_main.exe`
   - Linux/Mac: `modules_template_main`
3. Build the project if not already done

#### Problem: Input File Not Found

**Symptoms:**
```
Error: Unable to read input file "test.c"
```

**Solutions:**
1. Verify file exists:
   ```bash
   ls test.c
   ```
2. Check file permissions:
   ```bash
   chmod +r test.c
   ```
3. Use absolute path or correct relative path
4. Verify file extension is `.c` or `.h`

#### Problem: No Output Generated

**Symptoms:**
- Program runs but no `_pp.c` file is created

**Solutions:**
1. Check for error messages in the console
2. Verify write permissions in the output directory
3. Check disk space availability
4. Look for the output file in the same directory as input:
   ```bash
   ls -la *_pp.c
   ```

#### Problem: Output File is Empty or Incorrect

**Symptoms:**
- Output file created but contains no content or wrong content

**Solutions:**
1. Verify correct flags are used:
   ```bash
   # For comment removal only
   ./modules_template_main -c input.c
   
   # For full preprocessing
   ./modules_template_main -all input.c
   ```

2. Check input file is valid C source code
3. Review error messages for preprocessing failures
4. Test with a simple input file first

### 10.3 Preprocessing Issues

#### Problem: Macros Not Expanded

**Symptoms:**
- Macro names appear in output instead of their values

**Solutions:**
1. Ensure `-d` or `-all` flag is used:
   ```bash
   ./modules_template_main -d input.c
   ```
2. Verify macro is defined before use with `#define`
3. Check macro name doesn't exceed 128 characters
4. Verify macro value doesn't exceed 512 characters

#### Problem: Comments Still Visible in Output

**Symptoms:**
- Comments remain in the `_pp.c` file

**Solutions:**
1. Verify `-c` flag is active (it's default if no flags provided)
2. If using `-d` alone, comments are preserved; use `-all` instead:
   ```bash
   ./modules_template_main -all input.c
   ```

#### Problem: Include Files Not Found

**Symptoms:**
```
Error on line 1: Could not open include file "header.h"
```

**Solutions:**
1. Verify included file exists in the same directory as input file
2. Use correct relative path in `#include` directive
3. Check filename spelling and case (case-sensitive on Linux/Mac)
4. Verify file permissions allow reading
5. Remember: system includes `<...>` are not supported

#### Problem: Circular Include Loop

**Symptoms:**
- Program hangs or crashes
- Memory usage grows rapidly
- Stack overflow error

**Solutions:**
1. P1PP does **not** detect circular includes
2. Add include guards manually to header files:
   ```c
   #ifndef HEADER_H
   #define HEADER_H
   
   // header content
   
   #endif
   ```
3. Restructure includes to avoid circular dependencies

#### Problem: #ifdef Blocks Behaving Incorrectly

**Symptoms:**
- Code included when it should be excluded, or vice versa

**Solutions:**
1. Verify the macro is defined before `#ifdef`:
   ```c
   #define DEBUG
   #ifdef DEBUG
       // This will be included
   #endif
   ```

2. Remember: `#ifndef` is not supported, use `#ifdef` only
3. Ensure all `#ifdef` blocks have matching `#endif`
4. Check nesting doesn't exceed 64 levels

### 10.4 Common Usage Mistakes

#### Mistake: Wrong Flag Combination

**Incorrect:**
```bash
./modules_template_main -c -d input.c    # Works but redundant
```

**Correct:**
```bash
./modules_template_main -all input.c     # Cleaner equivalent
```

#### Mistake: Trying to Use System Includes

**Incorrect:**
```c
#include <stdio.h>    // Not supported
```

**Correct:**
```c
// Either accept it remains unprocessed, or
// Create local header files instead
#include "mystdio.h"
```

#### Mistake: Using Function-Like Macros

**Incorrect:**
```c
#define SQUARE(x) ((x) * (x))    // Not fully supported
```

**Correct:**
```c
#define MAX_SIZE 100             // Use object-like macros
```

### 10.5 Getting Help

If you encounter issues not covered here:

1. **Check error messages carefully** - they usually indicate the problem
2. **Review the input file** - ensure it's valid C syntax
3. **Test with simplified input** - isolate the problematic section
4. **Check the examples** - compare with working examples in Section 6
5. **Verify build environment** - ensure all prerequisites are installed
6. **Review limitations** - the issue may be due to unsupported features (Section 9)

**For course-related help:**
- Consult the course instructor or teaching assistants
- Review the `COMP_P1PP_handout.txt` document
- Check the README.md for internal design details

---

## Appendix: Quick Reference

### Command Summary

```bash
# Show help
./modules_template_main -help

# Remove comments only (default)
./modules_template_main input.c
./modules_template_main -c input.c

# Process directives and macros only (keeps comments)
./modules_template_main -d input.c

# Full preprocessing (comments + directives + macros)
./modules_template_main -all input.c
```

### Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Errors occurred |

### File Extensions

| Input | Output |
|-------|--------|
| `file.c` | `file_pp.c` |
| `file.h` | `file_pp.h` |

### Maximum Limits

| Limit | Value |
|-------|-------|
| Macro name length | 128 characters |
| Macro value length | 512 characters |
| Include path length | 4096 characters |
| Include filename | 256 characters |
| `#ifdef` nesting depth | 64 levels |

---

**End of User Manual**
