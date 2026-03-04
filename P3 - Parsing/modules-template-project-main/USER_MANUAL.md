# P3 Parser User Manual
## Shift-Reduce Automaton Parser Practice Tool

**Version:** 1.0  
**Course:** Compilers - Practice 3  
**Last Updated:** March 2026

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

**P3 Parser** is a general-purpose **Shift-Reduce Automaton (SRA) parser engine** implemented in C. It is the Practice 3 tool for the Compilers course (Dolors Sala, UPF) and implements bottom-up parsing using ACTION and GOTO tables loaded at runtime from a plain-text grammar specification file.

### What Does P3 Do?

- Loads a grammar specification (terminals, non-terminals, production rules, ACTION table, GOTO table) from a `.txt` file
- Reads a token sequence from a `.cscn` file produced by the P2 scanner
- Runs a bottom-up Shift-Reduce Automaton parse over the token sequence
- Produces a `<basename>_p3dbg.txt` step-by-step parse log file
- Reports **ACCEPTED** or **ERROR** as the parse result

### Key Benefits

- **Grammar-agnostic engine**: changing the grammar file changes the language without modifying the engine source code
- **Modular Design**: dedicated modules for the DFA, stack, grammar loading, token input, and output
- **Educational**: demonstrates bottom-up parsing with a shift/reduce automaton
- **Traceable**: outputs a detailed step-by-step parse log for every parse action

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
   git clone https://github.com/FrancescBaiget/GK.git
   cd GK/P3-Parser/modules-template-project-main
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
   make
   ```

5. **Locate the executable**:
   - The main executable is `p3_parser` (Linux/macOS) or `p3_parser.exe` (Windows)
   - It will be inside the `build/` directory after compilation

---

## 3. Quick Start

### Basic Usage

Parse a token file using the default grammar (Language 1):
```bash
./p3_parser samples/sample1.cscn
```

Parse with an explicit grammar file:
```bash
./p3_parser samples/sample1.cscn data/language1.txt
```

Parse using Language 2 grammar:
```bash
./p3_parser samples/sample4.cscn data/language2.txt
```

This produces a `<basename>_p3dbg.txt` file recording every SRA step and prints the parse result (`ACCEPTED` or `ERROR`).

---

## 4. Command-Line Usage

### Syntax

```
p3_parser <input.cscn> [language_spec.txt]
```

### Arguments

| Argument | Description | Default |
|----------|-------------|---------|
| `<input.cscn>` | Token input file (scanner output from P2) | Required |
| `[language_spec.txt]` | Grammar specification file | `./data/language1.txt` |

### Notes

- The input file must be a valid `.cscn` token file produced by the P2 scanner.
- If no grammar file is specified, `./data/language1.txt` is used by default.
- The output debug file is automatically named `<basename>_p3dbg.txt` and created in the current working directory.

---

## 5. Features

### 5.1 Grammar-Agnostic SRA Engine

The parser engine reads any grammar expressed in the grammar file format. Two grammars are included:

- **Language 1** (`data/language1.txt`): Arithmetic expressions
  - Grammar rules: `s→e`, `e→e+t | t`, `t→t*f | f`, `f→(e) | NUM`
  - Terminals: `+`, `*`, `(`, `)`, `NUM`, `$`
  - 13 states
- **Language 2** (`data/language2.txt`): Theory slide 32 grammar
  - Grammar rules: `e→e+(e) | int`

### 5.2 Parse Actions

The SRA engine performs four types of actions:

- **SHIFT**: push the next input token and transition to a new state
- **REDUCE**: pop the right-hand side symbols of a rule and push the left-hand side non-terminal via the GOTO table
- **ACCEPT**: parsing completed successfully; the input is valid under the grammar
- **ERROR**: an invalid token was encountered; parsing halts immediately

### 5.3 Step-by-step Debug Log

Every parse step is recorded in `<basename>_p3dbg.txt` with the following format:

```
[SHIFT  ] pos=<n>  state=<from>-><to>  token=<name>("<lexeme>")  stack: <trace>
[REDUCE<r>] pos=<n>  state=<from>-><to>  rule=<desc>  stack: <trace>
[ACCEPT ] pos=<n>  state=<s>  ACCEPTED  stack: <trace>
[ERROR  ] pos=<n>  state=<s>  token=<name>  PARSE ERROR
```

### 5.4 Module Architecture

| Module | Responsibility |
|--------|----------------|
| `dfa/` | Stores ACTION/GOTO tables; provides `dfa_get_action` / `dfa_get_goto` |
| `lang_spec/` | Loads grammar from file into `lang_spec_t`; maps tokens to terminal indices |
| `token_loader/` | Reads `.cscn` files and builds the token list |
| `parser_stack/` | SRA parse stack: push, pop, top-state, print |
| `sra/` | Main SRA parse loop: shift/reduce/accept/error |
| `out_writer/` | Opens `_p3dbg.txt` and writes formatted step logs |
| `module_args/` | Parses CLI arguments into `config_t` |

### 5.5 Debug Tracing (Compile-time)

Each module header has a `TRACE_<MODULE>` compile-time flag (default `0`). Set it to `1` to enable per-module debug output to stderr. This is useful for tracing internal state during development.

---

## 6. Examples

### Example 1: Arithmetic Expression (Language 1)

**Command:**
```bash
./p3_parser samples/sample1.cscn data/language1.txt
```

**Input (`sample1.cscn`):**
```
<3, CAT_NUMBER> <+, CAT_OPERATOR> <5, CAT_NUMBER> <*, CAT_OPERATOR> <2, CAT_NUMBER>
```

Parses the expression `3 + 5 * 2`.  
**Output:** `sample1_p3dbg.txt` — result: **ACCEPTED**

---

### Example 2: Parenthesised Expression (Language 1)

**Command:**
```bash
./p3_parser samples/sample2.cscn data/language1.txt
```

**Input (`sample2.cscn`):**
```
<(, CAT_SPECIALCHAR> <3, CAT_NUMBER> <+, CAT_OPERATOR> <4, CAT_NUMBER> <), CAT_SPECIALCHAR> <*, CAT_OPERATOR> <2, CAT_NUMBER>
```

Parses the expression `(3+4) * 2`.  
**Output:** `sample2_p3dbg.txt` — result: **ACCEPTED**

---

### Example 3: Single Number (Language 1 and 2)

**Command:**
```bash
./p3_parser samples/sample3.cscn
```

**Input (`sample3.cscn`):**
```
<42, CAT_NUMBER>
```

Parses the single number `42`.  
**Output:** `sample3_p3dbg.txt` — result: **ACCEPTED**

---

### Example 4: Language 2 Grammar

**Command:**
```bash
./p3_parser samples/sample4.cscn data/language2.txt
```

**Input (`sample4.cscn`):** token sequence for `42 + ( 7 )`  
**Output:** `sample4_p3dbg.txt` — result: **ACCEPTED**

---

## 7. Input/Output Files

### Input Requirements

- **Token file**: `.cscn` file produced by the P2 scanner
- **Grammar file**: `.txt` grammar specification (see Grammar File Format below)
- **Encoding**: ASCII or UTF-8
- **Path**: relative or absolute

### Grammar File Format

```
LANGUAGE <name>

