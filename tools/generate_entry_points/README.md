# XGL Entry Point Generator

Generates Vulkan API entry points from vk.xml registry automatically.

## Architecture

- `xml_parser.py`: Parses Vulkan XML registry
- `code_generator.py`: Generates C++ entry point code
- `type_mapper.py`: Maps Vulkan types to XGL types
- `template_engine.py`: Handles C++ code templates
- `main.py`: CLI entry point

## Usage

### Manual Generation
```bash
python main.py --xml-path ../../icd/api/include/khronos/sdk-1.4/vulkan/vk.xml --output-dir generated/
```

### CMake Integration
```bash
# From build directory
cmake --build . --target generate_entry_points
```

The build system automatically uses the version-controlled vk.xml file located at:
`xgl/icd/api/include/khronos/sdk-1.4/vulkan/vk.xml`

## Generated Files

- `entry_*.cpp`: Individual entry point files per object type
- `entry_all.h`: Combined header with all entry points