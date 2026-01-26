# -----------------------------------------------------
# src/io/CMakeLists.txt
# CMakeLists.txt for io module
# 
# This module handles file input/output operations.
# -----------------------------------------------------

add_library(io STATIC io.c)
target_include_directories(io PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io PRIVATE utils errors)
message(STATUS "(${PROJECT_NAME}) io configured: Added as static library")
