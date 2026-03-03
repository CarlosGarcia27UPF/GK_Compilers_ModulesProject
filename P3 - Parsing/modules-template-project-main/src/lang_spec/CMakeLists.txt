# -----------------------------------------------------------------------------
# src/lang_spec/CMakeLists.txt
# Builds the lang_spec static library.
# -----------------------------------------------------------------------------

add_library(lang_spec lang_spec.c)
target_include_directories(lang_spec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lang_spec PUBLIC dfa utils)
message(STATUS "(${PROJECT_NAME}) lang_spec configured: Added as static library")
