# -----------------------------------------------------------------------------
# src/CMakeLists.txt
# 
# This file includes each module directory (sub-library) so they can be
# built individually with their own CMakeLists.txt.
# Each module (e.g., module_args, module_2) is compiled as a static library to allow
# individual development, compilation, and unit testing in isolation.
#
# By organizing code this way:
# - Each developer works on a separate module with minimal merge conflicts.
# - Modules can be independently tested with their own test_<module>.c files.
# - A main program (optional) can link all modules together for integration.
#
# This CMake file only declares the libraries and their internal includes.
# The main executable and test targets are defined in the top-level and test
# CMakeLists.txt files respectively.
# -----------------------------------------------------------------------------

message(STATUS "(${PROJECT_NAME}) Adding all module subdirectories in src/...")
message(STATUS " - (${PROJECT_NAME}) (src) Configuring source modules and utils...")

# Add utils library from the top-level src folder
add_library(utils STATIC utils_files.c)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "   - (${PROJECT_NAME}) Added utils library")

# Add modules subdirectories
add_subdirectory(module_args)
add_subdirectory(module_2)
message(STATUS "   - (${PROJECT_NAME}) Added modules subdirectories")

message(STATUS "(${PROJECT_NAME}) Finished including module subdirectories.")