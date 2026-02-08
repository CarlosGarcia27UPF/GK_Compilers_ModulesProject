# -----------------------------------------------------
# src/driver/CMakeLists.txt
# Driver Module (Main / Orchestrator)
# Coordinates all modules
# -----------------------------------------------------

add_library(driver STATIC driver.c)

target_include_directories(driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link all modules
target_link_libraries(driver PUBLIC 
    char_stream
    automata
    token
    token_list
    lang_spec
    logger
    error
    counter
    out_writer
)

message(STATUS "(${PROJECT_NAME}) Driver module configured: Added as static library")
