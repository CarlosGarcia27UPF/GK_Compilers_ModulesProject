# -----------------------------------------------------------------------------
# src/sra/CMakeLists.txt
# Builds the sra static library.
# -----------------------------------------------------------------------------

add_library(sra sra.c)
target_include_directories(sra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sra PUBLIC dfa lang_spec token_loader parser_stack out_writer)
message(STATUS "(${PROJECT_NAME}) sra configured: Added as static library")
