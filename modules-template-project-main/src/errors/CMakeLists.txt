# -----------------------------------------------------
# src/errors/CMakeLists.txt
# CMakeLists.txt for errors module
# 
# This module handles error reporting and diagnostics.
# -----------------------------------------------------

add_library(errors STATIC errors.c)
target_include_directories(errors PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(errors PRIVATE utils)
message(STATUS "(${PROJECT_NAME}) errors configured: Added as static library")
