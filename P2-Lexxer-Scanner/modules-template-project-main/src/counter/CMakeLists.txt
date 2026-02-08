# -----------------------------------------------------
# src/counter/CMakeLists.txt
# Counter Module (COUNTCONFIG) - STUB
# TODO: Implement full functionality
# -----------------------------------------------------

add_library(counter STATIC counter.c)

target_include_directories(counter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "(${PROJECT_NAME}) Counter module (STUB) configured: Added as static library")
