/*
 * tests/test_io.c
 *
 * Test program for the IO module.
 * Tests: io_read_file, io_write_file, io_make_output_name
 * 
 */

#include "test_io.h"
#include <unistd.h>

/* Helper: Create a temporary test file */
static void create_temp_file(const char *filename, const char *content) {
    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "%s", content);
        fclose(f);
    }
}

/* Helper: Delete a file */
static void delete_file(const char *filename) {
    unlink(filename);
}

/* Test 1: io_read_file - Read file contents into buffer */
void test_io_read_file(void) {
    printf("Test 1: io_read_file\n");
    
    const char *test_file = "test_read.txt";
    const char *test_content = "Hello, World!\nLine 2\n";
    
    create_temp_file(test_file, test_content);
    
    buffer_t buf;
    buffer_init(&buf);
    
    int result = io_read_file(test_file, &buf);
    assert(result == 0);
    assert(buf.len == strlen(test_content));
    assert(strncmp(buf.data, test_content, buf.len) == 0);
    
    printf("  [PASS] Read file successfully\n");
    
    buffer_free(&buf);
    delete_file(test_file);
}

/* Test 2: io_write_file - Write buffer contents to file */
void test_io_write_file(void) {
    printf("Test 2: io_write_file\n");
    
    const char *test_file = "test_write.txt";
    const char *test_content = "Test output content";
    
    buffer_t buf;
    buffer_init(&buf);
    buffer_append_str(&buf, test_content);
    
    int result = io_write_file(test_file, &buf);
    assert(result == 0);
    
    /* Verify file was written */
    FILE *f = fopen(test_file, "r");
    assert(f != NULL);
    
    char read_content[256];
    size_t n = fread(read_content, 1, sizeof(read_content) - 1, f);
    read_content[n] = '\0';
    fclose(f);
    
    assert(strcmp(read_content, test_content) == 0);
    printf("  [PASS] Written file verified\n");
    
    buffer_free(&buf);
    delete_file(test_file);
}

/* Test 3: io_make_output_name - Generate _pp filename with extension */
void test_io_make_output_name_with_ext(void) {
    printf("Test 3: io_make_output_name (with extension)\n");
    
    buffer_t out_name;
    buffer_init(&out_name);
    
    int result = io_make_output_name("myfile.c", &out_name);
    assert(result == 0);
    assert(strcmp(out_name.data, "myfile_pp.c") == 0);
    
    printf("  [PASS] Generated: %s\n", out_name.data);
    
    buffer_free(&out_name);
}

/* Test 4: io_make_output_name - No extension returns original */
void test_io_make_output_name_no_ext(void) {
    printf("Test 4: io_make_output_name (no extension)\n");
    
    buffer_t out_name;
    buffer_init(&out_name);
    
    int result = io_make_output_name("myfile", &out_name);
    assert(result == 0);
    /* When no extension, returns the input as-is */
    assert(strcmp(out_name.data, "myfile") == 0);
    
    printf("  [PASS] Generated: %s\n", out_name.data);
    
    buffer_free(&out_name);
}

/* Test 5: io_make_output_name - Handle multiple extensions */
void test_io_make_output_name_multi_dot(void) {
    printf("Test 5: io_make_output_name (multiple dots)\n");
    
    buffer_t out_name;
    buffer_init(&out_name);
    
    int result = io_make_output_name("archive.tar.gz", &out_name);
    assert(result == 0);
    assert(strcmp(out_name.data, "archive.tar_pp.gz") == 0);
    
    printf("  [PASS] Generated: %s\n", out_name.data);
    
    buffer_free(&out_name);
}

/* Test 6: Round-trip - Read, write, re-read */
void test_io_roundtrip(void) {
    printf("Test 6: Round-trip (read -> write -> read)\n");
    
    const char *input_file = "test_input.txt";
    const char *output_file = "test_input_pp.txt";
    const char *test_content = "Line 1\nLine 2\nLine 3\n";
    
    /* Step 1: Create input file */
    create_temp_file(input_file, test_content);
    
    /* Step 2: Read input */
    buffer_t buf;
    buffer_init(&buf);
    int result = io_read_file(input_file, &buf);
    assert(result == 0);
    
    /* Step 3: Write output */
    result = io_write_file(output_file, &buf);
    assert(result == 0);
    
    /* Step 4: Re-read output */
    buffer_t buf2;
    buffer_init(&buf2);
    result = io_read_file(output_file, &buf2);
    assert(result == 0);
    
    /* Step 5: Verify content matches */
    assert(buf.len == buf2.len);
    assert(memcmp(buf.data, buf2.data, buf.len) == 0);
    
    printf("  [PASS] Round-trip successful\n");
    
    buffer_free(&buf);
    buffer_free(&buf2);
    delete_file(input_file);
    delete_file(output_file);
}

int main(void) {
    printf("=== IO Module Test Suite ===\n\n");
    
    test_io_read_file();
    test_io_write_file();
    test_io_make_output_name_with_ext();
    test_io_make_output_name_no_ext();
    test_io_make_output_name_multi_dot();
    test_io_roundtrip();
    
    printf("\n=== All IO tests passed! ===\n\n");
    
    return 0;
}
