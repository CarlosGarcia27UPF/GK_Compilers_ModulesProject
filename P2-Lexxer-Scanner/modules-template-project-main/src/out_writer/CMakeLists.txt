# out_writer module: .cscn output formatting and writing
add_library(out_writer STATIC out_writer.c)
target_include_directories(out_writer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(out_writer PUBLIC token_list lang_spec)
message(STATUS "(${PROJECT_NAME}) out_writer configured: Added as static library")
