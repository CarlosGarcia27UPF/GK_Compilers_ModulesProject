# -----------------------------------------------------------------------------
# src/token_loader/CMakeLists.txt
# Builds the token_loader static library.
# -----------------------------------------------------------------------------

add_library(token_loader token_loader.c)
target_include_directories(token_loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(token_loader PUBLIC lang_spec)
message(STATUS "(${PROJECT_NAME}) token_loader configured: Added as static library")