TERMINALS <count>
<name> LEXEME:<value>       # match by literal lexeme
<name> CATEGORY:<tag>       # match by scanner category
<name> EOF                  # end-of-input sentinel

NONTERMINALS <count>
<name>

RULES <count>
<lhs_index> <rhs_len> <description>

STATES <count>
ACTION
E|S<n>|R<n>|A  ...         # one row per state, one cell per terminal
GOTO
<state>|-1  ...             # one row per state, one column per non-terminal
```

### Output Files

| Output | Description |
|--------|-------------|
| `<basename>_p3dbg.txt` | Step-by-step parse log (SHIFT / REDUCE / ACCEPT / ERROR) |
| `proj_p3_parser.log` | Program execution log |

### Naming Convention

- `sample1.cscn` → `sample1_p3dbg.txt`
- `myinput.cscn` → `myinput_p3dbg.txt`

**Output Location:** The debug file is created in the current working directory.

---

## 8. Error Handling

### Error Format

Parse errors are recorded in the `_p3dbg.txt` file:
```
[ERROR  ] pos=<n>  state=<s>  token=<name>  PARSE ERROR
```

Also reported to stdout/log:
```
Parse result: ERROR
```

### Common Error Types

| Error | Meaning |
|-------|---------|
| Cannot open input file | Token file not found or not readable |
| Cannot load grammar spec | Grammar file missing or malformed |
| Cannot load tokens | Token file format invalid |
| Parse error | Input token sequence is not valid under the grammar |

**Error from failed grammar load:**
```
Failed to load language spec: data/language1.txt
```

**Error from failed token load:**
```
Failed to load tokens from: samples/sample1.cscn
```

### Exit Codes

| Code | Meaning |
|------|---------|
| `0` | Success (ACCEPTED) |
| `1` | Error (parse failure or I/O error) |

---

## 9. Limitations

- Supports only grammars expressible as **LR(0)/SLR shift-reduce automata** via the provided grammar file format
- Grammar must be pre-compiled into ACTION/GOTO table format manually; there is no automatic table generation
- Input must be a `.cscn` file produced by the P2 scanner; raw source files are not accepted directly
- Only the token categories and lexemes recognised by the P2 scanner can be matched in grammar terminals: `CAT_NUMBER`, `CAT_OPERATOR`, `CAT_SPECIALCHAR`, `CAT_KEYWORD`, `CAT_IDENTIFIER`, `CAT_LITERAL`, `CAT_NONRECOGNIZED`
- No support for ambiguous grammars
- No error recovery — parsing halts on the first parse error
- Maximum stack depth and token count are implementation-defined

---

## 10. Troubleshooting

### 10.1 Build Issues

#### Problem: CMake Configuration Fails
```
CMake Error: Could not find CMAKE_ROOT
```
**Solutions:**
1. Verify CMake is installed: `cmake --version`
2. Ensure CMake version is 3.10 or higher
3. Reinstall CMake if necessary

#### Problem: Compiler Not Found
```
CMake Error: CMAKE_C_COMPILER not set
```
**Solutions:**
1. Install GCC:
   - MSYS2: `pacman -S mingw-w64-ucrt-x86_64-gcc`
   - Ubuntu: `sudo apt-get install gcc`
   - macOS: `xcode-select --install`
2. Verify the compiler is in PATH: `gcc --version`

#### Problem: Build Fails with Linking Errors
```
undefined reference to `dfa_get_action'
```
**Solutions:**
1. Clean and rebuild: `cd build && rm -rf * && cmake .. && make`
2. Verify all source files are present in the `src/` directory

