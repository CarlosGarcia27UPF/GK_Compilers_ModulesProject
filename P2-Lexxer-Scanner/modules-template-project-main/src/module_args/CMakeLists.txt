# -----------------------------------------------------
# src/module_args/CMakeLists.txt
# CMakeLists.txt for module_args
# 
# This file defines the module_args static library. It is compiled independently
# and can be tested separately.
# It is included from the top-level CMakeLists.txt via `add_subdirectory(src/module_args)`.
# The module is compiled as a static library and can be linked by main programs or tests.
# -----------------------------------------------------

# Create the static library from the module_args source file
add_library(module_args module_args.c)

# Include the current source directory for header file access
# Since it is PRIVATE, the includes must include the path to the headers
target_include_directories(module_args PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link utils because module_args depends on utils
target_link_libraries(module_args PRIVATE utils)

# Print a status message during CMake configuration to confirm module_args is processed
message(STATUS "(${PROJECT_NAME}) module_args configured: Added as static library")

# Optionally, you can set properties or compile options specific to this module