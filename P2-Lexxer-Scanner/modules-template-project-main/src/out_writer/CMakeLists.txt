# -----------------------------------------------------
# src/out_writer/CMakeLists.txt
# Output Writer Module (.cscn formatting) - STUB
# TODO: Implement full functionality
# -----------------------------------------------------

add_library(out_writer STATIC out_writer.c)

target_include_directories(out_writer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(out_writer PUBLIC token_list lang_spec)

message(STATUS "(${PROJECT_NAME}) Out Writer module (STUB) configured: Added as static library")
