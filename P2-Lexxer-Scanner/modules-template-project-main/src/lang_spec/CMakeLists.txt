# -----------------------------------------------------
# src/lang_spec/CMakeLists.txt
# Language Specification Module
# Single source of language constants. No dependencies.
# -----------------------------------------------------

add_library(lang_spec STATIC lang_spec.c)

target_include_directories(lang_spec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "(${PROJECT_NAME}) Lang Spec module configured: Added as static library")
