"""Unit tests for type mapper module."""

import unittest

from type_mapper import TypeMapper
from xml_parser import Parameter, Command, CommandType


class TestTypeMapper(unittest.TestCase):
    """Test cases for TypeMapper."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.mapper = TypeMapper()
    
    def test_vulkan_to_xgl_mapping(self):
        """Test Vulkan to XGL type mapping."""
        self.assertEqual(self.mapper.get_xgl_type('VkDevice'), 'Device')
        self.assertEqual(self.mapper.get_xgl_type('VkInstance'), 'Instance')
        self.assertEqual(self.mapper.get_xgl_type('VkBuffer'), 'Buffer')
    
    def test_unknown_type_passthrough(self):
        """Test unknown types pass through unchanged."""
        self.assertEqual(self.mapper.get_xgl_type('UnknownType'), 'UnknownType')
    
    def test_object_from_handle_generation(self):
        """Test ObjectFromHandle call generation."""
        call = self.mapper.get_object_from_handle_call('VkDevice')
        self.assertEqual(call, 'ApiDevice::ObjectFromHandle')
        
        call = self.mapper.get_object_from_handle_call('VkBuffer')
        self.assertEqual(call, 'ApiBuffer::ObjectFromHandle')
    
    def test_method_name_conversion(self):
        """Test Vulkan function name to method name conversion."""
        self.assertEqual(self.mapper.get_method_name('vkCreateFence'), 'CreateFence')
        self.assertEqual(self.mapper.get_method_name('vkDestroyBuffer'), 'DestroyBuffer')
        self.assertEqual(self.mapper.get_method_name('CustomFunction'), 'CustomFunction')
    
    def test_allocator_logic_detection(self):
        """Test allocator logic requirement detection."""
        self.assertTrue(self.mapper.needs_allocator_logic('vkCreateBuffer'))
        self.assertTrue(self.mapper.needs_allocator_logic('vkCreateFence'))
        self.assertFalse(self.mapper.needs_allocator_logic('vkGetDeviceQueue'))
    
    def test_parameter_list_formatting(self):
        """Test parameter list formatting."""
        params = [
            Parameter('device', 'VkDevice'),
            Parameter('pInfo', 'VkCreateInfo', True, True),
            Parameter('count', 'uint32_t')
        ]
        
        formatted = self.mapper.format_parameter_list(params)
        
        self.assertIn('VkDevice', formatted)
        self.assertIn('const VkCreateInfo*', formatted)
        self.assertIn('device', formatted)
        self.assertIn('pInfo', formatted)
    
    def test_method_parameters_formatting(self):
        """Test method parameter formatting."""
        params = [
            Parameter('device', 'VkDevice'),
            Parameter('pInfo', 'VkCreateInfo', True, True),
            Parameter('pResult', 'VkResult', True)
        ]
        
        # Skip first parameter (handle)
        formatted = self.mapper.format_method_parameters(params, skip_first=True)
        self.assertEqual(formatted, 'pInfo, pResult')
        
        # Include all parameters
        formatted = self.mapper.format_method_parameters(params, skip_first=False)
        self.assertEqual(formatted, 'device, pInfo, pResult')
    
    def test_allocator_parameter_detection(self):
        """Test allocator parameter detection."""
        params = [
            Parameter('device', 'VkDevice'),
            Parameter('pAllocator', 'VkAllocationCallbacks', True, True),
            Parameter('pResult', 'VkResult', True)
        ]
        
        allocator = self.mapper.get_allocator_parameter(params)
        self.assertEqual(allocator, 'pAllocator')
    
    def test_no_allocator_parameter(self):
        """Test when no allocator parameter exists."""
        params = [
            Parameter('device', 'VkDevice'),
            Parameter('count', 'uint32_t')
        ]
        
        allocator = self.mapper.get_allocator_parameter(params)
        self.assertIsNone(allocator)
    
    def test_required_includes_generation(self):
        """Test required includes generation."""
        params = [Parameter('device', 'VkDevice'), Parameter('buffer', 'VkBuffer')]
        commands = [Command('vkTest', 'void', params, CommandType.DEVICE)]
        
        includes = self.mapper.get_required_includes(commands)
        
        # Should include base headers
        self.assertIn('#include "include/vk_conv.h"', includes)
        self.assertIn('#include "include/vk_device.h"', includes)
        
        # Should include specific type headers
        self.assertIn('#include "include/vk_device.h"', includes)
        self.assertIn('#include "include/vk_buffer.h"', includes)


class TestTypeMapperConstants(unittest.TestCase):
    """Test TypeMapper constants and static data."""
    
    def test_handle_mappings_completeness(self):
        """Test that common Vulkan handles are mapped."""
        mapper = TypeMapper()
        
        common_handles = [
            'VkDevice', 'VkInstance', 'VkPhysicalDevice', 
            'VkBuffer', 'VkImage', 'VkPipeline'
        ]
        
        for handle in common_handles:
            self.assertIn(handle, mapper.HANDLE_MAPPINGS)
            self.assertTrue(mapper.HANDLE_MAPPINGS[handle])  # Non-empty mapping
    
    def test_allocator_functions_set(self):
        """Test allocator functions set contains expected functions."""
        mapper = TypeMapper()
        
        expected_functions = [
            'vkCreateBuffer', 'vkCreateFence', 'vkAllocateMemory'
        ]
        
        for func in expected_functions:
            self.assertIn(func, mapper.ALLOCATOR_FUNCTIONS)


if __name__ == '__main__':
    unittest.main()