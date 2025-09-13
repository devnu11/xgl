"""Main code generator for XGL entry points."""

import logging
from pathlib import Path
from typing import Dict, List, Optional

from xml_parser import VulkanRegistryParser, Command, CommandType
from template_engine import TemplateEngine
from type_mapper import TypeMapper
from config_loader import ConfigLoader


class EntryPointGenerator:
    """Generates C++ entry point files from Vulkan XML registry."""
    
    def __init__(self, parser: VulkanRegistryParser, output_dir: Path):
        self.parser = parser
        self.output_dir = output_dir
        self.template_engine = TemplateEngine()
        self.type_mapper = TypeMapper()
        self.config = ConfigLoader()
        self.logger = logging.getLogger(__name__)
        self.verbose = False
        
        # Always configure logging to show warnings and errors
        if not self.logger.handlers:
            logging.basicConfig(level=logging.WARNING, format='%(levelname)s: %(message)s')
    
    def enable_verbose(self) -> None:
        """Enable verbose logging."""
        self.verbose = True
        # Reconfigure logging to INFO level for verbose mode
        logging.getLogger().setLevel(logging.INFO)
    
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
        """Group commands by explicit file mappings, preserving the order specified in mappings."""
        file_mappings = self._get_file_mappings()
        enhanced_mappings = self._get_enhanced_file_mappings()
        
        groups: Dict[str, List[Command]] = {}
        unmapped_functions = []
        missing_functions = []  # Functions in mappings but not in XML
        
        # Iterate through file mappings in order to preserve function ordering
        for file_name, mapping_data in file_mappings.items():
            groups[file_name] = []
            
            # Build a map of enhanced functions for quick lookup
            enhanced_functions = {}
            if file_name in enhanced_mappings:
                for func_entry in enhanced_mappings[file_name]['functions']:
                    enhanced_functions[func_entry['name']] = func_entry
            
            # Process all functions from the old format, applying enhanced metadata where available
            func_list = mapping_data if isinstance(mapping_data, list) else mapping_data.get('functions', [])
            for func_entry in func_list:
                func_name = func_entry if isinstance(func_entry, str) else func_entry.get('name')
                if func_name in commands:
                    command = commands[func_name]
                    
                    # Check if we have enhanced metadata for this function
                    if func_name in enhanced_functions:
                        enhanced_entry = enhanced_functions[func_name]
                        command.impl_type = enhanced_entry.get('impl', 'standard')
                        command.impl_method = enhanced_entry.get('method')
                        command.impl_return = enhanced_entry.get('return')
                    else:
                        # Set default implementation type for functions without enhanced metadata
                        command.impl_type = 'standard'
                        command.impl_method = None
                        command.impl_return = None
                    
                    groups[file_name].append(command)
                else:
                    # Function is in mapping but not found in XML
                    missing_functions.append(f"{func_name} (mapped to {file_name})")
        
        # Check for functions in commands that aren't mapped to any file
        mapped_functions = set()
        for file_name, mapping_data in file_mappings.items():
            if file_name in enhanced_mappings:
                func_list = enhanced_mappings[file_name]['functions']
                for func_entry in func_list:
                    mapped_functions.add(func_entry['name'])
            else:
                func_list = mapping_data if isinstance(mapping_data, list) else mapping_data.get('functions', [])
                for func_entry in func_list:
                    func_name = func_entry if isinstance(func_entry, str) else func_entry.get('name')
                    mapped_functions.add(func_name)
        
        for command in commands.values():
            if command.name not in mapped_functions:
                unmapped_functions.append(command.name)
        
        # Report and ignore unmapped functions (they don't exist in current files)
        if unmapped_functions:
            self._log_info(f"INFO: Ignoring {len(unmapped_functions)} unmapped functions (not in existing entry files):")
            for func in sorted(unmapped_functions):
                self._log_info(f"  - {func}")
            self._log_info("These functions will not be generated unless added to the file mappings.")
        
        # Report missing functions (in mappings but not in XML)
        if missing_functions:
            self.logger.warning(f"{len(missing_functions)} functions are mapped but don't exist in XML registry:")
            for func in sorted(missing_functions):
                self.logger.warning(f"  - {func}")
            self.logger.warning("These mappings may be vestigial and should be reviewed/removed.")
        
        return groups
    
    def _get_enhanced_file_mappings(self) -> Dict[str, Dict[str, List]]:
        """Get enhanced mapping with implementation metadata for select files."""
        result = {}
        for file_name in self.config.enhanced_functions:
            result[file_name] = {
                'functions': [
                    {
                        'name': override.name,
                        'impl': override.impl if override.impl else 'standard',
                        'method': override.method,
                        'return': override.return_value
                    }
                    for override in self.config.get_enhanced_function_overrides(file_name)
                ]
            }
        return result
    
    def _get_file_mappings(self) -> Dict[str, List[str]]:
        """Get explicit mapping of file names to function details with implementation metadata."""  
        return self.config.file_mappings
    
    def _find_handle_parameter_for_object_type(self, command: Command, file_object_type: str) -> Optional:
        """Find the handle parameter that matches the expected object type for this file.
        
        Args:
            command: The Vulkan command to analyze
            file_object_type: The object type expected for this file (e.g., 'memory', 'buffer')
            
        Returns:
            The parameter that matches the expected handle type, or None if not found
        """
        from xml_parser import Parameter
        
        # Get expected VkHandle type for this file's object type
        expected_type = self.config.get_expected_handle_type(file_object_type)
        if not expected_type:
            return None
        
        # First, look for direct handle parameters of the expected type
        for param in command.parameters:
            if param.type_name == expected_type and not param.is_pointer:
                return param
        
        # Check for embedded handles in struct parameters using config-based patterns
        struct_patterns = {
            'memory': ['VkMemoryMapInfo', 'VkMemoryUnmapInfo', 'VkMemoryGetFdInfoKHR', 'VkDeviceMemoryOpaqueCaptureAddressInfo'],
            'image': ['VkImageMemoryRequirementsInfo2', 'VkImageSparseMemoryRequirementsInfo2'],
            'buffer': ['VkBufferMemoryRequirementsInfo2', 'VkBufferDeviceAddressInfo'],
        }
        
        struct_types = struct_patterns.get(file_object_type, [])
        for param in command.parameters:
            if param.type_name in struct_types and param.is_pointer:
                # Create a synthetic parameter representing the embedded handle
                # This is a bit of a hack, but it allows us to identify these cases
                synthetic_param = Parameter(
                    name=f"_embedded_{file_object_type}",  # e.g., "_embedded_memory"
                    type_name=expected_type,
                    is_pointer=False
                )
                return synthetic_param
                
        return None
    
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
        functions = self._generate_functions(commands, object_type)
        
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
    
    def _generate_functions(self, commands: List[Command], file_object_type: str) -> str:
        """Generate all functions for given commands."""
        functions = []
        
        for command in commands:
            function_code = self._generate_single_function(command, file_object_type)
            functions.append(function_code)
        
        return '\n\n'.join(functions)
    
    def _generate_single_function(self, command: Command, file_object_type: str) -> str:
        """Generate code for a single entry point function."""
        context = self._build_function_context(command, file_object_type)
        return self.template_engine.render_entry_function(context)
    
    def _build_function_context(self, command: Command, file_object_type: str) -> Dict[str, str]:
        """Build template context for function generation."""
        first_handle = command.get_first_handle_param()
        
        # Get implementation metadata from enhanced mappings
        impl_type = getattr(command, 'impl_type', 'standard')
        impl_method = getattr(command, 'impl_method', None)
        impl_return = getattr(command, 'impl_return', None)
        
        # Convert file_object_type from snake_case to PascalCase for XGL class name
        # e.g., "cmd_pool" -> "CmdPool", "memory" -> "Memory", "device" -> "Device"
        file_class_name = ''.join(word.capitalize() for word in file_object_type.split('_'))
        
        # Use impl_method if specified, otherwise fall back to type mapper with file's class context
        if impl_method:
            method_name = impl_method
        else:
            method_name = self.type_mapper.get_method_name(command.name, file_class_name)
        
        # Build basic context
        # Determine which parameter to exclude from method parameters
        target_handle = self._find_handle_parameter_for_object_type(command, file_object_type)
        if target_handle and not target_handle.name.startswith('_embedded_'):
            # Skip the target handle parameter specifically (but not synthetic embedded ones)
            skip_param_name = target_handle.name
        elif target_handle and target_handle.name.startswith('_embedded_'):
            # For embedded handles, don't skip any parameter - the handle is inside a struct
            skip_param_name = None
        else:
            # Fallback: skip first parameter if it's a handle
            skip_param_name = first_handle.name if first_handle else None
        
        # Check if function has a device parameter that will need initialization
        has_device_param = any(param.type_name == 'VkDevice' and param.name == 'device' for param in command.parameters)
        
        context = {
            'function_name': command.name,
            'return_type': command.return_type,
            'parameters': self.type_mapper.format_parameter_list(command.parameters),
            'method_name': method_name,
            'method_params': self._format_method_parameters_excluding_param(command.parameters, skip_param_name),
            # For entry_points pattern, include all parameters (don't skip first)
            'entry_points_params': self.type_mapper.format_method_parameters(command.parameters, skip_first=False),
            # Add implementation metadata from enhanced mappings
            'impl_type': impl_type,
            'impl_method': impl_method,
            'impl_return': impl_return,
            # Include raw command parameters for special processing
            '_command_params': command.parameters,
            # Store the target handle for templates
            '_target_handle_param': target_handle.name if target_handle else (first_handle.name if first_handle else None),
            # Device initialization logic
            'needs_device_init': has_device_param and target_handle is not None,
            'device_init_line': 'const Device* pDevice = ApiDevice::ObjectFromHandle(device);\n    ' if has_device_param and target_handle is not None else ''
        }
        
        # Add destroy function specific context
        is_destroy_function = command.name.startswith('vkDestroy') and len(command.parameters) >= 2
        if is_destroy_function:
            context.update(self._build_destroy_context(command, file_class_name))
        
        # Add function body (skip handle context for destroy functions as destroy context is more specific)
        if first_handle and not is_destroy_function:
            # Find the handle parameter that matches the file's object type, fallback to first handle
            target_handle = self._find_handle_parameter_for_object_type(command, file_object_type)
            if target_handle:
                context.update(self._build_handle_context(command, target_handle, file_class_name, file_object_type))
            else:
                # Fallback to first handle - if it's a device handle, use Device as object type
                if first_handle.type_name == 'VkDevice':
                    context.update(self._build_handle_context(command, first_handle, 'Device', 'device'))
                else:
                    context.update(self._build_handle_context(command, first_handle, file_class_name, file_object_type))
        
        # For static member calls, we need the object_type even for global functions
        if impl_type == 'static_member_call':
            # Use the file's class name for static calls
            context['object_type'] = file_class_name
        
        # Add allocator logic for functions with allocator parameters (but not destroy functions or global functions)
        if not is_destroy_function and first_handle:
            allocator_param = self._find_allocator_parameter(command)
            if allocator_param:
                context['allocator_logic'] = self.template_engine.render_allocator_logic(allocator_param.name)
                # Replace allocator parameter with pAllocCB in method parameters, also excluding the target handle
                context['method_params'] = self._format_method_parameters_with_allocator_excluding_handle(
                    command.parameters, allocator_param.name, skip_param_name)
            else:
                context['allocator_logic'] = ''
        elif not is_destroy_function:
            # Global functions - no allocator logic needed
            context['allocator_logic'] = ''
        
        function_body = self.template_engine.render_function_body(context)
        context['function_body'] = function_body
        
        return context
    
    def _build_destroy_context(self, command: Command, file_class_name: str) -> Dict[str, str]:
        """Build context for destroy functions."""
        if len(command.parameters) == 2:
            # 2-parameter destroy functions: (handle, allocator) - handle destroys itself
            # Examples: vkDestroyInstance, vkDestroyDevice
            handle_param = command.parameters[0]  # The object being destroyed
            allocator_param = command.parameters[1]  # Allocator
            
            context = {
                'device_param': handle_param.name,  # Use the handle as device param for these functions
                'handle_param': handle_param.name,
                'allocator_param': allocator_param.name,
                'object_type': file_class_name
            }
        else:
            # 3-parameter destroy functions: (device, handle, allocator)
            device_param = command.parameters[0]  # First param is usually device
            handle_param = command.parameters[1]  # Second param is the object being destroyed
            allocator_param = command.parameters[2]  # Third param is allocator
            
            context = {
                'device_param': device_param.name,
                'handle_param': handle_param.name,
                'allocator_param': allocator_param.name,
                'object_type': file_class_name
            }
        
        return context
    
    def _format_method_parameters_excluding_param(self, parameters, exclude_param_name: Optional[str]) -> str:
        """Format method parameters, excluding a specific parameter by name and converting device to pDevice."""
        if not exclude_param_name:
            # No parameter to exclude, format all parameters with device conversion
            return self._format_method_parameters_with_device_conversion(parameters)
        
        # Filter out the excluded parameter
        filtered_params = [param for param in parameters if param.name != exclude_param_name]
        return self._format_method_parameters_with_device_conversion(filtered_params)
    
    def _format_method_parameters_with_device_conversion(self, parameters) -> str:
        """Format method parameters, converting VkDevice parameters to pDevice."""
        param_names = []
        for param in parameters:
            if param.type_name == 'VkDevice' and param.name == 'device':
                param_names.append('pDevice')
            else:
                param_names.append(param.name)
        
        # Apply formatting similar to the original format_method_parameters
        if len(param_names) >= 3 or len(", ".join(param_names)) > 80:  # Leave room for method call syntax
            return "\n" + " " * 8 + (",\n" + " " * 8).join(param_names)
        else:
            return ", ".join(param_names)
    
    def _format_method_parameters_with_allocator_excluding_handle(self, parameters, allocator_param_name: str, exclude_param_name: Optional[str]) -> str:
        """Format method parameters, replacing allocator with pAllocCB, excluding a specific parameter, and converting device to pDevice."""
        # Filter out the excluded parameter first
        if exclude_param_name:
            filtered_params = [param for param in parameters if param.name != exclude_param_name]
        else:
            filtered_params = list(parameters)
        
        # Now apply allocator replacement and device conversion logic
        param_names = []
        for param in filtered_params:
            if param.name == allocator_param_name:
                param_names.append('pAllocCB')
            elif param.type_name == 'VkDevice' and param.name == 'device':
                param_names.append('pDevice')
            else:
                param_names.append(param.name)
        
        # Apply formatting similar to the original _format_method_parameters_with_allocator
        if len(param_names) >= 3 or len(", ".join(param_names)) > 80:  # Leave room for method call syntax
            return "\n" + " " * 8 + (",\n" + " " * 8).join(param_names)
        else:
            return ", ".join(param_names)
    
    def _find_allocator_parameter(self, command: Command):
        """Find the allocator callback parameter in a command."""
        for param in command.parameters:
            if 'VkAllocationCallbacks' in param.type_name and param.is_pointer:
                return param
        return None
    
    def _format_method_parameters_with_allocator(self, parameters, allocator_param_name: str, skip_first: bool = True) -> str:
        """Format method parameters, replacing allocator parameter with pAllocCB."""
        # Skip first parameter (handle) for non-global functions and replace allocator with pAllocCB
        params_to_use = parameters[1:] if skip_first else parameters
        param_names = []
        for param in params_to_use:
            if param.name == allocator_param_name:
                param_names.append('pAllocCB')
            else:
                param_names.append(param.name)
        
        # If 3 or more parameters or line would exceed 120 chars, use line wrapping
        if len(param_names) >= 3 or len(", ".join(param_names)) > 80:  # Leave room for method call syntax
            return "\n" + " " * 8 + (",\n" + " " * 8).join(param_names)
        else:
            return ", ".join(param_names)
    
    def _build_handle_context(self, command: Command, handle_param, file_class_name: str, file_object_type: str) -> Dict[str, str]:
        """Build context for handle-based functions."""
        
        # Handle embedded parameters - extract the actual parameter name to use
        if handle_param.name.startswith('_embedded_'):
            # For embedded handles, we need to determine how to extract the handle from the struct
            # For common patterns, we can make reasonable assumptions about access
            embedded_mappings = {
                'memory': {
                    'VkMemoryMapInfo': 'pMemoryMapInfo->memory',
                    'VkMemoryUnmapInfo': 'pMemoryUnmapInfo->memory',
                    'VkMemoryGetFdInfoKHR': 'pGetFdInfo->memory',
                    'VkDeviceMemoryOpaqueCaptureAddressInfo': 'pInfo->memory'
                },
                'image': {
                    'VkImageMemoryRequirementsInfo2': 'pInfo->image',
                    'VkImageSparseMemoryRequirementsInfo2': 'pInfo->image'
                },
                'buffer': {
                    'VkBufferMemoryRequirementsInfo2': 'pInfo->buffer',
                    'VkBufferDeviceAddressInfo': 'pInfo->buffer'
                }
            }
            
            # Find the struct parameter that contains the embedded handle
            struct_patterns = {
                'memory': ['VkMemoryMapInfo', 'VkMemoryUnmapInfo', 'VkMemoryGetFdInfoKHR', 'VkDeviceMemoryOpaqueCaptureAddressInfo'],
                'image': ['VkImageMemoryRequirementsInfo2', 'VkImageSparseMemoryRequirementsInfo2'],
                'buffer': ['VkBufferMemoryRequirementsInfo2', 'VkBufferDeviceAddressInfo'],
            }
            
            struct_types = struct_patterns.get(file_object_type, [])
            for param in command.parameters:
                if param.type_name in struct_types and param.is_pointer:
                    # Get the access pattern for this struct type
                    access_pattern = embedded_mappings.get(file_object_type, {}).get(param.type_name)
                    if access_pattern:
                        handle_param_name = access_pattern
                    else:
                        # Fallback: assume the struct has a field with the expected name
                        expected_field = file_object_type  # e.g., 'memory', 'image', 'buffer'
                        handle_param_name = f"{param.name}->{expected_field}"
                    break
            else:
                # Couldn't find the struct parameter, fallback to a generic name
                handle_param_name = f"embedded_{file_object_type}"
        else:
            handle_param_name = handle_param.name
        
        context = {
            'handle_param': handle_param_name,
            'object_type': file_class_name
        }
        
        # Determine function type for body template selection based on file's object type
        if file_class_name == 'Device':
            context['function_type'] = 'device'
        elif file_class_name == 'Instance':
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