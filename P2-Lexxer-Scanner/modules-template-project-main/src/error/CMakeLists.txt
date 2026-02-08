# -----------------------------------------------------
# src/error/CMakeLists.txt
# Error Module (Catalog + Reporter) - STUB
# TODO: Implement full functionality
# -----------------------------------------------------

add_library(error STATIC error.c)

target_include_directories(error PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "(${PROJECT_NAME}) Error module (STUB) configured: Added as static library")
