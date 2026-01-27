# P1PP User Manual
## C Preprocessor Practice Tool

**Version:** 1.0  
**Course:** Compilers - Practice 1  
**Last Updated:** January 2026

---

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Command-Line Usage](#command-line-usage)
5. [Features](#features)
6. [Examples](#examples)
7. [Input/Output Files](#inputoutput-files)
8. [Error Handling](#error-handling)
9. [Limitations](#limitations)
10. [Troubleshooting](#troubleshooting)

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

P1PP reports errors with file name and line number:

```
Error in file.c:15