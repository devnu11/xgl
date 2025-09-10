# XGL Entry Point Generator

Generates Vulkan API entry points from vk.xml registry automatically.

## Architecture

- `xml_parser.py`: Parses Vulkan XML registry
- `code_generator.py`: Generates C++ entry point code
- `type_mapper.py`: Maps Vulkan types to XGL types
- `template_engine.py`: Handles C++ code templates
- `main.py`: CLI entry point

## Usage

```bash
python main.py --xml-path path/to/vk.xml --output-dir xgl/icd/api/entry/generated
```

## Generated Files

- `entry_*.cpp`: Individual entry point files per object type
- `entry_all.h`: Combined header with all entry points