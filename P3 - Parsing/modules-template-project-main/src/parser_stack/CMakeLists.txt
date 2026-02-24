# -----------------------------------------------------------------------------
# src/parser_stack/CMakeLists.txt
# Builds the parser_stack static library.
# -----------------------------------------------------------------------------

add_library(parser_stack parser_stack.c)
target_include_directories(parser_stack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(parser_stack PUBLIC lang_spec)
message(STATUS "(${PROJECT_NAME}) parser_stack configured: Added as static library")
