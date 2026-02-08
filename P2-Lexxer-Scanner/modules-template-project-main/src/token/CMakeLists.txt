# token module: token data object
add_library(token STATIC token.c)
target_include_directories(token PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(token PUBLIC lang_spec)
message(STATUS "(${PROJECT_NAME}) token configured: Added as static library")
