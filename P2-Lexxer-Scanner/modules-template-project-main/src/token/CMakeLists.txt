# -----------------------------------------------------
# src/token/CMakeLists.txt
# Token module - pure data object, no I/O
# Uses lang_spec for category types
# -----------------------------------------------------

add_library(token STATIC token.c)

target_include_directories(token PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(token PUBLIC lang_spec)

message(STATUS "(${PROJECT_NAME}) Token module configured: Added as static library")
