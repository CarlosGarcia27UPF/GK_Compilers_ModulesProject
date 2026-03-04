# P3 Parser – Shift-Reduce Automaton (SRA) Engine

## Overview

This project implements a **general Shift-Reduce Automaton (SRA) parser engine** in C.
The engine is grammar-agnostic: the language specification (terminals, non-terminals,
production rules, ACTION table, and GOTO table) is loaded at run time from a plain-text
file, so changing the grammar file changes the language parsed without modifying the engine code.

The project was built for the Compilers course (Dolors Sala, UPF) — Practice 3: Bottom-up Parsing.

Two example grammars are provided:

| File | Grammar | Sample inputs |
|------|---------|---------------|
| `data/language1.txt` | Arithmetic expressions (`e -> e+t \| t`, `t -> t*f \| f`, `f -> (e) \| NUM`) | `samples/sample1.cscn`, `samples/sample2.cscn`, `samples/sample3.cscn` |
| `data/language2.txt` | Theory slide 32 (`e -> e+(e) \| int`) | `samples/sample3.cscn`, `samples/sample4.cscn` |

---

## Project Structure

```
/
├── CMakeLists.txt          # Top-level CMake configuration
├── data/
│   ├── language1.txt       # Grammar spec for Language 1 (arithmetic)
│   └── language2.txt       # Grammar spec for Language 2 (theory slide 32)
├── samples/
│   ├── sample1.cscn        # 3 + 5 * 2   (Language 1)
│   ├── sample2.cscn        # (3+4) * 2   (Language 1)
│   ├── sample3.cscn        # 42          (Language 1 and 2)
│   └── sample4.cscn        # 42 + ( 7 )  (Language 2)
├── src/
│   ├── main.c / main.h     # Main entry point (wrapper calling all modules)
│   ├── utils_files.c/h     # Utility: timestamped log-file helper
│   ├── dfa/                # Module: DFA – ACTION and GOTO tables and transitions
│   ├── lang_spec/          # Module: Grammar/Language – load grammar from file
│   ├── token_loader/       # Module: Input System – read .cscn token files
│   ├── parser_stack/       # Module: Stack – SRA parse stack operations
│   ├── sra/                # Module: Parser Engine – SRA main loop (DFA + Stack)
│   ├── out_writer/         # Module: Output & Debug – write _p3dbg.txt step log
│   └── module_args/        # Module: Main – process command-line arguments
└── tests/
    ├── test_dfa.c/h        # Unit tests: DFA module
    ├── test_lang_spec.c/h  # Unit tests: lang_spec module
    ├── test_parser_stack.c/h # Unit tests: parser_stack module
    ├── test_sra.c/h        # Integration tests: SRA engine (language1 & language2)
    ├── test_module_args.c/h # Unit tests: module_args
    └── CMakeLists.txt
```

---

## Modules

| Module | File(s) | Responsibility |
|--------|---------|----------------|
| **DFA** | `src/dfa/` | Deterministic Finite Automaton: stores ACTION/GOTO tables and provides `dfa_get_action` / `dfa_get_goto` |
| **Grammar & Language** | `src/lang_spec/` | Loads a grammar file into `lang_spec_t`; wraps DFA queries; maps tokens to terminal indices |
| **Input System** | `src/token_loader/` | Reads `.cscn` scanner-output files and builds the token list for the parser |
| **Stack** | `src/parser_stack/` | SRA parse stack: push, pop, top-state, print |
| **Parser Engine (SRA)** | `src/sra/` | Main SRA parse loop: reads tokens, queries DFA, calls shift/reduce/accept/error handlers |
| **Output & Debug** | `src/out_writer/` | Opens `<name>_p3dbg.txt` and writes a formatted line for each SRA step |
| **Main / Args** | `src/module_args/` | Parses `argc`/`argv` into a `config_t` struct used by `main.c` |

Each module header has a `TRACE_<MODULE>` compile-time flag (default 0).  
Set it to 1 to enable per-module debug output to `stderr`.

---

## Building

```bash
mkdir build && cd build
cmake ..
make
```

The main executable is `p3_parser`.

---

## Usage

```
p3_parser <input.cscn> [language_spec.txt]
```

- `<input.cscn>`: scanner output file (token sequence).
- `[language_spec.txt]`: grammar file (defaults to `./data/language1.txt`).

**Output:** a debug file `<basename>_p3dbg.txt` recording every SRA step.

Examples:
```bash
./p3_parser samples/sample1.cscn data/language1.txt   # arithmetic expression
./p3_parser samples/sample4.cscn data/language2.txt   # theory slide 32 grammar
```

---

## Running Tests

```bash
cd build && ctest --output-on-failure
```

Tests cover: DFA, lang_spec, parser_stack, module_args, and SRA integration (both grammars).

---

## Output Format

Each line in `<name>_p3dbg.txt` follows one of these formats:

```
[SHIFT  ] pos=<n>  state=<from>-><to>  token=<name>("<lexeme>")  stack: <trace>
[REDUCE<r>] pos=<n>  state=<from>-><to>  rule=<desc>  stack: <trace>
[ACCEPT ] pos=<n>  state=<s>  ACCEPTED  stack: <trace>
[ERROR  ] pos=<n>  state=<s>  token=<name>  PARSE ERROR
```

`<trace>` shows the state at the bottom, then each pushed symbol as `(state,symbol)`.

---

## Grammar File Format

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
