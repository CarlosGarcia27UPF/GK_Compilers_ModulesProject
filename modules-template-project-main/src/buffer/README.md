# Buffer Module Documentation

## Overview
The Buffer module provides a dynamic, resizable string buffer implementation that serves as the fundamental data structure for text manipulation throughout the C preprocessor. It offers automatic memory management with exponential growth strategy for efficient string operations.

## Files
- **buffer.h** - Header file with structure definition and function declarations
- **buffer.c** - Implementation of dynamic buffer operations
- **CMakeLists.txt** - Build configuration for the buffer module

## Core Data Structure

### `buffer_t`
```c
typedef struct {
    char *data;    // Pointer to dynamically allocated character array
    long len;      // Current length of content (excluding null terminator)
    long cap;      // Total allocated capacity
} buffer_t;
```

**Field Descriptions:**

#### `char *data`
- **Type:** Pointer to dynamically allocated memory
- **Purpose:** Holds the actual string content
- **Null Termination:** Always null-terminated (when properly used)
- **Initial State:** NULL until first allocation
- **Memory Management:** Managed by buffer_grow(), allocated via malloc/realloc

#### `long len`
- **Type:** Long integer
- **Purpose:** Tracks the current length of meaningful content
- **Excludes:** The null terminator (not counted in len)
- **Range:** 0 to cap-1
- **Updates:** Incremented by append operations

#### `long cap`
- **Type:** Long integer  
- **Purpose:** Total allocated capacity in bytes
- **Includes:** Space for content AND null terminator
- **Growth:** Doubles when capacity is exceeded
- **Initial:** 64 bytes (after buffer_init)

**Invariant:**
```
Always: len < cap
Always: data[len] == '\0' (if data != NULL)
```

---

## Public Functions

### `void buffer_init(buffer_t *b)`

**Purpose:** Initializes a buffer to empty state with initial capacity allocation.

**Parameters:**
- `b`: Pointer to buffer structure to initialize

**Returns:** void (no return value)

**Behavior:**
1. Sets data to NULL
2. Sets len and cap to 0
3. Allocates initial capacity (64 bytes)
4. Sets first byte to '\0' for empty string

**Initial Capacity:** 64 bytes (defined by first buffer_grow call)

**Implementation:**
```c
void buffer_init(buffer_t *b)
{
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
    
    /* Allocate initial capacity so data is always valid */
    buffer_grow(b, 1);
    if (b->data) b->data[0] = '\0';
}
```

**After Initialization:**
```
b->data → ['\0', ...] (64 bytes allocated)
b->len  = 0
b->cap  = 64
```

**Example Usage:**
```c
buffer_t buf;
buffer_init(&buf);

// Now safe to use:
buffer_append_str(&buf, "Hello");
printf("%s\n", buf.data);  // Prints: Hello

buffer_free(&buf);
```

**Why Initialize?**
- Ensures data pointer is never NULL
- Prevents segmentation faults
- Provides consistent initial state
- Ready to use immediately after init

---

### `void buffer_free(buffer_t *b)`

**Purpose:** Releases all memory allocated by the buffer and resets to initial state.

**Parameters:**
- `b`: Pointer to buffer to free (can be NULL)

**Returns:** void

**Behavior:**
1. Checks if buffer pointer is NULL (safe to call on NULL)
2. Frees the data array
3. Resets all fields to zero
4. Does NOT free the buffer_t structure itself

**Implementation:**
```c
void buffer_free(buffer_t *b)
{
    if (!b) return;      // Safe on NULL
    free(b->data);       // Release memory
    b->data = NULL;      // Prevent dangling pointer
    b->len = 0;          // Reset length
    b->cap = 0;          // Reset capacity
}
```

**Safety Features:**
- NULL-safe: Can call on NULL pointer
- Prevents double-free: Sets data to NULL after freeing
- Idempotent: Safe to call multiple times
- Reusable: Buffer can be re-initialized after free

**Example Usage:**
```c
buffer_t buf;
buffer_init(&buf);
buffer_append_str(&buf, "some text");

// When done:
buffer_free(&buf);
// buf is now safe but empty

// Can re-initialize if needed:
buffer_init(&buf);
buffer_append_str(&buf, "new text");
buffer_free(&buf);
```

