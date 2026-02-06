# -----------------------------------------------------
# src/module1/CMakeLists.txt
# CMakeLists.txt for Module_2 
#
# This file defines the module_2 static library. It is compiled independently
# and can be tested separately.
# It is included from the top-level CMakeLists.txt via `add_subdirectory(src/module_2)`.
# The module is compiled as a static library and can be linked by main programs or tests.
# -----------------------------------------------------

# Create the static library from the module_2 source file
add_library(module_2 module_2.c)

# Include the current source directory for header file access
# Since it is PRIVATE, the includes must include the path to the headers
target_include_directories(module_2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link utils because module_2 depends on utils
target_link_libraries(module_2 PRIVATE utils)

# Print a status message during CMake configuration to confirm module_2 is processed
message(STATUS "(${PROJECT_NAME}) Module_2 configured: Added as static library")

# Optionally, you can set properties or compile options specific to this module