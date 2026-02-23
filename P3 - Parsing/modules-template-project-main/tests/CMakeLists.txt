# -----------------------------------------------------------------------------
# tests/CMakeLists.txt
#
# Configures unit / integration test executables for each module.
# Each test is a standalone executable linked against its module library.
# -----------------------------------------------------------------------------

message(STATUS "(${PROJECT_NAME}) Configuring test executables for modules...")

# Test for module_args
add_executable(test_module_args test_module_args.c)
target_link_libraries(test_module_args PRIVATE module_args utils)
target_include_directories(test_module_args PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestModuleArgs COMMAND test_module_args arg90 arg91 arg92)
message(STATUS " - (${PROJECT_NAME}) Test for module_args added")

# Test for lang_spec
add_executable(test_lang_spec test_lang_spec.c)
target_link_libraries(test_lang_spec PRIVATE lang_spec dfa utils)
target_include_directories(test_lang_spec PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestLangSpec
         COMMAND test_lang_spec ${PROJECT_SOURCE_DIR}/data/language1.txt)
message(STATUS " - (${PROJECT_NAME}) Test for lang_spec added")

# Test for dfa
add_executable(test_dfa test_dfa.c)
target_link_libraries(test_dfa PRIVATE dfa utils)
target_include_directories(test_dfa PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestDFA COMMAND test_dfa)
message(STATUS " - (${PROJECT_NAME}) Test for dfa added")

# Test for parser_stack
add_executable(test_parser_stack test_parser_stack.c)
target_link_libraries(test_parser_stack PRIVATE parser_stack lang_spec utils)
target_include_directories(test_parser_stack PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestParserStack COMMAND test_parser_stack)
message(STATUS " - (${PROJECT_NAME}) Test for parser_stack added")

# Integration test for sra engine (uses sample1.cscn + language1.txt)
add_executable(test_sra test_sra.c)
target_link_libraries(test_sra PRIVATE sra dfa lang_spec token_loader parser_stack
                               out_writer utils)
target_include_directories(test_sra PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME TestSRALang1
         COMMAND test_sra
             ${PROJECT_SOURCE_DIR}/samples/sample1.cscn
             ${PROJECT_SOURCE_DIR}/data/language1.txt
             accept)
add_test(NAME TestSRALang1Sample2
         COMMAND test_sra
             ${PROJECT_SOURCE_DIR}/samples/sample2.cscn
             ${PROJECT_SOURCE_DIR}/data/language1.txt
             accept)
add_test(NAME TestSRALang2Simple
         COMMAND test_sra
             ${PROJECT_SOURCE_DIR}/samples/sample3.cscn
             ${PROJECT_SOURCE_DIR}/data/language2.txt
             accept)
add_test(NAME TestSRALang2Compound
         COMMAND test_sra
             ${PROJECT_SOURCE_DIR}/samples/sample4.cscn
             ${PROJECT_SOURCE_DIR}/data/language2.txt
             accept)
message(STATUS " - (${PROJECT_NAME}) Tests for sra added")

message(STATUS " - (${PROJECT_NAME}) Test configuration completed.")

