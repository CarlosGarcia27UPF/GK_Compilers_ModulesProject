/*
 * -----------------------------------------------------------------------------
 * parser_engine.c
 *
 * Implementation of the parser engine module for bottom-up shift/reduce parsing.
 * 
 * This module coordinates the parsing process by:
 * 1. Maintaining the parse state (stack, current token, step count)
 * 2. Querying the SRA for parsing actions
 * 3. Executing shift and reduce operations via stack operations
 * 4. Writing detailed debug traces for each step
 *
 * author: Emil Svensson
 * -----------------------------------------------------------------------------
 */

#include "./parser_engine.h"
#include <stdlib.h>
#include <string.h>

// TODO: Include headers for SRA, Stack, Language, and TokenList modules
// #include "../sra/sra.h"
// #include "../stack/stack.h"
// #include "../language/language.h"
// #include "../token_loader/token_loader.h"

/*
 * Initialize the parser with language specification and tokens
 */
Parser* parser_init(Language* language, TokenList* tokens, const char* output_filename) {
    if (!language || !tokens || !output_filename) {
        fprintf(stderr, "Error: Invalid parameters to parser_init\n");
        return NULL;
    }

    // Allocate parser structure
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        fprintf(stderr, "Error: Failed to allocate parser structure\n");
        return NULL;
    }

    // Initialize fields
    parser->language = language;
    parser->tokens = tokens;
    parser->current_token_index = 0;
    parser->step_count = 0;
    parser->initialized = false;

    // Open debug output file
    parser->debug_output = fopen(output_filename, "w");
    if (!parser->debug_output) {
        fprintf(stderr, "Error: Failed to open output file: %s\n", output_filename);
        free(parser);
        return NULL;
    }

    // TODO: Initialize SRA with language specification
    // parser->sra = sra_init(language);
    parser->sra = NULL; // Placeholder

    // TODO: Initialize stack
    // parser->stack = stack_create();
    parser->stack = NULL; // Placeholder

    if (!parser->sra || !parser->stack) {
        fprintf(stderr, "Error: Failed to initialize SRA or stack\n");
        fclose(parser->debug_output);
        free(parser);
        return NULL;
    }

    // Write header to debug file
    fprintf(parser->debug_output, "================================\n");
    fprintf(parser->debug_output, "PARSER DEBUG TRACE\n");
    fprintf(parser->debug_output, "================================\n");
    fprintf(parser->debug_output, "Input file: %s\n", output_filename);
    fprintf(parser->debug_output, "Grammar: [Grammar name from language]\n");
    fprintf(parser->debug_output, "================================\n\n");

    parser->initialized = true;
    return parser;
}

/*
 * Main parsing function - executes the complete parse
 */
int parse(Parser* parser) {
    if (!parser || !parser->initialized) {
        fprintf(stderr, "Error: Parser not initialized\n");
        return -1;
    }

    fprintf(parser->debug_output, "Starting parse...\n\n");
    
    // TODO: Push initial state (state 0) onto stack
    // stack_push(parser->stack, 0);
    
    // Main parsing loop
    while (true) {
        // TODO: Get current state from stack top
        // int current_state = stack_top_state(parser->stack);
        int current_state = 0; // Placeholder
        
        // TODO: Get current token
        // Token* current_token = parser_get_current_token(parser);
        void* current_token = NULL; // Placeholder
        
        // TODO: Query SRA for action
        // ParserAction action = sra_get_action(parser->sra, current_state, current_token);
        ParserAction action = ACTION_ERROR; // Placeholder
        
        // Execute action based on type
        switch (action) {
            case ACTION_SHIFT: {
                write_parser_trace(parser, "SHIFT");
                if (parser_shift(parser) != 0) {
                    fprintf(stderr, "Error during shift operation\n");
                    return -1;
                }
                break;
            }
            
            case ACTION_REDUCE: {
                // TODO: Get rule to reduce by
                // int rule_id = sra_get_reduce_rule(parser->sra, current_state);
                int rule_id = 0; // Placeholder
                
                char reduce_msg[256];
                snprintf(reduce_msg, sizeof(reduce_msg), "REDUCE by rule %d", rule_id);
                write_parser_trace(parser, reduce_msg);
                
                if (parser_reduce(parser, rule_id) != 0) {
                    fprintf(stderr, "Error during reduce operation\n");
                    return -1;
                }
                break;
            }
            
            case ACTION_ACCEPT: {
                write_parser_trace(parser, "ACCEPT");
                fprintf(parser->debug_output, "\n================================\n");
                fprintf(parser->debug_output, "Parsing completed successfully!\n");
                fprintf(parser->debug_output, "Total steps: %d\n", parser->step_count);
                fprintf(parser->debug_output, "================================\n");
                return 0;
            }
            
            case ACTION_ERROR: {
                write_parser_trace(parser, "ERROR");
                fprintf(parser->debug_output, "\n================================\n");
                fprintf(parser->debug_output, "Parse error encountered!\n");
                fprintf(parser->debug_output, "Step: %d\n", parser->step_count);
                fprintf(parser->debug_output, "================================\n");
                return -1;
            }
        }
        
        parser->step_count++;
        
        // Safety check to prevent infinite loops during development
        if (parser->step_count > 10000) {
            fprintf(stderr, "Error: Parser exceeded maximum steps (possible infinite loop)\n");
            return -1;
        }
    }
    
    return 0;
}

