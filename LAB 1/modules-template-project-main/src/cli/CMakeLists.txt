# -----------------------------------------------------
# src/cli/CMakeLists.txt
# CMakeLists.txt for cli module
# 
# This module handles command-line argument parsing.
# -----------------------------------------------------

add_library(cli STATIC cli.c)
target_include_directories(cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cli PRIVATE utils)
message(STATUS "(${PROJECT_NAME}) cli configured: Added as static library")
