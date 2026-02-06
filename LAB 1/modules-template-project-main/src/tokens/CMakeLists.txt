# -----------------------------------------------------
# src/tokens/CMakeLists.txt
# CMakeLists.txt for tokens module
# 
# This module handles tokenization/lexing of source files.
# -----------------------------------------------------

add_library(tokens STATIC tokens.c)
target_include_directories(tokens PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tokens PRIVATE utils)
message(STATUS "(${PROJECT_NAME}) tokens configured: Added as static library")