/*
 * Execute a shift operation
 */
int parser_shift(Parser* parser) {
    if (!parser) return -1;
    
    // TODO: Get current token
    // Token* token = parser_get_current_token(parser);
    
    // TODO: Get current state from stack
    // int current_state = stack_top_state(parser->stack);
    
    // TODO: Get next state from SRA based on token
    // int next_state = sra_get_next_state(parser->sra, current_state, token);
    int next_state = 0; // Placeholder
    
    // TODO: Push token and next state onto stack
    // stack_push_token(parser->stack, token, next_state);
    
    // Advance to next token
    parser_advance_token(parser);
    
    return 0;
}

/*
 * Execute a reduce operation
 */
int parser_reduce(Parser* parser, int rule_id) {
    if (!parser) return -1;
    
    // TODO: Get production rule details from language module
    // ProductionRule* rule = language_get_rule(parser->language, rule_id);
    // Symbol lhs = rule->lhs;
    // int rhs_length = rule->rhs_length;
    int rhs_length = 0; // Placeholder
    
    // TODO: Pop rhs_length items from stack
    // stack_pop_n(parser->stack, rhs_length);
    
    // TODO: Get current state after popping
    // int current_state = stack_top_state(parser->stack);
    int current_state = 0; // Placeholder
    
    // TODO: Get goto state for the non-terminal (lhs)
    // int goto_state = sra_get_goto(parser->sra, current_state, lhs);
    int goto_state = 0; // Placeholder
    
    // TODO: Push non-terminal and goto state onto stack
    // stack_push_nonterminal(parser->stack, lhs, goto_state);
    
    return 0;
}

/*
 * Execute one parsing step (shift or reduce)
 */
int parser_step(Parser* parser, ParserAction action) {
    if (!parser) return -1;
    
    switch (action) {
        case ACTION_SHIFT:
            return parser_shift(parser);
        case ACTION_REDUCE:
            // Rule ID would need to be passed separately
            return -1; // Not implemented in this version
        default:
            return -1;
    }
}

/*
 * Write debug trace information for current parsing state
 */
void write_parser_trace(Parser* parser, const char* operation) {
    if (!parser || !operation) return;
    
    fprintf(parser->debug_output, "Step %d: %s\n", parser->step_count, operation);
    
    // TODO: Write current state
    // int current_state = stack_top_state(parser->stack);
    // fprintf(parser->debug_output, "  State: %d\n", current_state);
    fprintf(parser->debug_output, "  State: [PLACEHOLDER]\n");
    
    // TODO: Write current token info
    // Token* token = parser_get_current_token(parser);
    // fprintf(parser->debug_output, "  Token: %s (position %d)\n", 
    //         token_to_string(token), parser->current_token_index);
    fprintf(parser->debug_output, "  Token: [PLACEHOLDER] (position %d)\n", 
            parser->current_token_index);
    
    // TODO: Write full stack contents
    // char* stack_str = stack_to_string(parser->stack);
    // fprintf(parser->debug_output, "  Stack: %s\n", stack_str);
    // free(stack_str);
    fprintf(parser->debug_output, "  Stack: [PLACEHOLDER]\n");
    
    fprintf(parser->debug_output, "\n");
}

/*
 * Get the current token being processed
 */
void* parser_get_current_token(Parser* parser) {
    if (!parser || !parser->tokens) return NULL;
    
    // TODO: Implement token access via token_loader module
    // return token_list_get(parser->tokens, parser->current_token_index);
    return NULL; // Placeholder
}

/*
 * Advance to the next token in the input
 */
bool parser_advance_token(Parser* parser) {
    if (!parser) return false;
    
    // TODO: Check if we can advance
    // if (parser_at_end(parser)) return false;
    
    parser->current_token_index++;
    return true;
}

/*
 * Check if parsing is complete (all tokens consumed)
 */
bool parser_at_end(Parser* parser) {
    if (!parser || !parser->tokens) return true;
    
    // TODO: Check token list length
    // return parser->current_token_index >= token_list_length(parser->tokens);
    return false; // Placeholder
}

/*
 * Clean up parser resources
 */
void parser_cleanup(Parser* parser) {
    if (!parser) return;
    
    // Close debug output file
    if (parser->debug_output) {
        fclose(parser->debug_output);
        parser->debug_output = NULL;
    }
    
    // TODO: Clean up SRA
    // if (parser->sra) {
    //     sra_cleanup(parser->sra);
    // }
    
    // TODO: Clean up stack
    // if (parser->stack) {
    //     stack_destroy(parser->stack);
    // }
    
    // Free parser structure
    free(parser);
}

/*
 * Get a string representation of a parser action
 */
const char* parser_action_to_string(ParserAction action) {
    switch (action) {
        case ACTION_SHIFT:  return "SHIFT";
        case ACTION_REDUCE: return "REDUCE";
        case ACTION_ACCEPT: return "ACCEPT";
        case ACTION_ERROR:  return "ERROR";
        default:            return "UNKNOWN";
    }
}