---

### 10.2 Runtime Issues

#### Problem: "Command not found"
```
bash: ./p3_parser: No such file or directory
```
**Solutions:**
1. Verify you are in the correct directory (should contain `p3_parser`)
2. Check the executable name: `p3_parser.exe` on Windows, `p3_parser` on Linux/macOS
3. Build the project if you have not done so yet

#### Problem: "Failed to load language spec"
```
Failed to load language spec: data/language1.txt
```
**Solutions:**
1. Verify the grammar file exists: `ls data/language1.txt`
2. Run the executable from the `modules-template-project-main` directory, or provide an absolute path
3. Check file permissions

#### Problem: "Failed to load tokens"
```
Failed to load tokens from: samples/sample1.cscn
```
**Solutions:**
1. Verify the input file exists: `ls samples/sample1.cscn`
2. Check that the file is a valid `.cscn` token file produced by the P2 scanner
3. Verify file permissions

#### Problem: No debug output file generated
**Solutions:**
1. Check for error messages on stdout/log
2. Verify write permissions in the working directory
3. Look for the file with `ls -la *_p3dbg.txt`

---

### 10.3 Parse Issues

#### Problem: PARSE ERROR on valid-looking input
**Solutions:**
1. Verify the correct grammar file is used for the input
2. Ensure the `.cscn` file was produced from valid input for that grammar
3. Check that Language 2 inputs are run with `data/language2.txt` and Language 1 inputs with `data/language1.txt`
4. Review the `_p3dbg.txt` file to see which state/token caused the error

#### Problem: Wrong grammar loaded (default used accidentally)
**Solution:** Always specify the grammar file explicitly:
```bash
./p3_parser samples/sample4.cscn data/language2.txt
```

---

### 10.4 Common Usage Mistakes

**Mistake:** Using raw source files instead of `.cscn` token files

Incorrect:
```bash
./p3_parser example.c
```

Correct:
```bash
./p3_parser example.cscn
```
*(First run through the P2 scanner to produce the `.cscn` file)*

---

**Mistake:** Running from the wrong directory (grammar file not found)

Incorrect:
```bash
cd build && ./p3_parser samples/sample1.cscn
```

Correct:
```bash
# From the modules-template-project-main directory:
./build/p3_parser samples/sample1.cscn data/language1.txt
```

---

### 10.5 Getting Help

1. Check error messages carefully — the `_p3dbg.txt` file shows the exact step where the error occurred
2. Review the `_p3dbg.txt` step log to trace the parse
3. Test with the provided sample files first
4. Verify the grammar file format matches the specification in [Section 7](#7-inputoutput-files)
5. Review [Section 9 – Limitations](#9-limitations) — the issue may be due to an unsupported grammar type
6. For course-related help: consult the instructor or check `COMP_P3_handout.txt`

---

## Appendix: Quick Reference

### Command Summary

```bash
# Parse with default grammar (Language 1)
./p3_parser samples/sample1.cscn

# Parse with explicit grammar file
./p3_parser samples/sample1.cscn data/language1.txt

# Parse using Language 2 grammar
./p3_parser samples/sample4.cscn data/language2.txt
```

### Exit Codes

| Code | Meaning |
|------|---------|
| `0` | ACCEPTED (parse successful) |
| `1` | ERROR (parse failed or I/O error) |

### File Extensions

| Input | Output |
|-------|--------|
| `<name>.cscn` | `<name>_p3dbg.txt` |

### Sample Files

| File | Expression | Compatible Grammar |
|------|------------|--------------------|
| `sample1.cscn` | `3 + 5 * 2` | Language 1 |
| `sample2.cscn` | `(3+4) * 2` | Language 1 |
| `sample3.cscn` | `42` | Language 1 and 2 |
| `sample4.cscn` | `42 + ( 7 )` | Language 2 |

---

**End of User Manual**
