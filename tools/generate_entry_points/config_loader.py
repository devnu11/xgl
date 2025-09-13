"""Configuration loader for XGL entry point generator."""

import json5
from pathlib import Path
from typing import Dict, List, Any, Optional
from dataclasses import dataclass


@dataclass
class ClassConfig:
    """Configuration for a single XGL class."""
    vk_handle_type: str
    xgl_class_name: str
    api_prefix: bool
    function_prefix_to_strip: str
    embedded_struct_types: List[str]
    embedded_access_patterns: Dict[str, str]


@dataclass 
class FunctionOverride:
    """Configuration for a function override."""
    name: str
    method: Optional[str] = None
    impl: Optional[str] = None
    return_value: Optional[str] = None


class ConfigLoader:
    """Loads and manages configuration for entry point generation."""
    
    def __init__(self, config_dir: Path = None):
        if config_dir is None:
            config_dir = Path(__file__).parent / "config"
        self.config_dir = config_dir
        
        # Load configurations
        self._load_class_mappings()
        self._load_function_overrides()
    
    def _load_class_mappings(self):
        """Load class mapping configuration."""
        config_file = self.config_dir / "class_mappings.json5"
        with open(config_file, 'r') as f:
            data = json5.load(f)
        
        self.classes: Dict[str, ClassConfig] = {}
        for class_name, config in data["classes"].items():
            self.classes[class_name] = ClassConfig(
                vk_handle_type=config["vk_handle_type"],
                xgl_class_name=config["xgl_class_name"], 
                api_prefix=config["api_prefix"],
                function_prefix_to_strip=config["function_prefix_to_strip"],
                embedded_struct_types=config["embedded_handles"]["struct_types"],
                embedded_access_patterns=config["embedded_handles"]["access_patterns"]
            )
    
    def _load_function_overrides(self):
        """Load function override configuration."""
        config_file = self.config_dir / "function_overrides.json5"
        with open(config_file, 'r') as f:
            data = json5.load(f)
        
        # Load merged files structure
        self.enhanced_functions: Dict[str, List[FunctionOverride]] = {}
        self.file_mappings: Dict[str, List[str]] = {}
        
        for file_name, file_config in data["files"].items():
            # Extract function list (preserving order)
            self.file_mappings[file_name] = file_config["functions"]
            
            # Extract enhanced function specifications
            self.enhanced_functions[file_name] = []
            enhanced_specs = file_config.get("enhanced", {})
            
            for func_name, func_config in enhanced_specs.items():
                override = FunctionOverride(
                    name=func_name,
                    method=func_config.get("method"),
                    impl=func_config.get("impl"),
                    return_value=func_config.get("return")
                )
                self.enhanced_functions[file_name].append(override)
    
    # Methods to replace scattered mappings throughout the codebase
    
    def get_handle_mappings(self) -> Dict[str, str]:
        """Get VkHandle -> XGL class name mappings."""
        return {
            config.vk_handle_type: config.xgl_class_name 
            for config in self.classes.values()
        }
    
    def get_api_prefix_classes(self) -> List[str]:
        """Get list of classes that need Api prefix."""
        return [
            config.xgl_class_name 
            for config in self.classes.values() 
            if config.api_prefix
        ]
    
    def get_function_prefix_mappings(self) -> Dict[str, str]:
        """Get class -> function prefix mappings for stripping."""
        return {
            config.xgl_class_name: config.function_prefix_to_strip
            for config in self.classes.values()
        }
    
    def get_embedded_struct_patterns(self, object_type: str) -> List[str]:
        """Get embedded struct types for an object type."""
        # Convert object_type (e.g., "memory") to class name (e.g., "Memory")
        class_name = ''.join(word.capitalize() for word in object_type.split('_'))
        
        config = self.classes.get(class_name)
        return config.embedded_struct_types if config else []
    
    def get_embedded_access_patterns(self, object_type: str) -> Dict[str, str]:
        """Get embedded access patterns for an object type."""
        # Convert object_type (e.g., "memory") to class name (e.g., "Memory")
        class_name = ''.join(word.capitalize() for word in object_type.split('_'))
        
        config = self.classes.get(class_name)
        return config.embedded_access_patterns if config else {}
    
    def get_expected_handle_type(self, object_type: str) -> Optional[str]:
        """Get expected VkHandle type for object type."""
        # Map object_type to expected VkHandle type
        type_mappings = {
            'memory': 'VkDeviceMemory',
            'image': 'VkImage', 
            'buffer': 'VkBuffer',
            'deferred_operation': 'VkDeferredOperationKHR',
            'cmd_pool': 'VkCommandPool',
            'descriptor_pool': 'VkDescriptorPool',
            'descriptor_set': 'VkDescriptorSet',
            'descriptor_set_layout': 'VkDescriptorSetLayout',
            'device': 'VkDevice',
            'fence': 'VkFence',
            'framebuffer': 'VkFramebuffer',
            'image_view': 'VkImageView',
            'instance': 'VkInstance',
            'physical_device': 'VkPhysicalDevice',
            'pipeline': 'VkPipeline',
            'pipeline_layout': 'VkPipelineLayout',
            'query_pool': 'VkQueryPool',
            'queue': 'VkQueue',
            'render_pass': 'VkRenderPass',
            'sampler': 'VkSampler',
            'semaphore': 'VkSemaphore',
            'shader_module': 'VkShaderModule',
            'surface': 'VkSurfaceKHR',
            'cmd_buffer': 'VkCommandBuffer'
        }
        return type_mappings.get(object_type)
    
    def get_xgl_class_for_object_type(self, object_type: str) -> str:
        """Get XGL class name for object type."""
        # Convert snake_case to PascalCase and look up
        class_name = ''.join(word.capitalize() for word in object_type.split('_'))
        return class_name  # For most cases, direct conversion works
        
        # Special cases could be handled here if needed
        special_cases = {
            'cmd_pool': 'CmdPool',
            'cmd_buffer': 'CmdBuffer'
        }
        return special_cases.get(object_type, class_name)
    
    def get_enhanced_function_overrides(self, file_name: str) -> List[FunctionOverride]:
        """Get enhanced function overrides for a file."""
        return self.enhanced_functions.get(file_name, [])
    
    def get_file_function_list(self, file_name: str) -> List[str]:
        """Get list of functions mapped to a file."""
        return self.file_mappings.get(file_name, [])