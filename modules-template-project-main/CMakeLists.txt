# ------------------------------------------------------------------------------
# Top-level CMakeLists.txt
# CMake configuration for the "modules_template" C project
#
# Purpose:
#   - Create a template with two separate modules to work and test independently
#     with no conflicts when comitting.
#   - Build and run unit tests using CTest (tests/)
#   - Ensure a consistent C11 build environment across platforms
#   - Serve as a GitHub Actions-compatible template for C projects with tests
#
# Structure:
#   - src/    → Contains the codes separated with modules 2 modules (define as many modules as team members)
#   - tests/  → Contains the tests to validate the program in separate parts/modules
#
# This project is meant to be simple, portable, and CI-friendly.
# ------------------------------------------------------------------------------

# CMake top-level build configuration
cmake_minimum_required(VERSION 3.10)

project(modules_template C)
message(STATUS "(${PROJECT_NAME}) Configuring top-level project...")

# Optionally, define a user-friendly name (used in messages, packaging, etc.).
# set(PROJECT_DISPLAY_NAME "modules-template")

# Set language standard
# Set the required C standard to C11: required to use static_assert and other modern features.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
message(STATUS " - (${PROJECT_NAME}) C standard set to C11")

# Specify Debug build type by default (optional)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
  message(STATUS " - (${PROJECT_NAME}) Default build type set to Debug")
endif()

# Enable testing support (CTest)
enable_testing()
message(STATUS " - (${PROJECT_NAME}) Testing enabled")

# Add source modules and main program
add_subdirectory(src)
message(STATUS " - (${PROJECT_NAME}) Added src/ directory")

# Add tests (for each module or overall integration)
add_subdirectory(tests)
message(STATUS " - (${PROJECT_NAME}) Added tests/ directory")

# Optionally build main executable if needed 
# This executable links together all module libraries.
# (to have modular test for each module in parallel)
add_executable(modules_template_main src/main.c)
target_link_libraries(modules_template_main PRIVATE module_args module_2 utils)
message(STATUS " - (${PROJECT_NAME}) Modules Template Main executable 'modules_template_main' linked with module_args and module_2")
