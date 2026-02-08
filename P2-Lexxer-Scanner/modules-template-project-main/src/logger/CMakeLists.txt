# logger module: debug message router
add_library(logger STATIC logger.c)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "(${PROJECT_NAME}) logger configured: Added as static library")
