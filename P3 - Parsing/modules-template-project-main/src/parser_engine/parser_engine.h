/*
 * -----------------------------------------------------------------------------
 * parser_engine.h 
 *
 * Header file for the parser engine module. This module implements the main
 * shift/reduce parsing algorithm, orchestrating the interaction between the
 * SRA (Shift-Reduce Automaton), stack, token loader, and language modules.
 *
 * Responsibilities:
 * - Main parsing loop coordinating shift/reduce operations
 * - Interfacing with SRA module to get parsing actions (shift/reduce/accept)
 * - Managing stack operations through stack module
 * - Processing tokens from token loader
 * - Writing debug trace output for each parsing step
 *
 * Functions:
 * - `parser_init`: Initialize the parser with language specification and tokens
 * - `parse`: Main entry point for parsing - executes full parse
 * - `parser_step`: Execute one parsing step (shift or reduce)
 * - `parser_cleanup`: Clean up parser resources
 * - `write_parser_trace`: Write debug output for current parsing state
 *
 * Dependencies:
 * - SRA module: for determining parsing actions
 * - Stack module: for maintaining parse stack
 * - Language module: for grammar rules and symbols
 * - Token loader module: for accessing input tokens
 *
 * Team: [Your Team Name]
 * Contributor/s: [Your Name]
 * -----------------------------------------------------------------------------
 */

#ifndef PARSER_ENGINE_H
#define PARSER_ENGINE_H

#include "../main.h"
#include <stdbool.h>

// Forward declarations (actual types defined in respective modules)
typedef struct Language Language;
typedef struct TokenList TokenList;
typedef struct SRA SRA;
typedef struct Stack Stack;

// Parsing action types (returned by SRA)
typedef enum {
    ACTION_SHIFT,
    ACTION_REDUCE,
    ACTION_ACCEPT,
    ACTION_ERROR
} ParserAction;

// Parser state structure
typedef struct {
    SRA* sra;                    // Shift-Reduce Automaton
    Stack* stack;                // Parse stack
    TokenList* tokens;           // Input token list
    Language* language;          // Grammar specification
    int current_token_index;     // Current position in token list
    int step_count;              // Number of steps executed
    FILE* debug_output;          // Debug output file
    bool initialized;            // Parser initialization flag
} Parser;

/*
 * Initialize the parser with language specification and tokens
 * 
 * Parameters:
 *   language: Pointer to grammar specification
 *   tokens: Pointer to token list from scanner
 *   output_filename: Name of debug output file (_p3dbg.txt)
 * 
 * Returns:
 *   Pointer to initialized Parser structure, NULL on failure
 */
Parser* parser_init(Language* language, TokenList* tokens, const char* output_filename);

/*
 * Main parsing function - executes the complete parse
 * 
 * This function implements the shift/reduce parsing algorithm:
 * 1. Push initial state onto stack
 * 2. Loop until accept or error:
 *    a. Get current state from stack top
 *    b. Query SRA for action (shift/reduce/accept/error)
 *    c. Execute action:
 *       - SHIFT: push token and next state
 *       - REDUCE: pop items, get goto state, push non-terminal
 *       - ACCEPT: parsing successful
 *       - ERROR: parsing failed
 *    d. Write trace for each step
 * 
 * Parameters:
 *   parser: Pointer to initialized Parser structure
 * 
 * Returns:
 *   0 on success (accept), -1 on parse error
 */
int parse(Parser* parser);

/*
 * Execute one parsing step (shift or reduce)
 * 
 * Parameters:
 *   parser: Pointer to Parser structure
 *   action: The action to execute (SHIFT or REDUCE)
 * 
 * Returns:
 *   0 on success, -1 on error
 */
int parser_step(Parser* parser, ParserAction action);

/*
 * Execute a shift operation
 * 
 * Parameters:
 *   parser: Pointer to Parser structure
 * 
 * Returns:
 *   0 on success, -1 on error
 */
int parser_shift(Parser* parser);

/*
 * Execute a reduce operation
 * 
 * Parameters:
 *   parser: Pointer to Parser structure
 *   rule_id: Production rule to reduce by
 * 
 * Returns:
 *   0 on success, -1 on error
 */
int parser_reduce(Parser* parser, int rule_id);

/*
 * Write debug trace information for current parsing state
 * 
 * Output format includes:
 * - Step number
 * - Operation (SHIFT/REDUCE/ACCEPT)
 * - Current state
 * - Current token position and value
 * - Full stack contents (symbols and states)
 * - Production rule (for REDUCE operations)
 * 
 * Parameters:
 *   parser: Pointer to Parser structure
 *   operation: String describing the operation ("SHIFT", "REDUCE by rule X", etc.)
 */
void write_parser_trace(Parser* parser, const char* operation);

/*
 * Get the current token being processed
 * 
 * Parameters:
 *   parser: Pointer to Parser structure
 * 
 * Returns:
 *   Pointer to current token, NULL if at end
 */
void* parser_get_current_token(Parser* parser);

/*
 * Advance to the next token in the input
 * 
 * Parameters:
 *   parser: Pointer to Parser structure
 * 
 * Returns:
 *   true if advanced successfully, false if at end
 */
bool parser_advance_token(Parser* parser);

/*
 * Check if parsing is complete (all tokens consumed)
 * 
 * Parameters:
 *   parser: Pointer to Parser structure
 * 
 * Returns:
 *   true if at end of input, false otherwise
 */
bool parser_at_end(Parser* parser);

/*
 * Clean up parser resources
 * 
 * Parameters:
 *   parser: Pointer to Parser structure to clean up
 */
void parser_cleanup(Parser* parser);

/*
 * Get a string representation of a parser action
 * 
 * Parameters:
 *   action: Parser action enum value
 * 
 * Returns:
 *   String representation of the action
 */
const char* parser_action_to_string(ParserAction action);

#endif // PARSER_ENGINE_H
