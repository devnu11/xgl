#!/usr/bin/env python3
"""
XGL Entry Point Generator

Generates Vulkan API entry points from vk.xml registry.
"""

import argparse
from pathlib import Path

from xml_parser import VulkanRegistryParser
from code_generator import EntryPointGenerator


def main():
    parser = argparse.ArgumentParser(
        description="Generate XGL entry points from Vulkan XML registry"
    )
    parser.add_argument(
        "--xml-path", required=True, help="Path to vk.xml file"
    )
    parser.add_argument(
        "--output-dir", required=True, help="Output directory for generated files"
    )
    parser.add_argument(
        "--verbose", action="store_true", help="Enable verbose output"
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