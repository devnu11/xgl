"""Unit tests for code generator module."""

import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest.mock import MagicMock, Mock

from code_generator import EntryPointGenerator
from xml_parser import Command, Parameter, CommandType


class TestEntryPointGenerator(unittest.TestCase):
    """Test cases for EntryPointGenerator."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = TemporaryDirectory()
        self.output_dir = Path(self.temp_dir.name)
        
        # Mock parser
        self.mock_parser = MagicMock()
        self.mock_parser.get_commands.return_value = self._create_sample_commands()
        
        self.generator = EntryPointGenerator(self.mock_parser, self.output_dir)
    
    def tearDown(self):
        """Clean up test fixtures."""
        self.temp_dir.cleanup()
    
    def _create_sample_commands(self):
        """Create sample commands for testing."""
        device_params = [
            Parameter('device', 'VkDevice'),
            Parameter('pInfo', 'VkFenceCreateInfo', True, True),
            Parameter('pAllocator', 'VkAllocationCallbacks', True, True),
            Parameter('pFence', 'VkFence', True)
        ]
        
        instance_params = [
            Parameter('instance', 'VkInstance'),
            Parameter('pInfo', 'VkDeviceCreateInfo', True, True),
            Parameter('pDevice', 'VkDevice', True)
        ]
        
        return {
            'vkCreateFence': Command('vkCreateFence', 'VkResult', device_params, CommandType.DEVICE),
            'vkCreateDevice': Command('vkCreateDevice', 'VkResult', instance_params, CommandType.INSTANCE)
        }
    
    def test_initialization(self):
        """Test generator initialization."""
        self.assertIsNotNone(self.generator)
        self.assertEqual(self.generator.output_dir, self.output_dir)
        self.assertFalse(self.generator.verbose)
    
    def test_enable_verbose(self):
        """Test enabling verbose mode."""
        self.generator.enable_verbose()
        self.assertTrue(self.generator.verbose)
    
    def test_group_commands_by_object_type(self):
        """Test command grouping by object type."""
        commands = self._create_sample_commands()
        groups = self.generator._group_commands_by_object_type(commands)
        
        self.assertIn('device', groups)
        self.assertIn('instance', groups)
        self.assertEqual(len(groups['device']), 1)
        self.assertEqual(len(groups['instance']), 1)
    
    def test_get_object_type_from_command(self):
        """Test object type extraction from command."""
        device_cmd = self._create_sample_commands()['vkCreateFence']
        instance_cmd = self._create_sample_commands()['vkCreateDevice']
        
        device_type = self.generator._get_object_type_from_command(device_cmd)
        instance_type = self.generator._get_object_type_from_command(instance_cmd)
        
        self.assertEqual(device_type, 'device')
        self.assertEqual(instance_type, 'instance')
    
    def test_build_function_context(self):
        """Test function context building."""
        command = self._create_sample_commands()['vkCreateFence']
        context = self.generator._build_function_context(command)
        
        self.assertEqual(context['function_name'], 'vkCreateFence')
        self.assertEqual(context['return_type'], 'VkResult')
        self.assertEqual(context['method_name'], 'CreateFence')
        self.assertIn('parameters', context)
        self.assertIn('function_body', context)
    
    def test_build_handle_context_device(self):
        """Test handle context building for device commands."""
        command = self._create_sample_commands()['vkCreateFence']
        handle_param = command.get_first_handle_param()
        
        context = self.generator._build_handle_context(command, handle_param)
        
        self.assertEqual(context['handle_param'], 'device')
        self.assertEqual(context['object_type'], 'Device')
        self.assertEqual(context['function_type'], 'device')
    
    def test_build_handle_context_instance(self):
        """Test handle context building for instance commands."""
        command = self._create_sample_commands()['vkCreateDevice']
        handle_param = command.get_first_handle_param()
        
        context = self.generator._build_handle_context(command, handle_param)
        
        self.assertEqual(context['handle_param'], 'instance')
        self.assertEqual(context['object_type'], 'Instance')
        self.assertEqual(context['function_type'], 'instance')
    
    def test_generate_creates_files(self):
        """Test that generate creates output files."""
        self.generator.generate()
        
        # Check that files were created
        generated_files = list(self.output_dir.glob('*.cpp'))
        self.assertTrue(len(generated_files) > 0)
        
        # Verify content exists
        for file_path in generated_files:
            content = file_path.read_text()
            self.assertIn('VKAPI_ATTR', content)
            self.assertIn('namespace vk', content)
    
    def test_generate_single_function(self):
        """Test single function generation."""
        command = self._create_sample_commands()['vkCreateFence']
        result = self.generator._generate_single_function(command)
        
        self.assertIn('VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence', result)
        self.assertIn('VkDevice', result)
        self.assertIn('ApiDevice::ObjectFromHandle', result)
    
    def test_generate_includes(self):
        """Test includes generation."""
        commands = list(self._create_sample_commands().values())
        includes = self.generator._generate_includes(commands)
        
        self.assertIn('#include "include/vk_conv.h"', includes)
        self.assertIn('#include "include/vk_device.h"', includes)
    
    def test_generate_functions(self):
        """Test functions generation."""
        commands = list(self._create_sample_commands().values())
        functions = self.generator._generate_functions(commands)
        
        self.assertIn('vkCreateFence', functions)
        self.assertIn('vkCreateDevice', functions)
        self.assertIn('VKAPI_ATTR', functions)


class TestEntryPointGeneratorIntegration(unittest.TestCase):
    """Integration tests for EntryPointGenerator."""
    
    def setUp(self):
        """Set up integration test fixtures."""
        self.temp_dir = TemporaryDirectory()
        self.output_dir = Path(self.temp_dir.name)
        
        # Create a more realistic mock parser
        self.mock_parser = self._create_realistic_parser()
        self.generator = EntryPointGenerator(self.mock_parser, self.output_dir)
    
    def tearDown(self):
        """Clean up integration test fixtures."""
        self.temp_dir.cleanup()
    
    def _create_realistic_parser(self):
        """Create a more realistic mock parser."""
        parser = MagicMock()
        
        # Device commands
        fence_params = [
            Parameter('device', 'VkDevice'),
            Parameter('pCreateInfo', 'VkFenceCreateInfo', True, True),
            Parameter('pAllocator', 'VkAllocationCallbacks', True, True),
            Parameter('pFence', 'VkFence', True)
        ]
        
        buffer_params = [
            Parameter('device', 'VkDevice'),
            Parameter('buffer', 'VkBuffer'),
            Parameter('pAllocator', 'VkAllocationCallbacks', True, True)
        ]
        
        # Instance commands
        device_params = [
            Parameter('instance', 'VkInstance'),
            Parameter('pCreateInfo', 'VkDeviceCreateInfo', True, True),
            Parameter('pAllocator', 'VkAllocationCallbacks', True, True),
            Parameter('pDevice', 'VkDevice', True)
        ]
        
        commands = {
            'vkCreateFence': Command('vkCreateFence', 'VkResult', fence_params, CommandType.DEVICE),
            'vkDestroyBuffer': Command('vkDestroyBuffer', 'void', buffer_params, CommandType.DEVICE),
            'vkCreateDevice': Command('vkCreateDevice', 'VkResult', device_params, CommandType.INSTANCE)
        }
        
        parser.get_commands.return_value = commands
        return parser
    
    def test_full_generation_workflow(self):
        """Test complete generation workflow."""
        self.generator.enable_verbose()
        self.generator.generate()
        
        # Verify files were created
        cpp_files = list(self.output_dir.glob('*.cpp'))
        self.assertTrue(len(cpp_files) >= 2)  # At least device and instance files
        
        # Verify file contents
        for cpp_file in cpp_files:
            content = cpp_file.read_text()
            
            # Check basic structure
            self.assertIn('namespace vk', content)
            self.assertIn('namespace entry', content)
            self.assertIn('VKAPI_ATTR', content)
            
            # Check copyright header
            self.assertIn('Advanced Micro Devices', content)
            self.assertIn('THIS FILE IS AUTO-GENERATED', content)
    
    def test_device_file_generation(self):
        """Test device-specific file generation."""
        self.generator.generate()
        
        device_file = self.output_dir / 'entry_vk_device.cpp'
        if device_file.exists():
            content = device_file.read_text()
            
            self.assertIn('vkCreateFence', content)
            self.assertIn('vkDestroyBuffer', content)
            self.assertIn('ApiDevice::ObjectFromHandle', content)
    
    def test_instance_file_generation(self):
        """Test instance-specific file generation."""
        self.generator.generate()
        
        instance_file = self.output_dir / 'entry_vk_instance.cpp'
        if instance_file.exists():
            content = instance_file.read_text()
            
            self.assertIn('vkCreateDevice', content)
            self.assertIn('ApiInstance::ObjectFromHandle', content)


if __name__ == '__main__':
    unittest.main()