**Memory Leak Prevention:**
```c
// GOOD: Proper cleanup
buffer_t *buf = malloc(sizeof(buffer_t));
buffer_init(buf);
buffer_append_str(buf, "text");
buffer_free(buf);  // Frees buf->data
free(buf);         // Frees buf structure

// BAD: Memory leak
buffer_t *buf = malloc(sizeof(buffer_t));
buffer_init(buf);
buffer_append_str(buf, "text");
free(buf);  // LEAK! buf->data not freed
```

---

### `int buffer_append_char(buffer_t *b, char c)`

**Purpose:** Appends a single character to the end of the buffer.

**Parameters:**
- `b`: Buffer to append to
- `c`: Character to append

**Returns:**
- `0`: Success
- `1`: Failure (NULL buffer or allocation failure)

**Behavior:**
1. Validates buffer pointer
2. Ensures capacity for char + null terminator
3. Appends character
4. Updates length
5. Maintains null termination

**Implementation:**
```c
int buffer_append_char(buffer_t *b, char c)
{
    if (!b) return 1;
    
    /* +1 for char, +1 for '\0' */
    if (buffer_grow(b, b->len + 2) != 0) return 1;
    
    b->data[b->len] = c;
    b->len++;
    b->data[b->len] = '\0';
    return 0;
}
```

**Capacity Calculation:**
```
Need: len + 1 (for char) + 1 (for '\0') = len + 2
Example:
  Before: len=5, cap=64
  After:  len=6, cap=64  (no growth needed)
  
  Before: len=63, cap=64
  After:  len=64, cap=128 (growth triggered)
```

**Example Usage:**
```c
buffer_t buf;
buffer_init(&buf);

buffer_append_char(&buf, 'A');
buffer_append_char(&buf, 'B');
buffer_append_char(&buf, 'C');

printf("%s\n", buf.data);  // Prints: ABC
printf("Length: %ld\n", buf.len);  // Prints: Length: 3

buffer_free(&buf);
```

**Use Cases:**
- Building strings character by character
- Processing text streams one char at a time
- Implementing character-based filters

---

### `int buffer_append_n(buffer_t *b, const char *s, long n)`

**Purpose:** Appends exactly n bytes from a string to the buffer.

**Parameters:**
- `b`: Buffer to append to
- `s`: Source string (can contain embedded nulls)
- `n`: Number of bytes to copy

**Returns:**
- `0`: Success
- `1`: Failure (invalid arguments or allocation failure)

**Behavior:**
1. Validates all inputs (buffer, string, n >= 0)
2. Handles n=0 as no-op (success)
3. Grows buffer if needed
4. Copies n bytes using memcpy (fast)
5. Updates length
6. Adds null terminator

**Implementation:**
```c
int buffer_append_n(buffer_t *b, const char *s, long n)
{
    if (!b || !s || n < 0) return 1;
    
    if (n == 0) return 0;  // No-op
    
    /* +n for content, +1 for '\0' */
    if (buffer_grow(b, b->len + n + 1) != 0) return 1;
    
    memcpy(b->data + b->len, s, (size_t)n);
    b->len += n;
    b->data[b->len] = '\0';
    return 0;
}
```

**Why memcpy Instead of strcpy?**
- **Faster:** No need to search for null terminator
- **Binary Safe:** Can copy data with embedded nulls
- **Precise:** Copies exactly n bytes, no more, no less

**Example Usage:**
```c
buffer_t buf;
buffer_init(&buf);

// Append first 5 characters
const char *text = "Hello, World!";
buffer_append_n(&buf, text, 5);
printf("%s\n", buf.data);  // Prints: Hello

// Append middle portion
buffer_append_n(&buf, text + 7, 5);
printf("%s\n", buf.data);  // Prints: HelloWorld

buffer_free(&buf);
```

**Handling Binary Data:**
```c
buffer_t buf;
buffer_init(&buf);

// Can store data with embedded nulls
char binary[] = {'A', '\0', 'B', '\0', 'C'};
buffer_append_n(&buf, binary, 5);

printf("Length: %ld\n", buf.len);  // Prints: 5
// buf.data contains: A \0 B \0 C \0
```

**Use Cases:**
- Appending substrings without null termination
- Copying file chunks
- Processing fixed-length fields
- Binary data handling

---

### `int buffer_append_str(buffer_t *b, const char *s)`

**Purpose:** Appends a null-terminated string to the buffer.

**Parameters:**
- `b`: Buffer to append to
- `s`: Null-terminated string to append

**Returns:**
- `0`: Success
- `1`: Failure (NULL string or allocation failure)

