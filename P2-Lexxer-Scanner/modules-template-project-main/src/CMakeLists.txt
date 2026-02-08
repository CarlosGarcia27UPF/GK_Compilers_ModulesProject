# -----------------------------------------------------------------------------
# src/CMakeLists.txt
# 
# This file includes each module directory (sub-library) so they can be
# built individually with their own CMakeLists.txt.
# Each module (e.g., module_args, module_2) is compiled as a static library to allow
# individual development, compilation, and unit testing in isolation.
#
# By organizing code this way:
# - Each developer works on a separate module with minimal merge conflicts.
# - Modules can be independently tested with their own test_<module>.c files.
# - A main program (optional) can link all modules together for integration.
#
# This CMake file only declares the libraries and their internal includes.
# The main executable and test targets are defined in the top-level and test
# CMakeLists.txt files respectively.
# -----------------------------------------------------------------------------

message(STATUS "(${PROJECT_NAME}) Adding all module subdirectories in src/...")
message(STATUS " - (${PROJECT_NAME}) (src) Configuring source modules and utils...")

# Add utils library from the top-level src folder
add_library(utils STATIC utils_files.c)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "   - (${PROJECT_NAME}) Added utils library")

# ===== P2 SCANNER/LEXER MODULES =====
# Modules are added in dependency order

# 1. lang_spec - no dependencies (single source of language constants)
add_subdirectory(lang_spec)

# 2. char_stream - no dependencies (input cursor)
add_subdirectory(char_stream)

# 3. token - depends on lang_spec
add_subdirectory(token)

# 4. token_list - depends on token
add_subdirectory(token_list)

# 5. automata - depends on char_stream, lang_spec, token
add_subdirectory(automata)

# 6. Stub modules (to be implemented by team members)
add_subdirectory(logger)
add_subdirectory(error)
add_subdirectory(counter)
add_subdirectory(out_writer)

# 7. driver - depends on all modules
add_subdirectory(driver)

message(STATUS "   - (${PROJECT_NAME}) Added P2 Scanner/Lexer modules")

# ===== LEGACY TEMPLATE MODULES =====
add_subdirectory(module_args)
add_subdirectory(module_2)
message(STATUS "   - (${PROJECT_NAME}) Added template modules subdirectories")

# ===== MAIN EXECUTABLE =====
# P2 Scanner main executable
add_executable(modules_template_main main.c)
target_include_directories(modules_template_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(modules_template_main 
    driver
    automata
    token_list
    token
    char_stream
    lang_spec
    out_writer
    logger
    error
    counter
)
message(STATUS "   - (${PROJECT_NAME}) Added main executable")

message(STATUS "(${PROJECT_NAME}) Finished including module subdirectories.")