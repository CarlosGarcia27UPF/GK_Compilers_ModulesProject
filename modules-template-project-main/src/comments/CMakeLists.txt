# -----------------------------------------------------
# src/comments/CMakeLists.txt
# CMakeLists.txt for comments module
# 
# This module handles comment removal from source files.
# -----------------------------------------------------

add_library(comments STATIC comments.c)
target_include_directories(comments PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(comments PRIVATE utils)
message(STATUS "(${PROJECT_NAME}) comments configured: Added as static library")
