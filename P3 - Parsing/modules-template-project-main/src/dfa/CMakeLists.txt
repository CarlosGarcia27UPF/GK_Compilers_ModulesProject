# -----------------------------------------------------------------------------
# src/dfa/CMakeLists.txt
# Builds the dfa static library.
# -----------------------------------------------------------------------------

add_library(dfa dfa.c)
target_include_directories(dfa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "(${PROJECT_NAME}) dfa configured: Added as static library")