**Behavior:**
1. Validates string pointer
2. Calculates string length with strlen
3. Delegates to buffer_append_n

**Implementation:**
```c
int buffer_append_str(buffer_t *b, const char *s)
{
    if (!s) return 1;
    return buffer_append_n(b, s, (long)strlen(s));
}
```

**Why So Simple?**
- Reuses buffer_append_n logic (DRY principle)
- Single point of truth for append behavior
- Minimal code duplication

**Example Usage:**
```c
buffer_t buf;
buffer_init(&buf);

buffer_append_str(&buf, "Hello");
buffer_append_str(&buf, " ");
buffer_append_str(&buf, "World");
buffer_append_str(&buf, "!");

printf("%s\n", buf.data);  // Prints: Hello World!

buffer_free(&buf);
```

**Building Complex Strings:**
```c
buffer_t code;
buffer_init(&code);

buffer_append_str(&code, "int main() {\n");
buffer_append_str(&code, "    printf(\"Hello, World!\\n\");\n");
buffer_append_str(&code, "    return 0;\n");
buffer_append_str(&code, "}\n");

// code.data now contains a complete C program
```

**Use Cases:**
- Concatenating multiple strings
- Building output text
- Formatting messages
- Most common buffer operation

---

## Private Functions

### `static int buffer_grow(buffer_t *b, long min_capacity)`

**Purpose:** Internal function that grows buffer capacity using exponential growth strategy.

**Visibility:** Static (file-scope only, not in header)

**Parameters:**
- `b`: Buffer to grow
- `min_capacity`: Minimum capacity needed

**Returns:**
- `0`: Success (capacity is sufficient)
- `1`: Allocation failure

**Growth Strategy:**

**Initial Capacity:** 64 bytes
```c
long new_cap = (b->cap == 0) ? 64 : b->cap;
```

**Doubling Strategy:**
```c
while (new_cap < min_capacity) {
    new_cap *= 2;
}
```

**Example Growth Sequence:**
```
Request  Current  New Cap   Growth
------   -------  -------   ------
1 byte   0        64        Initial
65 bytes 64       128       x2
200 bytes 128     256       x2
300 bytes 256     512       x2
1000 bytes 512    1024      x2
```

**Implementation:**
```c
static int buffer_grow(buffer_t *b, long min_capacity)
{
    // Already have enough?
    if (b->cap >= min_capacity) return 0;
    
    // Start with 64 or current capacity
    long new_cap = (b->cap == 0) ? 64 : b->cap;
    
    // Double until sufficient
    while (new_cap < min_capacity) {
        new_cap *= 2;
    }
    
    // Reallocate
    char *new_data = (char *)realloc(b->data, (size_t)new_cap);
    if (!new_data) return 1;  // Allocation failed
    
    // Update buffer
    b->data = new_data;
    b->cap = new_cap;
    return 0;
}
```

**Why Exponential Growth?**

**Performance:**
```
Linear growth (add 100 each time):
  For 10,000 bytes: 100 reallocations
  O(n) reallocations

Exponential growth (double):
  For 10,000 bytes: ~7 reallocations
  O(log n) reallocations
```

**Memory vs. Speed Tradeoff:**
- **Pro:** Very fast - minimal reallocations
- **Pro:** Amortized O(1) append operations
- **Con:** May waste up to 50% memory (if you only use half the capacity)
- **Standard:** Used by std::vector, Java ArrayList, Python list

**Allocation Safety:**
```c
char *new_data = (char *)realloc(b->data, (size_t)new_cap);
if (!new_data) return 1;  // IMPORTANT: Don't lose old data

// WRONG:
// b->data = realloc(b->data, new_cap);
// if (!b->data) return 1;  // Lost old data if realloc failed!
```

---

## Design Patterns and Principles

### 1. **Dynamic Array Pattern**
Buffer implements a classic resizable array:
- Automatic growth when capacity exceeded
- Exponential growth for efficiency
- Maintains null termination for C string compatibility

### 2. **RAII-like Resource Management**
```c
buffer_init()  // Acquire resources
// Use buffer
buffer_free()  // Release resources
```

### 3. **Defensive Programming**
Every public function validates inputs:
```c
if (!b) return 1;      // NULL buffer check
if (!s) return 1;      // NULL string check
if (n < 0) return 1;   // Invalid length check
```

