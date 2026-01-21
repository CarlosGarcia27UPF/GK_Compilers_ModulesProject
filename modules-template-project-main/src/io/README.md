# IO Module Documentation

## Overview
The IO (Input/Output) module handles all file operations for the C preprocessor. It provides functions to read source files into memory buffers, write processed output to files, and generate appropriate output filenames.

## Files
- **io.h** - Header file with function declarations
- **io.c** - Implementation of file I/O operations
- **CMakeLists.txt** - Build configuration for the IO module

## Dependencies
- **buffer/buffer.h**: Uses the `buffer_t` dynamic string buffer for all I/O operations
- Standard C library: `<stdio.h>`, `<string.h>`, `<stdlib.h>`

## Core Functions

### `int io_read_file(const char *path, buffer_t *out)`

**Purpose:** Reads an entire file from disk into a memory buffer.

**Parameters:**
- `path`: Null-terminated string containing the file path to read
- `out`: Pointer to a `buffer_t` structure that will receive the file contents

**Returns:**
- `0`: Success - file was read completely
- `1`: File could not be opened (doesn't exist, no permissions, etc.)
- `2`: Memory allocation failure during buffer append

**Behavior:**
1. Opens file in binary read mode (`"rb"`)
2. Reads file in 4KB chunks
3. Appends each chunk to the output buffer
4. Closes file when complete
5. Returns appropriate error code

**Implementation Details:**

```c
int io_read_file(const char *path, buffer_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return 1;                    // Can't open file
    
    char tmp[4096];                      // 4KB temporary buffer
    size_t n;
    
    while ((n = fread(tmp, 1, sizeof(tmp), f)) > 0) {
        if (buffer_append_n(out, tmp, (long)n) != 0) {
            fclose(f);
            return 2;                    // Buffer append failed
        }
    }
    
    fclose(f);
    return 0;                            // Success
}
```

**Why Binary Mode?**
- Preserves exact byte content (no line-ending conversion)
- Consistent behavior across Windows, Linux, and macOS
- Important for accurate line counting in the preprocessor

**Chunk Reading Strategy:**
- 4KB chunks balance memory usage and I/O efficiency
- Reduces system calls compared to reading byte-by-byte
- Allows processing of arbitrarily large files
- Buffer automatically grows as needed

**Error Handling:**
- File handle is closed on both success and failure paths
- Prevents resource leaks even when buffer allocation fails
- Caller can distinguish between "file not found" and "out of memory"

**Example Usage:**
```c
buffer_t file_contents;
buffer_init(&file_contents);

int result = io_read_file("example.c", &file_contents);
if (result == 1) {
    fprintf(stderr, "Error: Could not open file\n");
} else if (result == 2) {
    fprintf(stderr, "Error: Out of memory\n");
} else {
    // Success! file_contents.data contains the file
    printf("Read %ld bytes\n", file_contents.len);
}

buffer_free(&file_contents);
```

---

### `int io_write_file(const char *path, const buffer_t *in)`

**Purpose:** Writes the contents of a buffer to a file on disk.

**Parameters:**
- `path`: Null-terminated string containing the destination file path
- `in`: Pointer to a `buffer_t` structure containing data to write

**Returns:**
- `0`: Success - buffer was written to file
- `1`: File could not be opened for writing

**Behavior:**
1. Opens file in binary write mode (`"wb"`)
   - Creates file if it doesn't exist
   - Truncates file if it already exists
2. Writes entire buffer in a single `fwrite()` call
3. Closes file
4. Returns status code

**Implementation Details:**

```c
int io_write_file(const char *path, const buffer_t *in)
{
    FILE *f = fopen(path, "wb");
    if (!f) return 1;                    // Can't open for writing
    
    if (in->len > 0) {
        fwrite(in->data, 1, (size_t)in->len, f);
    }
    
    fclose(f);
    return 0;                            // Success
}
```

**Why Single Write?**
- Buffer already contains all data in memory
- Single write is more efficient than multiple small writes
- Simpler error handling (no partial writes to handle)

**Empty Buffer Handling:**
- Checks `in->len > 0` before writing
- Allows writing empty files (0 bytes) safely
- Prevents passing invalid pointers to `fwrite()`

**File Modes:**
- `"wb"` creates or truncates the file
- Binary mode preserves exact byte content
- Overwrites existing files without warning

**Example Usage:**
```c
buffer_t processed_code;
buffer_init(&processed_code);

// ... fill buffer with processed code ...

int result = io_write_file("output_pp.c", &processed_code);
if (result != 0) {
    fprintf(stderr, "Error: Could not write output file\n");
} else {
    printf("Successfully wrote %ld bytes\n", processed_code.len);
}

buffer_free(&processed_code);
```

---

### `int io_make_output_name(const char *input, buffer_t *out_name)`

**Purpose:** Generates an output filename by inserting `_pp` before the file extension.

**Parameters:**
- `input`: Original input filename (e.g., "example.c")
- `out_name`: Buffer to receive the generated output filename

**Returns:**
- `0`: Success - output name generated
- `1`: Buffer operation failed (out of memory)

**Naming Convention:**
- Input: `filename.ext` → Output: `filename_pp.ext`
- Input: `path/to/file.c` → Output: `path/to/file_pp.c`
- Input: `noextension` → Output: `noextension` (no change)

**Implementation Details:**

```c
int io_make_output_name(const char *input, buffer_t *out_name)
{
    // Find last '.' in filename
    const char *dot = strrchr(input, '.');
    if (!dot) {
        // No extension found - just copy input
        return buffer_append_str(out_name, input);
    }
    
    // Calculate length before extension
    long base_len = (long)(dot - input);
    
    // Build: "base" + "_pp" + ".ext"
    if (buffer_append_n(out_name, input, base_len) != 0) return 1;
    if (buffer_append_str(out_name, "_pp") != 0) return 1;
    if (buffer_append_str(out_name, dot) != 0) return 1;
    
    return 0;
}
```

**Algorithm Steps:**

1. **Find Extension:** Use `strrchr()` to find last '.' character
   - Returns pointer to last '.' or NULL if not found
   - Handles filenames with multiple dots (e.g., "file.min.c")

2. **Handle No Extension:** 
   - If no dot found, return input unchanged
   - Example: "makefile" → "makefile"

3. **Calculate Base Length:**
   - Pointer arithmetic: `dot - input` gives character count before extension
   - Cast to `long` to match buffer API

4. **Construct Output:**
   - Append base part (everything before '.')
   - Append "_pp" suffix
   - Append extension (including the '.')

**Examples:**

| Input | Output |
|-------|--------|
| `example.c` | `example_pp.c` |
| `header.h` | `header_pp.h` |
| `complex.min.c` | `complex.min_pp.c` |
| `file` | `file` |
| `/path/to/source.c` | `/path/to/source_pp.c` |
| `test.backup.c` | `test.backup_pp.c` |

**Edge Cases:**

- **Hidden files:** `.gitignore` → `.gitignore` (dot at start, no extension)
- **Multiple dots:** `file.v2.c` → `file.v2_pp.c` (uses last dot)
- **Directory separators:** Preserved in path
- **No extension:** Original filename returned unchanged

**Buffer Safety:**
- Each append operation checks for errors
- Early return on first failure prevents partial output
- Caller receives all-or-nothing result

**Example Usage:**
```c
buffer_t output_filename;
buffer_init(&output_filename);

int result = io_make_output_name("mycode.c", &output_filename);
if (result == 0) {
    printf("Will write to: %s\n", output_filename.data);
    // Prints: "Will write to: mycode_pp.c"
}

buffer_free(&output_filename);
```

---

## Integration with Main Program

The IO module is used in `main.c` to handle all file operations:

```c
int main(int argc, char **argv)
{
    // ... parse command line ...
    
    buffer_t in, out, out_name;
    buffer_init(&in);
    buffer_init(&out);
    buffer_init(&out_name);
    
    // READ: Load source file
    if (io_read_file(in_path, &in) != 0) {
        fprintf(stderr, "Error reading %s\n", in_path);
        return 1;
    }
    
    // GENERATE OUTPUT NAME: example.c → example_pp.c
    if (io_make_output_name(in_path, &out_name) != 0) {
        fprintf(stderr, "Error generating output name\n");
        return 1;
    }
    
    // PROCESS: pp_run() fills 'out' buffer
    pp_run(&ctx, &in, &out, ".");
    
    // WRITE: Save processed output
    io_write_file(out_name.data, &out);
    
    // CLEANUP
    buffer_free(&in);
    buffer_free(&out);
    buffer_free(&out_name);
    
    return 0;
}
```

## Design Patterns and Principles

### 1. **Buffer-Based I/O**
- All data passes through `buffer_t` structures
- Decouples file operations from processing logic
- Enables in-memory testing without touching filesystem
- Allows processing larger-than-memory files (with streaming modifications)

### 2. **Error Code Returns**
- Functions return `0` for success, non-zero for errors
- Different codes distinguish error types
- Enables precise error reporting to users
- Consistent with Unix/POSIX conventions

### 3. **Resource Management**
- Files always closed before return (even on error)
- No resource leaks
- Caller responsible for buffer lifecycle

### 4. **Binary Mode Philosophy**
- All I/O in binary mode for cross-platform consistency
- Preserves exact file content
- Critical for correctly counting lines and positions

### 5. **Chunk-Based Reading**
- 4KB chunks balance performance and memory
- Scales to large files
- Reduces system call overhead

## Testing the IO Module

The IO module can be tested independently:

```c
// test_io.c
#include "io/io.h"
#include "buffer/buffer.h"
#include <assert.h>

void test_read_nonexistent_file() {
    buffer_t buf;
    buffer_init(&buf);
    
    int result = io_read_file("does_not_exist.c", &buf);
    assert(result == 1);  // Should fail to open
    
    buffer_free(&buf);
}

void test_write_then_read() {
    buffer_t original, readback;
    buffer_init(&original);
    buffer_init(&readback);
    
    buffer_append_str(&original, "Hello, World!");
    
    io_write_file("test_temp.txt", &original);
    io_read_file("test_temp.txt", &readback);
    
    assert(readback.len == original.len);
    assert(strcmp(readback.data, original.data) == 0);
    
    buffer_free(&original);
    buffer_free(&readback);
}

void test_output_naming() {
    buffer_t name;
    buffer_init(&name);
    
    io_make_output_name("test.c", &name);
    assert(strcmp(name.data, "test_pp.c") == 0);
    
    buffer_free(&name);
}
```

## Performance Characteristics

### Read Performance
- **Time Complexity:** O(n) where n = file size
- **Space Complexity:** O(n) - entire file in memory
- **I/O Calls:** n/4096 (file size divided by chunk size)

### Write Performance
- **Time Complexity:** O(n) where n = buffer size
- **Space Complexity:** O(1) - no additional allocation
- **I/O Calls:** 1 (single write operation)

### Output Name Generation
- **Time Complexity:** O(n) where n = filename length
- **Space Complexity:** O(n) - output buffer size
- **String Operations:** 1 search + 3 appends

## Error Scenarios and Handling

| Error | Return Code | Possible Causes |
|-------|-------------|-----------------|
| File not found | `1` (read) | Wrong path, typo, file deleted |
| Permission denied | `1` (read/write) | No read/write access |
| Disk full | `1` (write) | No space to write file |
| Out of memory | `2` (read) | Large file, low RAM |
| Invalid path | `1` (write) | Directory doesn't exist |

## Future Enhancements

Possible extensions to the IO module:

1. **Streaming I/O**: Process files line-by-line to handle huge files
2. **Error Messages**: Return detailed error strings, not just codes
3. **Path Validation**: Check paths before attempting I/O
4. **Backup Creation**: Optionally preserve original files
5. **Directory Handling**: Create output directories if needed
6. **Encoding Detection**: Handle different character encodings
7. **Progress Callbacks**: For large file operations

Example streaming enhancement:
```c
// Instead of loading entire file
int io_read_file_streaming(const char *path, 
                           void (*line_callback)(const char *line, void *ctx),
                           void *user_ctx);
```

## Summary

The IO module provides three essential file operations:
- **Read**: Load source files into memory buffers
- **Write**: Save processed output to disk
- **Name**: Generate appropriate output filenames

Key strengths:
- **Simple API**: Three functions, clear responsibilities
- **Robust**: Handles errors gracefully, no resource leaks
- **Efficient**: Chunk-based I/O, minimal allocations
- **Testable**: Pure functions, easy to unit test
- **Portable**: Binary mode ensures cross-platform compatibility

The module serves as a reliable foundation for the preprocessor's file handling needs, keeping I/O concerns separate from preprocessing logic.
