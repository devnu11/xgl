"""Type mapping between Vulkan and XGL types."""

from typing import Dict, Optional, Set
from xml_parser import Parameter, Command


class TypeMapper:
    """Maps Vulkan types to XGL equivalents and generates conversions."""
    
    # Special cases that don't follow the simple "strip Vk prefix" rule
    HANDLE_MAPPINGS: Dict[str, str] = {
        'VkCommandBuffer': 'CmdBuffer',
        'VkCommandPool': 'CmdPool',
        'VkSurfaceKHR': 'Surface'
    }
    
    # Functions that need allocator callback handling
    ALLOCATOR_FUNCTIONS: Set[str] = {
        'vkCreateBuffer', 'vkCreateImage', 'vkCreateFence', 
        'vkCreateSemaphore', 'vkCreateEvent', 'vkAllocateMemory',
        'vkCreatePipeline', 'vkCreateRenderPass', 'vkCreateFramebuffer'
    }
    
    def __init__(self):
        self.includes_cache: Dict[str, Set[str]] = {}
    
    def get_xgl_type(self, vk_type: str) -> str:
        """Map Vulkan type to XGL type."""
        # Check for explicit mapping first
        if vk_type in self.HANDLE_MAPPINGS:
            vk_type = self.HANDLE_MAPPINGS[vk_type]
        
        # Default: strip 'Vk' prefix if present
        elif vk_type.startswith('Vk'):
            vk_type = vk_type[2:]  # Remove 'Vk' prefix
        
        return vk_type
    
    def get_object_from_handle_call(self, vk_type: str) -> str:
        """Generate ObjectFromHandle call for given type."""
        xgl_type = self.get_xgl_type(vk_type)
        return f"Api{xgl_type}::ObjectFromHandle"
    
    def get_method_name(self, function_name: str) -> str:
        """Convert Vulkan function name to XGL method name."""
        if function_name.startswith('vk'):
            return function_name[2:]  # Remove 'vk' prefix
        return function_name
    
    def needs_allocator_logic(self, function_name: str) -> bool:
        """Check if function needs allocator callback logic."""
        return function_name in self.ALLOCATOR_FUNCTIONS
    
    def get_required_includes(self, commands: list[Command]) -> str:
        """Get required includes for given commands as formatted string."""
        includes = set()
        
        for command in commands:
            # Add includes based on parameter types
            for param in command.parameters:
                if param.type_name in self.HANDLE_MAPPINGS:
                    include_name = param.type_name.lower().replace('vk', '')
                    includes.add(f'#include "include/vk_{include_name}.h"')
        
        # Always include common headers
        base_includes = [
            '#include "include/vk_conv.h"',
            '#include "include/vk_device.h"',
            '#include "include/vk_instance.h"'
        ]
        
        all_includes = base_includes + sorted(list(includes))
        return '\n'.join(all_includes)
    
    def format_parameter_list(self, parameters: list[Parameter]) -> str:
        """Format parameter list for function signature."""
        formatted_params = []
        
        for param in parameters:
            param_str = " " * 4 # Indent
            param_str += "const " if param.is_const else ""         
            param_str += param.type_name
            param_str += "*" if param.is_pointer else ""            
            param_str += " " * max(0, 48 - len(param_str))  # Pad type for alignment (XGL style)
            param_str += param.name
            
            formatted_params.append(param_str)
        
        return ",\n".join(formatted_params)
    
    def format_method_parameters(self, parameters: list[Parameter], skip_first: bool = True) -> str:
        """Format parameters for method call (excluding handle parameter)."""
        params_to_use = parameters[1:] if skip_first else parameters
        param_names = [param.name for param in params_to_use]
        
        # If 3 or more parameters or line would exceed 120 chars, use line wrapping
        if len(param_names) >= 3 or len(", ".join(param_names)) > 80:  # Leave room for method call syntax
            return "\n" + " " * 8 + (",\n" + " " * 8).join(param_names)
        else:
            return ", ".join(param_names)
    
    def get_allocator_parameter(self, parameters: list[Parameter]) -> Optional[str]:
        """Find allocator callback parameter."""
        for param in parameters:
            if 'Allocation' in param.type_name:
                return param.name
        return None