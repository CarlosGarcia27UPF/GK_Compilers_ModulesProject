# -----------------------------------------------------------------------------
# src/CMakeLists.txt
#
# Adds all module sub-libraries for the p3_parser project.
# Each module is compiled as a static library for independent development and
# unit testing.
# -----------------------------------------------------------------------------

message(STATUS "(${PROJECT_NAME}) Adding all module subdirectories in src/...")

# utils library (platform helpers, timestamp logging)
add_library(utils STATIC utils_files.c)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# _putenv/_tzset are MSVC/MinGW names; map them to POSIX equivalents on Linux
if(NOT WIN32)
    target_compile_definitions(utils PRIVATE _putenv=putenv _tzset=tzset)
endif()
message(STATUS "   - (${PROJECT_NAME}) Added utils library")

# Module subdirectories (each defines its own static library)
add_subdirectory(dfa)
add_subdirectory(module_args)
add_subdirectory(lang_spec)
add_subdirectory(token_loader)
add_subdirectory(parser_stack)
add_subdirectory(out_writer)
add_subdirectory(sra)
message(STATUS "   - (${PROJECT_NAME}) Added all module subdirectories")

message(STATUS "(${PROJECT_NAME}) Finished including module subdirectories.")
