# Parser / Bottom-up Parser (Practice 3)

> **Course:** Compilers  
> **Practice:** P3 – Parser / Shift-Reduce Automaton  
> **This README is a living internal design document for the team.**

---

## 1. Project Goal

The goal of this project is to design and implement a **modular bottom-up parser (P3)** as the third stage of a compiler.

This project corresponds to **Practice 3 (P3)** of the Compilers course.  
It extends the P2 lexer/scanner work with a **shift-reduce automaton (SRA) engine** that takes a token stream and determines whether it belongs to a given grammar.

The focus of P3 is:
- correct bottom-up parsing using shift/reduce operations
- a grammar-agnostic SRA engine driven by table data
- clear separation between the engine and the language specification
- modular responsibilities across DFA, stack, and parser components

The **User Manual** for students/users is provided in:
```
P3-Parser/modules-template-project-main/USER_MANUAL.md
```

---

## 2. Input and Output

### Input
- A scanner output file (`.cscn`) produced by the P2 lexer
- A grammar specification file (`.txt`) describing the language
- CLI:
  - `./p3_parser <input.cscn> [language_spec.txt]`

### Output
- A debug step log file with `_p3dbg.txt` suffix:
  - `file.cscn` → `file_p3dbg.txt`
- A timestamped run log:
  - `proj_p3_parser.log`

---

## 3. Repository Structure

The project is organized into independent modules under `src/`.

```text
modules-template-project-main/
├── CMakeLists.txt          # Top-level CMake configuration
├── data/
│   ├── language1.txt       # Grammar spec: arithmetic expressions
│   └── language2.txt       # Grammar spec: theory slide 32
├── samples/
│   ├── sample1.cscn        # 3 + 5 * 2   (Language 1)
│   ├── sample2.cscn        # (3+4) * 2   (Language 1)
│   ├── sample3.cscn        # 42          (Language 1 and 2)
│   └── sample4.cscn        # 42 + ( 7 )  (Language 2)
├── src/
│   ├── main.c / main.h     # Main entry point
│   ├── utils_files.c/h     # Utility: timestamped log-file helper
│   ├── dfa/                # DFA – ACTION and GOTO tables and transitions
│   ├── lang_spec/          # Grammar/Language – load grammar from file
│   ├── token_loader/       # Input System – read .cscn token files
│   ├── parser_stack/       # Stack – SRA parse stack operations
│   ├── sra/                # Parser Engine – SRA main loop (DFA + Stack)
│   ├── out_writer/         # Output & Debug – write _p3dbg.txt step log
│   └── module_args/        # Main – process command-line arguments
└── tests/
    ├── test_dfa.c/h          # Unit tests: DFA module
    ├── test_lang_spec.c/h    # Unit tests: lang_spec module
    ├── test_parser_stack.c/h # Unit tests: parser_stack module
    ├── test_sra.c/h          # Integration tests: SRA engine (language1 & language2)
    ├── test_module_args.c/h  # Unit tests: module_args
    └── CMakeLists.txt
```

---

## 4. High-Level Architecture

### Design Principles
- `main.c` orchestrates the full parse pipeline
- `lang_spec` loads the grammar from a file — changing the file changes the language, not the engine
- `dfa` is the **only** module that owns the ACTION/GOTO tables
- `sra` is the **only** module that runs the shift/reduce loop, using `dfa` and `parser_stack`
- `out_writer` produces the `_p3dbg.txt` step-by-step trace

### Parser Pipeline
1. Parse CLI arguments (`module_args`)
2. Load grammar specification from file (`lang_spec`)
3. Load token list from `.cscn` file (`token_loader`)
4. Open debug output file (`out_writer`)
5. Run SRA engine (`sra` → `dfa` + `parser_stack`)
6. Write parse result to log

---

## 5. Grammar

Two example grammars are supported:

### Language 1 – Arithmetic Expressions
```
1. s  → e
2. e  → e + t
3.      | t
4. t  → t * f
5.      | f
6. f  → ( e )
7.      | NUM
```
- Non-terminals: `s` (start), `e` (expression), `t` (term), `f` (factor)
- Terminals: `+`, `*`, `(`, `)`, `NUM`
- Multiplication has higher precedence than addition

### Language 2 – Theory Slide 32
```
1. e  → e + (e)
2. e  → int
```
- Terminals: `+`, `(`, `)`, `int`

---

## 6. Output Format

Each line in `<name>_p3dbg.txt` follows one of these formats:

### SHIFT
```
[SHIFT  ] pos=<n>  state=<from>-><to>  token=<name>("<lexeme>")  stack: <trace>
```

### REDUCE
```
[REDUCE<r>] pos=<n>  state=<from>-><to>  rule=<desc>  stack: <trace>
```

### ACCEPT
```
[ACCEPT ] pos=<n>  state=<s>  ACCEPTED  stack: <trace>
```

### ERROR
```
[ERROR  ] pos=<n>  state=<s>  token=<name>  PARSE ERROR
```

`<trace>` shows the state at the bottom, then each pushed symbol as `(state,symbol)`.

---

## 7. Grammar File Format

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

---

## 8. Development Rules

- No grammar constants hard-coded in the engine — all language data lives in `lang_spec`
- Each module must be independent and individually testable
- `main.c` must remain a thin orchestrator
- The SRA engine must run correctly for any valid grammar file, not only the two provided
- Project must always compile and pass all tests

---

## 9. Status

**Current Stage:** Practice 3 — Bottom-up Parsing  Finished

---

## 10. Team Responsibilities

Each team member owns one or more modules and is responsible for:
- design decisions
- implementation
- keeping this README consistent with the code

---