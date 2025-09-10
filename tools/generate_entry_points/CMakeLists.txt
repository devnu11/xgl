# CMake integration for XGL entry point generator

# Find Python3
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Set generator script paths
set(ENTRY_GENERATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ENTRY_GENERATOR_SCRIPT "${ENTRY_GENERATOR_DIR}/main.py")
set(ENTRY_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/entry_points")

# Download vk.xml if not present
set(VK_XML_URL "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Headers/main/registry/vk.xml")
set(VK_XML_PATH "${CMAKE_BINARY_DIR}/vk.xml")

# Download vk.xml target
add_custom_command(
    OUTPUT ${VK_XML_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}"
    COMMAND Python3::Interpreter -c "
import urllib.request
urllib.request.urlretrieve('${VK_XML_URL}', '${VK_XML_PATH}')
print('Downloaded vk.xml')
"
    COMMENT "Downloading Vulkan XML registry"
    VERBATIM
)

# Generate entry points target
add_custom_command(
    OUTPUT ${ENTRY_OUTPUT_DIR}/generated.stamp
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ENTRY_OUTPUT_DIR}"
    COMMAND Python3::Interpreter "${ENTRY_GENERATOR_SCRIPT}"
        --xml-path "${VK_XML_PATH}"
        --output-dir "${ENTRY_OUTPUT_DIR}"
        --verbose
    COMMAND ${CMAKE_COMMAND} -E touch "${ENTRY_OUTPUT_DIR}/generated.stamp"
    DEPENDS ${VK_XML_PATH} 
            ${ENTRY_GENERATOR_SCRIPT}
            "${ENTRY_GENERATOR_DIR}/xml_parser.py"
            "${ENTRY_GENERATOR_DIR}/code_generator.py"
            "${ENTRY_GENERATOR_DIR}/type_mapper.py"
            "${ENTRY_GENERATOR_DIR}/template_engine.py"
    COMMENT "Generating XGL entry points from vk.xml"
    VERBATIM
)

# Custom target for entry point generation
add_custom_target(generate_entry_points
    DEPENDS ${ENTRY_OUTPUT_DIR}/generated.stamp
    COMMENT "Generating XGL entry points"
)

# Enable CTest with custom configuration
include(CTest)
include(CTestConfig.cmake OPTIONAL)

# Test configuration
set(ENTRY_TESTS_DIR "${ENTRY_GENERATOR_DIR}/tests")
set(PYTHON_TEST_MODULES 
    xml_parser
    type_mapper  
    template_engine
    code_generator
    main
)

# Create individual test targets for each module
foreach(MODULE ${PYTHON_TEST_MODULES})
    add_test(
        NAME "entry_generator_test_${MODULE}"
        COMMAND Python3::Interpreter -m unittest "test_${MODULE}" -v
        WORKING_DIRECTORY "${ENTRY_GENERATOR_DIR}"
    )
    
    # Set test properties
    set_tests_properties("entry_generator_test_${MODULE}" PROPERTIES
        TIMEOUT 30
        LABELS "entry_generator;unit_tests;${MODULE}"
        ENVIRONMENT "PYTHONPATH=${ENTRY_GENERATOR_DIR}"
    )
endforeach()

# Add comprehensive test suite
add_test(
    NAME "entry_generator_test_suite"
    COMMAND Python3::Interpreter "${ENTRY_GENERATOR_DIR}/run_tests.py"
    WORKING_DIRECTORY "${ENTRY_GENERATOR_DIR}"
)

set_tests_properties("entry_generator_test_suite" PROPERTIES
    TIMEOUT 60
    LABELS "entry_generator;integration_tests;full_suite"
    ENVIRONMENT "PYTHONPATH=${ENTRY_GENERATOR_DIR}"
)

# Add enhanced test suite with coverage and reporting
add_test(
    NAME "entry_generator_test_with_coverage"
    COMMAND Python3::Interpreter "${ENTRY_GENERATOR_DIR}/run_tests_with_coverage.py"
    WORKING_DIRECTORY "${ENTRY_GENERATOR_DIR}"
)

set_tests_properties("entry_generator_test_with_coverage" PROPERTIES
    TIMEOUT 120
    LABELS "entry_generator;coverage;reporting"
    ENVIRONMENT "PYTHONPATH=${ENTRY_GENERATOR_DIR}"
)

