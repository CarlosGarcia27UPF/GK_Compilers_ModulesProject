# -----------------------------------------------------
# src/token_list/CMakeLists.txt
# Token List Module (Ordered Token Stream)
# Stores tokens only. No formatting or scanning logic.
# -----------------------------------------------------

add_library(token_list STATIC token_list.c)

target_include_directories(token_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(token_list PUBLIC token)

message(STATUS "(${PROJECT_NAME}) Token List module configured: Added as static library")
