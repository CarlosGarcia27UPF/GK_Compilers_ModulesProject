# automata module: scanner engine with DFA transition matrix
add_library(automata STATIC automata.c)
target_include_directories(automata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(automata PUBLIC char_stream token_list lang_spec error logger counter)
message(STATUS "(${PROJECT_NAME}) automata configured: Added as static library")
