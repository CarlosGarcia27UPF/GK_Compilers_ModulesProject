# -----------------------------------------------------
# src/macros/CMakeLists.txt
# CMakeLists.txt for macros module
# 
# This module handles macro storage and replacement.
# -----------------------------------------------------

add_library(macros STATIC macros.c)
target_include_directories(macros PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(macros PRIVATE utils)
message(STATUS "(${PROJECT_NAME}) macros configured: Added as static library")
