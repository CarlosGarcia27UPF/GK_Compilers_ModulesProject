# Lexer / Scanner (Practice 2)

> **Course:** Compilers  
> **Practice:** P2 – Lexer/Scanner  
> **This README is a living internal design document for the team.**

---

## 1. Project Goal

The goal of this project is to design and implement a **modular lexical scanner (P2)** as the second stage of a compiler.

This project corresponds to **Practice 2 (P2)** of the Compilers course.  
It extends the P1 preprocessor work with a **scanner/lexer** that turns source code into a token stream.

The focus of P2 is:
- correct token recognition
- DFA-driven scanning
- clear internal architecture
- modular responsibilities

The **User Manual** for students/users is provided in:
```
P2-Lexxer-Scanner/modules-template-project-main/USER_MANUAL.md
```

---

## 2. Input and Output

### Input
- A C-like source file (`.c`)
- CLI:
  - `./modules_template_main <input.c>`

### Output
- A token output file with `.cscn` suffix:
  - `file.c` → `file.cscn`
- Optional counting output (if enabled at build time):
  - `file.c` → `file.cdbgcnt`

---

## 3. Repository Structure

The project is organized into independent modules under `src/`.

```text
src/
main.c                // scanner driver
main.h

lang_spec/            // language specification constants
char_stream/          // input stream reader
token/                // token data type
token_list/           // token list data structure
automata/             // DFA scanner engine
out_writer/           // token output writer
logger/               // message routing
error/                // centralized errors
counter/              // operation counting

module_args/          // template utilities
module_2/             // template module
utils_files.c         // template utils
```

---

## 4. High-Level Architecture

### Design Principles
- `main.c` orchestrates the scan pipeline
- `automata` is the **only** module that consumes input characters
- `lang_spec` centralizes all language rules
- `out_writer` generates the `.cscn` output

### Scanner Pipeline
1. Parse CLI arguments
2. Open file through `char_stream`
3. Run `automata_scan` (DFA)
4. Produce token list
5. Write `.cscn` output

---

## 5. Token Categories

The scanner recognizes:

- **Numbers**: `[0-9]+`
- **Identifiers**: `[A-Za-z][A-Za-z0-9]*`
- **Keywords**: `if else while return int char void`
- **Literals**: `"..."` (double-quoted strings)
- **Operators**: `= > + *`
- **Special Characters**: `( ) ; { } [ ] ,`
- **Non-recognized**: grouped invalid lexemes

---

## 6. Output Format

### RELEASE (default)
```
<lexeme, CATEGORY> <lexeme, CATEGORY> ...
```

### DEBUG
- Same as release, with line number prefix
- Blank line between token lines

---

## 7. Operation Counting (Optional)

If compiled with `COUNTCONFIG`, the scanner prints counters for:
- comparisons
- I/O operations
- general instructions

Output routing is configured by compile-time flags.

---

## 8. Development Rules

- No magic constants outside `lang_spec`
- Each module must be independent and testable
- Main must remain a thin orchestrator
- Project must always compile and run

---

## 9. Status

**Current Stage:** Practice 2 — Lexical Analysis  
**Next Stage:** Parser integration (future hook in `main.c`)

---

## 10. Team Responsibilities

Each team member owns one or more modules and is responsible for:
- design decisions
- implementation
- keeping this README consistent with the code

---