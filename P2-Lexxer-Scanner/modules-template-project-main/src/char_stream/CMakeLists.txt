# -----------------------------------------------------
# src/char_stream/CMakeLists.txt
# Character Stream Module (Input Cursor)
# Cursor only. Never classifies, skips, or groups characters.
# -----------------------------------------------------

add_library(char_stream STATIC char_stream.c)

target_include_directories(char_stream PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "(${PROJECT_NAME}) Char Stream module configured: Added as static library")
