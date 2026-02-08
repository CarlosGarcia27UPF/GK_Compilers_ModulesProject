# token_list module: ordered token stream storage
add_library(token_list STATIC token_list.c)
target_include_directories(token_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(token_list PUBLIC token)
message(STATUS "(${PROJECT_NAME}) token_list configured: Added as static library")
