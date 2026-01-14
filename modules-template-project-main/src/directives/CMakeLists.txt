# -----------------------------------------------------
# src/directives/CMakeLists.txt
# CMakeLists.txt for directives module
# 
# This module handles preprocessor directive processing.
# -----------------------------------------------------

add_library(directives STATIC directives.c)
target_include_directories(directives PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(directives PRIVATE utils)
message(STATUS "(${PROJECT_NAME}) directives configured: Added as static library")
