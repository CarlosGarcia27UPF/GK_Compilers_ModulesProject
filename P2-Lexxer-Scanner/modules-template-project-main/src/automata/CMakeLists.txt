# -----------------------------------------------------
# src/automata/CMakeLists.txt
# Automata Module (Scanner Engine)
# ONLY module allowed to consume input (cs_get)
# All lexical decisions live here
# -----------------------------------------------------

add_library(automata STATIC automata.c)

target_include_directories(automata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(automata PUBLIC 
    char_stream
    lang_spec
    token
)

message(STATUS "(${PROJECT_NAME}) Automata module configured: Added as static library")
