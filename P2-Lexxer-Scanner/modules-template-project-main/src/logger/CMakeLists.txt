# -----------------------------------------------------
# src/logger/CMakeLists.txt
# Logger Module (Debug Router) - STUB
# TODO: Implement full functionality
# -----------------------------------------------------

add_library(logger STATIC logger.c)

target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "(${PROJECT_NAME}) Logger module (STUB) configured: Added as static library")