### 4. **Single Responsibility**
Each function has one clear purpose:
- `buffer_init()` - Initialize
- `buffer_free()` - Cleanup
- `buffer_append_*()` - Add data
- `buffer_grow()` - Manage capacity

### 5. **DRY (Don't Repeat Yourself)**
```c
// buffer_append_str delegates to buffer_append_n
int buffer_append_str(buffer_t *b, const char *s) {
    return buffer_append_n(b, s, strlen(s));
}
```

### 6. **Null Termination Guarantee**
Every append operation maintains null termination:
```c
b->data[b->len] = '\0';  // Always after updating len
```

This allows safe use with C string functions:
```c
printf("%s", buf.data);   // Always safe
strlen(buf.data);         // Always works
strcmp(buf.data, "x");    // No surprises
```

---

## Performance Characteristics

### Time Complexity

| Operation | Average | Worst Case | Notes |
|-----------|---------|------------|-------|
| `buffer_init()` | O(1) | O(1) | Single allocation |
| `buffer_free()` | O(1) | O(1) | Single deallocation |
| `buffer_append_char()` | O(1)* | O(n) | *Amortized |
| `buffer_append_n()` | O(n)* | O(n) | n = bytes copied |
| `buffer_append_str()` | O(n)* | O(n) | n = string length |

**Amortized O(1) for append_char:**
```
Over n appends:
  Total reallocations: O(log n)
  Total copies: O(n)
  Average per append: O(n)/n = O(1)
```

### Space Complexity

**Memory Usage:**
```
Best case:  len ≈ cap (just filled)
Worst case: len ≈ cap/2 (just after growth)
Average:    len ≈ 3*cap/4
```

**Example:**
```
After appending 1000 bytes:
  len = 1000
  cap = 1024 (last growth from 512 to 1024)
  waste = 24 bytes (2.4%)
```

**Memory Overhead:**
```c
sizeof(buffer_t) = sizeof(char*) + 2*sizeof(long)
                 = 8 + 16 = 24 bytes (on 64-bit)
```

---

## Common Usage Patterns

### Pattern 1: Build String from Parts
```c
buffer_t msg;
buffer_init(&msg);

buffer_append_str(&msg, "Error on line ");
buffer_append_str(&msg, "42");
buffer_append_str(&msg, ": ");
buffer_append_str(&msg, "undefined variable");

fprintf(stderr, "%s\n", msg.data);
buffer_free(&msg);
```

### Pattern 2: Read File into Buffer
```c
buffer_t content;
buffer_init(&content);

FILE *f = fopen("file.txt", "rb");
char chunk[4096];
size_t n;

while ((n = fread(chunk, 1, sizeof(chunk), f)) > 0) {
    buffer_append_n(&content, chunk, n);
}

fclose(f);
// Use content.data
buffer_free(&content);
```

### Pattern 3: Process Stream Character by Character
```c
buffer_t filtered;
buffer_init(&filtered);

for (const char *p = input; *p; p++) {
    if (isalpha(*p)) {
        buffer_append_char(&filtered, *p);
    }
}

// filtered.data contains only alphabetic characters
buffer_free(&filtered);
```

### Pattern 4: Reusable Buffer
```c
buffer_t temp;
buffer_init(&temp);

for (int i = 0; i < 10; i++) {
    temp.len = 0;  // Reset length, keep capacity
    temp.data[0] = '\0';
    
    buffer_append_str(&temp, "Item ");
    // ... build string ...
    
    process(temp.data);
}

buffer_free(&temp);
```

---

## Integration with Other Modules

### Used By

**IO Module:**
```c
// io.c
int io_read_file(const char *path, buffer_t *out) {
    // Reads file into buffer
    buffer_append_n(out, chunk, bytes_read);
}
```

**PP_CORE Module:**
```c
// pp_core.c
int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output) {
    // Processes input buffer, writes to output buffer
    buffer_append_n(output, line, line_len);
}
```

**Main Program:**
```c
// main.c
buffer_t in, out, out_name;
buffer_init(&in);
buffer_init(&out);
buffer_init(&out_name);

io_read_file(path, &in);
pp_run(&ctx, &in, &out, ".");
io_write_file(out_name.data, &out);

buffer_free(&in);
buffer_free(&out);
buffer_free(&out_name);
```

---

## Testing Strategies

### Unit Tests

