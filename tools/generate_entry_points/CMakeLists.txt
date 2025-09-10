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

# Test target
add_custom_target(test_generator
    COMMAND Python3::Interpreter "${ENTRY_GENERATOR_DIR}/run_tests.py"
    WORKING_DIRECTORY "${ENTRY_GENERATOR_DIR}"
    COMMENT "Running entry point generator tests"
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