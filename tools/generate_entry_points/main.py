#!/usr/bin/env python3
"""
XGL Entry Point Generator

Generates Vulkan API entry points from vk.xml registry.
"""

import argparse
from pathlib import Path

from xml_parser import VulkanRegistryParser
from code_generator import EntryPointGenerator

xgl_path = Path(__file__).parent.parent.parent
api_path = xgl_path / "icd" / "api"

def main():
    parser = argparse.ArgumentParser(
        description="Generate XGL entry points from Vulkan XML registry"
    )
    parser.add_argument(
        "--xml-path",
        required=False,
        help="Path to vk.xml file",
        default=api_path / "include/khronos/sdk-1.4/vulkan/vk.xml"
    )
    parser.add_argument(
        "--output-dir",
        required=False,
        help="Output directory for generated files",
        default=api_path / "entry"
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    xml_path = Path(args.xml_path)
    output_dir = Path(args.output_dir)
    
    if not xml_path.exists():
        raise FileNotFoundError(f"XML file not found: {xml_path}")
    
    output_dir.mkdir(parents=True, exist_ok=True)
    
    registry_parser = VulkanRegistryParser(xml_path)
    generator = EntryPointGenerator(registry_parser, output_dir)
    
    if args.verbose:
        generator.enable_verbose()
    
    generator.generate()


if __name__ == "__main__":
    main()