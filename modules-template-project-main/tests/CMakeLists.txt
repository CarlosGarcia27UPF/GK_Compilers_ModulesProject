# -----------------------------------------------------------------------------
# tests/CMakeLists.txt
#
# This file configures unit tests for each module using CTest. Each test is
# built as a standalone executable and linked with the corresponding module,
# allowing for isolated unit testing and debugging.
# -----------------------------------------------------------------------------
 
message(STATUS "(${PROJECT_NAME}) Configuring test executables for modules...")

# Test for cli
add_executable(test_cli test_cli.c)
target_link_libraries(test_cli PRIVATE cli)
target_include_directories(test_cli PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestCLI COMMAND test_cli)
message(STATUS " - (${PROJECT_NAME}) Test for cli added")

# Test for module_args
add_executable(test_module_args test_module_args.c)
target_link_libraries(test_module_args PRIVATE module_args utils)
target_include_directories(test_module_args PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestModuleArgs COMMAND test_module_args arg90 arg91 arg92)
message(STATUS " - (${PROJECT_NAME}) Test for module_args added")

# Test for module_2
add_executable(test_module_2 test_module_2.c)
target_link_libraries(test_module_2 PRIVATE module_2 utils)
target_include_directories(test_module_2 PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestModule_2 COMMAND test_module_2)
message(STATUS " - (${PROJECT_NAME}) Test for module_2 added")

# Test for errors
add_executable(test_errors test_errors.c)
target_link_libraries(test_errors PRIVATE errors utils)
target_include_directories(test_errors PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestErrors COMMAND test_errors)
message(STATUS " - (${PROJECT_NAME}) Test for errors added")

# Test for io module
add_executable(test_io test_io.c)
target_link_libraries(test_io PRIVATE io buffer)
target_include_directories(test_io PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestIO COMMAND test_io)
message(STATUS " - (${PROJECT_NAME}) Test for io module added")

# Test for macros module
add_executable(test_macros test_macros.c)
target_link_libraries(test_macros PRIVATE macros tokens buffer)
target_include_directories(test_macros PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestMacros COMMAND test_macros)
message(STATUS " - (${PROJECT_NAME}) Test for macros module added")

message(STATUS " - (${PROJECT_NAME}) Test configuration (executables) completed.")
