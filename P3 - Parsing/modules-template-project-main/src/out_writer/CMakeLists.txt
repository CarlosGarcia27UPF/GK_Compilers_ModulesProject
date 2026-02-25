# -----------------------------------------------------------------------------
# src/out_writer/CMakeLists.txt
# Builds the out_writer static library.
# -----------------------------------------------------------------------------

add_library(out_writer out_writer.c)
target_include_directories(out_writer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(out_writer PUBLIC parser_stack lang_spec)
message(STATUS "(${PROJECT_NAME}) out_writer configured: Added as static library")
