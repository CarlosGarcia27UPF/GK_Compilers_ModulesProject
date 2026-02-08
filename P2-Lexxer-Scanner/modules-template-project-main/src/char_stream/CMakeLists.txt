# char_stream module: input cursor with peek/get and line/col tracking
add_library(char_stream STATIC char_stream.c)
target_include_directories(char_stream PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "(${PROJECT_NAME}) char_stream configured: Added as static library")
