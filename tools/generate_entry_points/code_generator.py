"""Main code generator for XGL entry points."""

import logging
from pathlib import Path
from typing import Dict, List

from xml_parser import VulkanRegistryParser, Command, CommandType
from template_engine import TemplateEngine
from type_mapper import TypeMapper


class EntryPointGenerator:
    """Generates C++ entry point files from Vulkan XML registry."""
    
    def __init__(self, parser: VulkanRegistryParser, output_dir: Path):
        self.parser = parser
        self.output_dir = output_dir
        self.template_engine = TemplateEngine()
        self.type_mapper = TypeMapper()
        self.logger = logging.getLogger(__name__)
        self.verbose = False
    
    def enable_verbose(self) -> None:
        """Enable verbose logging."""
        self.verbose = True
        logging.basicConfig(level=logging.INFO)
    
    def generate(self) -> None:
        """Generate all entry point files."""
        self._log_info("Starting entry point generation...")
        
        commands = self.parser.get_commands()
        self._log_info(f"Found {len(commands)} commands")
        
        # Filter out NVIDIA/CUDA extensions
        filtered_commands = self._filter_excluded_extensions(commands)
        self._log_info(f"After filtering: {len(filtered_commands)} commands")
        
        # Group commands by type for organized file generation
        command_groups = self._group_commands_by_object_type(filtered_commands)
        
        for object_type, grouped_commands in command_groups.items():
            self._generate_file_for_object_type(object_type, grouped_commands)
        
        self._log_info("Entry point generation complete")
    
    def _filter_excluded_extensions(self, commands: Dict[str, Command]) -> Dict[str, Command]:
        """Filter to only allow core and select vendor extensions."""
        allowed_suffixes = [
            'khr',      # Khronos extensions
            'ext',      # Multi-vendor extensions
            'amd',      # AMD extensions
            'samsung',  # Samsung extensions
            'android',  # Android extensions
        ]
        
        filtered = {}
        for name, command in commands.items():
            func_name_lower = name.lower()
            
            # Always allow functions without vendor suffixes (core functions)
            has_vendor_suffix = any(func_name_lower.endswith(suffix) for suffix in 
                                   ['khr', 'ext', 'amd', 'nv', 'nvx', 'arm', 'qcom', 'samsung', 
                                    'google', 'huawei', 'intel', 'msft', 'qnx', 'sec', 'img', 
                                    'fuchsia', 'ggp', 'nn', 'mvk', 'android', 'win32', 'xcb', 
                                    'xlib', 'wayland', 'directfb', 'ohos'])
            
            if not has_vendor_suffix:
                # Core Vulkan function - always include
                filtered[name] = command
                continue
            
            # Check if function ends with allowed vendor suffix
            if any(func_name_lower.endswith(suffix) for suffix in allowed_suffixes):
                filtered[name] = command
            else:
                self._log_info(f"Filtering out non-allowed extension function: {name}")
                
        return filtered
    
    def _group_commands_by_object_type(self, commands: Dict[str, Command]) -> Dict[str, List[Command]]:
        """Group commands by their primary object type."""
        groups: Dict[str, List[Command]] = {}
        
        for command in commands.values():
            object_type = self._get_object_type_from_command(command)
            if object_type not in groups:
                groups[object_type] = []
            groups[object_type].append(command)
        
        return groups
    
    def _get_object_type_from_command(self, command: Command) -> str:
        """Determine object type from command based on function name patterns."""
        func_name = command.name.lower()
        
        # First priority: Handle destroy functions - categorize by object being destroyed
        if 'destroy' in func_name and len(command.parameters) >= 2:
            # For destroy functions, use the second parameter (the object being destroyed)
            destroy_param = command.parameters[1]  # Skip device parameter
            if hasattr(destroy_param, 'type_name'):
                xgl_type = self.type_mapper.get_xgl_type(destroy_param.type_name)
                return self._pascal_to_snake_case(xgl_type)
        
        # Second priority: Use first handle parameter (systematic approach)
        first_param = command.get_first_handle_param()
        if first_param:
            xgl_type = self.type_mapper.get_xgl_type(first_param.type_name)
            object_type = self._pascal_to_snake_case(xgl_type)
            
            # Special case overrides for known patterns that need different grouping
            special_cases = {
                'cmd_buffer': ['allocatecommandbuffers', 'begincommandbuffer', 'endcommandbuffer', 'resetcommandbuffer'],
                'gpa_session': ['gpasession'],
                'descriptor_set_layout': ['descriptorsetlayout'],
            }
            
            # Check if this function matches any special case patterns
            for special_type, patterns in special_cases.items():
                if any(pattern in func_name for pattern in patterns):
                    return special_type
            
            return object_type
            
        # Third priority: Pattern matching for functions without handle parameters
        pattern_mappings = {
            'instance': ['instance', 'enumerate'],
            'dispatch': ['getprocaddr'],
        }
        
        for object_type, patterns in pattern_mappings.items():
            if any(pattern in func_name for pattern in patterns):
                return object_type
            
        # Final fallback 
        return 'device'
    
    def _pascal_to_snake_case(self, pascal_str: str) -> str:
        """Convert PascalCase to snake_case."""
        import re
        
        # Strip 'Vk' prefix if present (for unmapped Vulkan types)
        if pascal_str.startswith('Vk'):
            pascal_str = pascal_str[2:]
        
        # Handle common extension suffixes as single words
        extension_suffixes = ['KHR', 'EXT', 'AMD', 'SAMSUNG']
        for suffix in extension_suffixes:
            if pascal_str.endswith(suffix):
                base = pascal_str[:-len(suffix)]
                # Convert base to snake_case and append suffix
                base_snake = re.sub(r'(?<!^)([A-Z])', r'_\1', base).lower()
                return f"{base_snake}_{suffix.lower()}"
        
        # Insert underscores before capital letters (except the first one)
        snake_str = re.sub(r'(?<!^)([A-Z])', r'_\1', pascal_str)
        
        # Convert to lowercase
        return snake_str.lower()
    
    def _generate_file_for_object_type(self, object_type: str, commands: List[Command]) -> None:
        """Generate entry point file for specific object type."""
        filename = f"entry_vk_{object_type}.cpp"
        filepath = self.output_dir / filename
        
        self._log_info(f"Generating {filename} with {len(commands)} functions")
        
        includes = self._generate_includes(commands)
        functions = self._generate_functions(commands)
        
        content = self.template_engine.render_file_header(
            filename=filename,
            object_type=object_type,
            includes=includes
        )
        
        # Replace placeholder with actual functions
        content = content.replace('$functions', functions)
        
        self._write_file(filepath, content)
    
    def _generate_includes(self, commands: List[Command]) -> str:
        """Generate include statements for commands."""
        return self.type_mapper.get_required_includes(commands)
    
    def _generate_functions(self, commands: List[Command]) -> str:
        """Generate all functions for given commands."""
        functions = []
        
        for command in commands:
            function_code = self._generate_single_function(command)
            functions.append(function_code)
        
        return '\n\n'.join(functions)
    
    def _generate_single_function(self, command: Command) -> str:
        """Generate code for a single entry point function."""
        context = self._build_function_context(command)
        return self.template_engine.render_entry_function(context)
    
    def _build_function_context(self, command: Command) -> Dict[str, str]:
        """Build template context for function generation."""
        first_handle = command.get_first_handle_param()
        method_name = self.type_mapper.get_method_name(command.name)
        
        # Build basic context
        context = {
            'function_name': command.name,
            'return_type': command.return_type,
            'parameters': self.type_mapper.format_parameter_list(command.parameters),
            'method_name': method_name,
            'method_params': self.type_mapper.format_method_parameters(command.parameters)
        }
        
        # Add function body
        if first_handle:
            context.update(self._build_handle_context(command, first_handle))
        
        function_body = self.template_engine.render_function_body(context)
        context['function_body'] = function_body
        
        return context
    
    def _build_handle_context(self, command: Command, handle_param) -> Dict[str, str]:
        """Build context for handle-based functions."""
        context = {
            'handle_param': handle_param.name,
            'object_type': self.type_mapper.get_xgl_type(handle_param.type_name)
        }
        
        # Determine function type for body template selection
        if handle_param.type_name == 'VkDevice':
            context['function_type'] = 'device'
        elif handle_param.type_name == 'VkInstance':
            context['function_type'] = 'instance'
        else:
            context['function_type'] = 'simple'
        
        # Add allocator logic if needed
        if self.type_mapper.needs_allocator_logic(command.name):
            allocator_param = self.type_mapper.get_allocator_parameter(command.parameters)
            if allocator_param:
                context['allocator_logic'] = self.template_engine.render_allocator_logic(allocator_param)
                # Update method params to use pAllocCB  
                original_params = self.type_mapper.format_method_parameters(command.parameters)
                context['method_params'] = original_params.replace(allocator_param, 'pAllocCB')
            else:
                context['allocator_logic'] = ''
        else:
            context['allocator_logic'] = ''
        
        return context
    
    def _write_file(self, filepath: Path, content: str) -> None:
        """Write generated content to file."""
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        
        self._log_info(f"Generated: {filepath}")
    
    def _log_info(self, message: str) -> None:
        """Log info message if verbose enabled."""
        if self.verbose:
            self.logger.info(message)