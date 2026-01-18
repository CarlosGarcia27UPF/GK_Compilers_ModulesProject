add_library(pp_core STATIC pp_core.c)
target_include_directories(pp_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(pp_core PRIVATE utils buffer)