```c
void test_init_and_free() {
    buffer_t b;
    buffer_init(&b);
    assert(b.data != NULL);
    assert(b.len == 0);
    assert(b.cap > 0);
    buffer_free(&b);
    assert(b.data == NULL);
}

void test_append_char() {
    buffer_t b;
    buffer_init(&b);
    
    buffer_append_char(&b, 'A');
    assert(b.len == 1);
    assert(b.data[0] == 'A');
    assert(b.data[1] == '\0');
    
    buffer_free(&b);
}

void test_append_string() {
    buffer_t b;
    buffer_init(&b);
    
    buffer_append_str(&b, "Hello");
    assert(b.len == 5);
    assert(strcmp(b.data, "Hello") == 0);
    
    buffer_append_str(&b, " World");
    assert(b.len == 11);
    assert(strcmp(b.data, "Hello World") == 0);
    
    buffer_free(&b);
}

void test_growth() {
    buffer_t b;
    buffer_init(&b);
    long initial_cap = b.cap;
    
    // Fill beyond initial capacity
    for (int i = 0; i < initial_cap; i++) {
        buffer_append_char(&b, 'x');
    }
    
    assert(b.cap > initial_cap);  // Grew!
    assert(b.len == initial_cap);
    
    buffer_free(&b);
}

void test_null_safety() {
    buffer_t b;
    buffer_init(&b);
    
    assert(buffer_append_str(&b, NULL) == 1);  // Fails safely
    assert(buffer_append_n(&b, NULL, 10) == 1);
    
    buffer_free(&b);
}
```

---

## Common Pitfalls and Solutions

### Pitfall 1: Forgetting to Initialize
```c
// WRONG:
buffer_t b;
buffer_append_str(&b, "text");  // CRASH! Uninitialized

// CORRECT:
buffer_t b;
buffer_init(&b);
buffer_append_str(&b, "text");
buffer_free(&b);
```

### Pitfall 2: Forgetting to Free
```c
// WRONG:
void process() {
    buffer_t b;
    buffer_init(&b);
    buffer_append_str(&b, "text");
    // return without buffer_free() - MEMORY LEAK!
}

// CORRECT:
void process() {
    buffer_t b;
    buffer_init(&b);
    buffer_append_str(&b, "text");
    buffer_free(&b);
}
```

### Pitfall 3: Using After Free
```c
// WRONG:
buffer_t b;
buffer_init(&b);
buffer_append_str(&b, "text");
buffer_free(&b);
printf("%s", b.data);  // CRASH! Use after free

// CORRECT:
buffer_t b;
buffer_init(&b);
buffer_append_str(&b, "text");
printf("%s", b.data);  // Use before free
buffer_free(&b);
```

### Pitfall 4: Modifying len Directly
```c
// WRONG:
b.len = 0;  // Reset length
// Problem: data[0] might not be '\0'

// CORRECT:
b.len = 0;
b.data[0] = '\0';  // Maintain null termination
```

---

## Future Enhancements

Possible extensions:

### 1. Reserve Capacity
```c
int buffer_reserve(buffer_t *b, long capacity);
// Pre-allocate to avoid growth during known operations
```

### 2. Shrink to Fit
```c
int buffer_shrink_to_fit(buffer_t *b);
// Reduce capacity to len + 1, freeing excess memory
```

### 3. Insert/Delete Operations
```c
int buffer_insert(buffer_t *b, long pos, const char *s);
int buffer_delete(buffer_t *b, long pos, long n);
```

### 4. Format String Support
```c
int buffer_printf(buffer_t *b, const char *fmt, ...);
// Like sprintf but appends to buffer
```

### 5. View/Substring
```c
typedef struct {
    const char *data;
    long len;
} buffer_view_t;

buffer_view_t buffer_substr(const buffer_t *b, long start, long len);
// Non-owning view into buffer
```

---

## Summary

The Buffer module provides essential dynamic string functionality:

**Core Operations:**
- **Initialize:** `buffer_init()` - Set up for use
- **Append:** `buffer_append_char/n/str()` - Add content
- **Free:** `buffer_free()` - Clean up

**Key Strengths:**
- **Automatic Growth:** No manual capacity management
- **Null Termination:** Always C-string compatible
- **Efficient:** Amortized O(1) append operations
- **Safe:** Defensive input validation
- **Simple:** Clean, focused API

**Design Excellence:**
- Exponential growth for performance
- Null-safe operations
- Clear ownership semantics
- Single responsibility functions
- Minimal dependencies (just stdlib)

The buffer module is the workhorse of the preprocessor, enabling efficient text manipulation without manual memory management complexity.
