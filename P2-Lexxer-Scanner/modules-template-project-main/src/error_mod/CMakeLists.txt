# error_mod module: error catalog and reporter
add_library(error_mod STATIC error_mod.c)
target_include_directories(error_mod PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "(${PROJECT_NAME}) error_mod configured: Added as static library")
