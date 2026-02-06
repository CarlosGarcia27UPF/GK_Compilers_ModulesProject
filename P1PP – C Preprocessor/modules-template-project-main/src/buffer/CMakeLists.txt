add_library(buffer STATIC buffer.c)
target_include_directories(buffer PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(buffer PRIVATE utils)
