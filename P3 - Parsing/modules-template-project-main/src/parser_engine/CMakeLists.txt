# -----------------------------------------------------
# src/parser_engine/CMakeLists.txt
# CMakeLists.txt for Parser Engine Module
#
# This file defines the parser_engine static library. It is compiled independently
# and can be tested separately.
# It is included from the top-level CMakeLists.txt via `add_subdirectory(src/parser_engine)`.
# The module is compiled as a static library and can be linked by main programs or tests.
#
# The parser_engine module orchestrates the shift/reduce parsing algorithm,
# coordinating between SRA, Stack, Language, and Token Loader modules.
# -----------------------------------------------------

# Create the static library from the parser_engine source file
add_library(parser_engine parser_engine.c)

# Include the current source directory for header file access
# Since it is PRIVATE, the includes must include the path to the headers
target_include_directories(parser_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link utils because parser_engine depends on utils
target_link_libraries(parser_engine PRIVATE utils)

# TODO: Link other modules when they are created
# Uncomment these as the modules become available:
# target_link_libraries(parser_engine PRIVATE sra)
# target_link_libraries(parser_engine PRIVATE stack)
# target_link_libraries(parser_engine PRIVATE language)
# target_link_libraries(parser_engine PRIVATE token_loader)

# Print a status message during CMake configuration to confirm parser_engine is processed
message(STATUS "(${PROJECT_NAME}) Parser Engine configured: Added as static library")

# Optionally, you can set properties or compile options specific to this module
# For example, enabling additional warnings:
# target_compile_options(parser_engine PRIVATE -Wall -Wextra)
