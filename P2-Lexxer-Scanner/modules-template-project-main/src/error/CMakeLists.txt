# error module: error catalog and reporter
add_library(error STATIC error.c)
target_include_directories(error PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "(${PROJECT_NAME}) error configured: Added as static library")
