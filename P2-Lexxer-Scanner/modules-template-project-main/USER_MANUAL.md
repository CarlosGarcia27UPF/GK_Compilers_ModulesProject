# P2 Lexer/Scanner User Manual
## Lexer/Scanner Practice Tool

**Version:** 1.0  
**Course:** Compilers - Practice 2  
**Last Updated:** February 2026

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

**P2 Lexer/Scanner** is the Practice 2 tool for the Compilers course. It performs lexical analysis of a source file, producing a token stream that can later be consumed by the parser. The scanner uses a DFA-based engine and outputs a token file with a `.cscn` suffix.

### What Does P2 Do?

P2 scans a source file and produces tokens for:
- **Numbers** (integers)
- **Identifiers**
- **Keywords**
- **String literals**
- **Operators**
- **Special characters**
- **Non-recognized sequences** (grouped)

### Key Benefits
- **Modular Design**: Dedicated modules for scanning, token storage, and output
- **Educational**: Demonstrates lexical analysis over a C subset
- **Traceable**: Provides token count and optional debug formatting

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
   cd GK_Compilers_ModulesProject/P2-Lexxer-Scanner/modules-template-project-main
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
   - The main executable is `modules_template_main`
   - It will be inside the `build/` directory after compilation

---

## 3. Quick Start

### Basic Usage

Scan a file and generate the token output:
```bash
./modules_template_main input.c
```

This produces:
- `input.cscn` (token output)
- If counting is enabled by build configuration: `input.cdbgcnt`

---

## 4. Command-Line Usage

### Syntax

```
./modules_template_main <input.c>
```

### Notes
- The scanner expects **one input file**.
- If the file cannot be opened, the tool returns an error.
- There are no runtime flags; configuration is controlled at **compile time**.

---

## 5. Features

### 5.1 Supported Language Subset

The scanner recognizes a subset of C:

#### Keywords
- `if`, `else`, `while`, `return`, `int`, `char`, `void`

#### Operators
- `=`, `>`, `+`, `*`

#### Special Characters
- `(` `)` `;` `{` `}` `[` `]` `,`

#### Numbers
- Integers: `[0-9]+`

#### Identifiers
- Pattern: `[A-Za-z][A-Za-z0-9]*`
- If an identifier matches a keyword, it is categorized as a **keyword**

#### Literals
- Double-quoted strings: `"..."`

#### Non-Recognized
- Any unsupported lexeme is grouped into a `CAT_NONRECOGNIZED` token

---

### 5.2 Output Formats

Output is written to a `.cscn` file in one of two formats:

#### RELEASE format (default)
- Tokens appear on the same line as their source line
- Tokens are separated by spaces
- Format:
  ```
  <lexeme, CATEGORY> <lexeme, CATEGORY> ...
  ```

#### DEBUG format
- Same as RELEASE
- Each line starts with the original source line number
- An empty line separates each token line

Format is controlled via compile-time `OUTFORMAT` definition.

---

### 5.3 Operation Counting (Optional)

If compiled with `COUNTCONFIG`:
- The scanner logs operation counts (comparisons, I/O, general ops)
- The output can be routed to:
  - Standard output
  - The `.cscn` file
  - A `.cdbgcnt` file (default)

#### How to Enable/Disable Counting (Comment/Uncomment)

Counting is controlled at compile time in:

- `src/CMakeLists.txt`

Current counting activation lines:

```cmake
# Enable operation counting and route output to .dbgcnt file.
target_compile_definitions(automata PRIVATE COUNTCONFIG COUNTOUT=1)
target_compile_definitions(modules_template_main PRIVATE COUNTCONFIG COUNTOUT=1)
```

Enable counting:
- Keep both lines uncommented.

Disable counting:
- Comment both lines:

```cmake
# target_compile_definitions(automata PRIVATE COUNTCONFIG COUNTOUT=1)
# target_compile_definitions(modules_template_main PRIVATE COUNTCONFIG COUNTOUT=1)
```

After changing those lines, rebuild so the generated Makefiles/executable pick up the new configuration:

```bash
cd build
cmake ..
cmake --build .
```

---

## 6. Examples

### Example 1: Basic Scan

**Command:**
```bash
./modules_template_main example.c
```

**Input (`example.c`):**
```
int x = 10;
```

**Output (`example.cscn`):**
```
<int, CAT_KEYWORD> <x, CAT_IDENTIFIER> <=, CAT_OPERATOR> <10, CAT_NUMBER> <;, CAT_SPECIALCHAR>
```

---

## 7. Input/Output Files

### Input Requirements

- **File format**: C-like source file
- **Encoding**: ASCII or UTF-8
- **Path**: Relative or absolute

### Output Files

| Output | Description |
|--------|-------------|
| `<input>.cscn` | Token output file |
| `<input>.cdbgcnt` | Operation count output (optional, if enabled) |

---

## 8. Error Handling

Errors are reported through a central error catalog and formatted as:

```
[ERROR <id>][<STEP>] Line <line>: <message>: <context>
```

### Common Error Types

| Error ID | Meaning |
|---------|---------|
| 1 | Cannot open input file |
| 2 | Cannot create output file |
| 3 | Unterminated literal |
| 4 | Non-recognized character(s) |
| 5 | Internal error |

---

## 9. Limitations

- Supports only a **subset of C**
- Operators are limited to single-character `= > + *`
- No floating-point numbers (only integers)
- No escape sequences inside literals
- No preprocessing directives

---

## 10. Troubleshooting

### 10.1 Build Issues

#### Problem: CMake Configuration Fails
**Solution:** Verify CMake is installed and version >= 3.10

#### Problem: Compiler Not Found
**Solution:** Install GCC and ensure it is in PATH

---

### 10.2 Runtime Issues

#### Problem: “Cannot open input file”
**Solution:** Check file path and permissions

#### Problem: “Cannot create output file”
**Solution:** Verify write permissions in the output directory

---

**End of User Manual**
