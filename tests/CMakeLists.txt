add_library(beast2_test_support STATIC test_support.c)
target_link_libraries(beast2_test_support PUBLIC beast2_core)
target_include_directories(beast2_test_support PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(
    beast2_test_support
    PUBLIC
        BEAST2_TEST_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
        BEAST2_TEST_BINARY_DIR="${PROJECT_BINARY_DIR}"
)
beast2_apply_warnings(beast2_test_support)

function(beast2_add_unit_test target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE beast2_core beast2_test_support)
    target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    beast2_apply_warnings(${target_name})

    add_test(NAME ${target_name} COMMAND ${target_name})
    set_tests_properties(${target_name} PROPERTIES WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
endfunction()

beast2_add_unit_test(test_parser test_parser.c)
beast2_add_unit_test(test_config test_config.c)
beast2_add_unit_test(test_filesystem test_filesystem.c)
beast2_add_unit_test(test_scheduler test_scheduler.c)
beast2_add_unit_test(test_tensor_memory test_tensor_memory.c)
beast2_add_unit_test(test_execution test_execution.c)
beast2_add_unit_test(test_runtime test_runtime.c)

add_test(NAME beast2_cli_help COMMAND beast2 --help)
set_tests_properties(beast2_cli_help PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_test(
    NAME beast2_cli_parse_example
    COMMAND beast2 --generator "${PROJECT_SOURCE_DIR}/examples/wan22_walk_cycle.b2"
)
set_tests_properties(beast2_cli_parse_example PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_test(
    NAME beast2_cli_parse_all_prompts
    COMMAND beast2 --generator "${PROJECT_SOURCE_DIR}/examples/wan22_walk_cycle.b2" --all-prompts
)
set_tests_properties(beast2_cli_parse_all_prompts PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_test(
    NAME beast2_cli_runtime_boot
    COMMAND beast2 --config "${PROJECT_SOURCE_DIR}/tests/fixtures/config/runtime-test.conf"
)
set_tests_properties(beast2_cli_runtime_boot PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_test(
    NAME beast2_cli_invalid_generator
    COMMAND beast2 --generator "${PROJECT_SOURCE_DIR}/tests/fixtures/invalid/bad_line_prefix.b2"
)
set_tests_properties(
    beast2_cli_invalid_generator
    PROPERTIES
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        WILL_FAIL TRUE
)

add_test(
    NAME beast2_cli_execute_generator
    COMMAND
        beast2
        --config "${PROJECT_SOURCE_DIR}/tests/fixtures/config/execution-test.conf"
        --run-generator "${PROJECT_SOURCE_DIR}/tests/fixtures/valid/executable_generator.b2"
)
set_tests_properties(beast2_cli_execute_generator PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
