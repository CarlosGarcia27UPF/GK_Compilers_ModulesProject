# C Preprocessor Implementation Summary

## Overview
This implementation provides a functional C preprocessor that handles the core preprocessing directives as specified in the requirements.

## Implemented Features

### 1. Comment Removal (-c flag, default)
- **Single-line comments** (`//`): Removes from `//` to end of line
- **Multi-line comments** (`/* */`): Removes all text between markers
- **Comment replacement**: Each comment is replaced with a single space
- **Newline preservation**: All newlines are preserved, including those inside block comments
- **String literal handling**: Comments inside strings and char literals are not removed

### 2. Directive Processing (-d flag)

#### #include "file"
- Includes local files from the same directory
- Recursive processing: included files are fully preprocessed
- Comments are removed from included files
- Macros are expanded in included content
- Nested includes are supported
- Path validation prevents buffer overflows

#### #define NAME VALUE
- Stores macro definitions in a dynamic table
- Supports redefinition (last definition wins)
- VALUE can be empty or contain any text
- Directive line is not included in output

#### #ifdef NAME / #endif
- Conditional compilation based on macro definitions
- Nested #ifdef blocks are supported (up to 64 levels)
- Code is included only if NAME is defined with #define
- Properly handles nesting with stack-based tracking

### 3. Macro Expansion
- Replaces macro names with their values in code
- Only expands whole identifiers (not partial matches)
- Preserves macros inside string and char literals
- Works with macros defined in current file or included files

### 4. Command-Line Interface
- `-c`: Comment removal only (default when no flags provided)
- `-d`: Process directives and expand macros (keeps comments)
- `-all`: Both comment removal and directive processing
- `-help`: Display usage information
- Flags can be in any order
- Multiple flag combinations supported

### 5. Output File Naming
- Input: `file.c` → Output: `file_pp.c`
- Input: `file.h` → Output: `file_pp.h`
- Preserves directory location of input file

## Module Architecture

All existing modules are properly integrated:

1. **buffer**: Dynamic text storage and manipulation
2. **cli**: Command-line argument parsing and help display
3. **comments**: Comment removal with state management for multi-line blocks
4. **directives**: Directive detection, parsing, and processing
5. **errors**: Error reporting with file name and line number
6. **io**: File reading, writing, and output name generation
7. **macros**: Macro table management and identifier expansion
8. **pp_core**: Main preprocessing engine and orchestration
9. **spec**: Constants and configuration values
10. **tokens**: Available for future tokenization (not required for current implementation)

## Design Decisions

### Single-Pass Architecture
The preprocessor processes the input file line-by-line in a single main pass:
1. Comment removal (if enabled)
2. Directive detection and processing (if enabled)
3. Macro expansion for non-directive lines (if directives enabled)

### State Management
- **Comment state**: Tracks multi-line block comments across lines
- **Macro table**: Dynamic array that grows as needed
- **Ifdef stack**: Stack-based tracking of conditional compilation nesting
- **Line counting**: Accurate line numbers for error reporting

### Error Handling
- Reports errors to stderr with file name and line number
- Continues processing after non-fatal errors
- Validates syntax of directives
- Checks for buffer overflows and path length issues
- Returns non-zero exit code if errors occurred

### Unsupported Directives
Directives not in the specification (e.g., #ifndef, #else, #undef) are:
- Preserved in the output unchanged
- Not treated as errors
- Do not affect processing of supported directives

## Testing

### Test Files Provided
- `input-example.c`: Main test file with comments, macros, and conditionals
- `input-example.h`: Header file with macro definitions
- `test_comprehensive.c`: Additional edge case testing
- `test_header.h`: Simple header for include testing

### Test Scenarios Verified
1. Comment removal preserves code structure
2. Macro definitions are stored and expanded correctly
3. #ifdef blocks correctly include/exclude code
4. #include recursively processes files
5. Flag combinations work as specified
6. Unknown directives are preserved
7. Nested structures work correctly

## Known Limitations

### Not Implemented (as per specification)
- Parametrized macros (marked as optional)
- #ifndef, #else, #elif directives
- #undef directive
- System includes (`#include <stdio.h>`)
- Macro expansion inside directive arguments
- String concatenation and token pasting
- Predefined macros (__LINE__, __FILE__, etc.)

### By Design
- Unsupported directives are preserved, not removed
- #endif without matching #ifdef (for unsupported directives) is kept in output
- Only local file includes are supported

## Security Considerations

### Buffer Overflow Protection
- Path lengths are validated before use
- snprintf return values are checked
- Fixed-size buffers use defined constants
- Dynamic buffers grow as needed

### Input Validation
- Directive syntax is validated
- File paths are checked for length
- Nesting depth is limited
- Null pointer checks throughout

### Resource Management
- All allocated memory is freed
- File handles are properly closed
- Buffer cleanup on error paths

## Performance Characteristics

### Memory Usage
- Dynamic buffers grow exponentially (2x) to minimize reallocations
- Macro table starts at 16 entries, grows as needed
- Included files are fully loaded into memory

### Time Complexity
- O(n) for input file size n
- O(m) for each macro expansion where m is macro count
- Recursive includes add depth factor

## Exit Codes
- 0: Success (no errors)
- 1: Errors occurred during preprocessing
- Non-zero: Fatal error (file not found, invalid input, etc.)

## Example Usage

```bash
# Remove comments only (default)
./preprocessor input.c

# Process directives but keep comments
./preprocessor -d input.c

# Full preprocessing
./preprocessor -all input.c
./preprocessor -c -d input.c  # equivalent

# Show help
./preprocessor -help
```

## File Locations
- Source code: `src/` directory organized by module
- Tests: `tests/` directory
- Build artifacts: `build/` directory (excluded from git)
- Output files: Same directory as input, with `_pp` suffix
