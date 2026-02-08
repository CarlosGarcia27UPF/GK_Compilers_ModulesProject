# lang_spec module: language specification constants and helpers
add_library(lang_spec STATIC lang_spec.c)
target_include_directories(lang_spec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "(${PROJECT_NAME}) lang_spec configured: Added as static library")