# Add code quality tests
add_test(
    NAME "entry_generator_code_quality"
    COMMAND Python3::Interpreter "${ENTRY_GENERATOR_DIR}/format_code.py"
    WORKING_DIRECTORY "${ENTRY_GENERATOR_DIR}"
)

set_tests_properties("entry_generator_code_quality" PROPERTIES
    TIMEOUT 30
    LABELS "entry_generator;code_quality"
    ENVIRONMENT "PYTHONPATH=${ENTRY_GENERATOR_DIR}"
)

# Add validation test (requires generated code)
add_test(
    NAME "entry_generator_validation"
    COMMAND Python3::Interpreter "${ENTRY_GENERATOR_DIR}/validate_output.py"
    WORKING_DIRECTORY "${ENTRY_GENERATOR_DIR}"
)

set_tests_properties("entry_generator_validation" PROPERTIES
    TIMEOUT 30
    LABELS "entry_generator;validation"
    DEPENDS generate_entry_points
    ENVIRONMENT "PYTHONPATH=${ENTRY_GENERATOR_DIR}"
)

# Test fixtures and dependencies
add_test(
    NAME "entry_generator_setup_fixture"
    COMMAND ${CMAKE_COMMAND} -E echo "Setting up entry point generator tests"
)

set_tests_properties("entry_generator_setup_fixture" PROPERTIES
    FIXTURES_SETUP "entry_generator_fixture"
    LABELS "entry_generator;fixtures"
)

add_test(
    NAME "entry_generator_cleanup_fixture" 
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning up entry point generator tests"
)

set_tests_properties("entry_generator_cleanup_fixture" PROPERTIES
    FIXTURES_CLEANUP "entry_generator_fixture"
    LABELS "entry_generator;fixtures"
)

# Make all unit tests depend on setup fixture
foreach(MODULE ${PYTHON_TEST_MODULES})
    set_tests_properties("entry_generator_test_${MODULE}" PROPERTIES
        FIXTURES_REQUIRED "entry_generator_fixture"
        RESOURCE_LOCK "python_path"
    )
endforeach()

# Set fixture requirements for integration tests
set_tests_properties("entry_generator_test_suite" PROPERTIES
    FIXTURES_REQUIRED "entry_generator_fixture"
    RESOURCE_LOCK "python_path"
)

set_tests_properties("entry_generator_code_quality" PROPERTIES
    FIXTURES_REQUIRED "entry_generator_fixture"
)

# Add test categories for selective running
add_custom_target(test_generator_unit
    COMMAND ${CMAKE_CTEST_COMMAND} --label-regex "unit_tests" --output-on-failure --parallel ${CTEST_PARALLEL_LEVEL}
    COMMENT "Running unit tests only"
    VERBATIM
)

add_custom_target(test_generator_integration  
    COMMAND ${CMAKE_CTEST_COMMAND} --label-regex "integration_tests" --output-on-failure
    COMMENT "Running integration tests only"
    VERBATIM
)

add_custom_target(test_generator_quality
    COMMAND ${CMAKE_CTEST_COMMAND} --label-regex "code_quality" --output-on-failure
    COMMENT "Running code quality checks only"
    VERBATIM
)

add_custom_target(test_generator_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} --label-regex "coverage" --output-on-failure
    COMMENT "Running tests with coverage analysis"
    VERBATIM
)

# Main test target with parallel execution
add_custom_target(test_generator
    COMMAND ${CMAKE_CTEST_COMMAND} --label-regex "entry_generator" --output-on-failure --parallel ${CTEST_PARALLEL_LEVEL}
    COMMENT "Running all entry point generator tests via CTest"
    VERBATIM
)

# Verbose test target for debugging
add_custom_target(test_generator_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --label-regex "entry_generator" --verbose --output-on-failure
    COMMENT "Running entry point generator tests with verbose output"
    VERBATIM
)

# Make generated files available to parent
set(XGL_GENERATED_ENTRY_POINTS_DIR "${ENTRY_OUTPUT_DIR}" PARENT_SCOPE)
set(XGL_ENTRY_POINT_SOURCES 
    "${ENTRY_OUTPUT_DIR}/entry_vk_device.cpp"
    "${ENTRY_OUTPUT_DIR}/entry_vk_instance.cpp"
    "${ENTRY_OUTPUT_DIR}/entry_vk_buffer.cpp"
    "${ENTRY_OUTPUT_DIR}/entry_vk_image.cpp"
    "${ENTRY_OUTPUT_DIR}/entry_vk_pipeline.cpp"
    PARENT_SCOPE
)