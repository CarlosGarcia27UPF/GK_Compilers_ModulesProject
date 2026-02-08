# counter module: operation counting system
add_library(counter STATIC counter.c)
target_include_directories(counter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "(${PROJECT_NAME}) counter configured: Added as static library")
