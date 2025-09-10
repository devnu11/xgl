"""Unit tests for XML parser module."""

import unittest
from pathlib import Path
from tempfile import NamedTemporaryFile
from xml.etree.ElementTree import Element, SubElement, ElementTree

from xml_parser import VulkanRegistryParser, CommandType, Parameter, Command


class TestVulkanRegistryParser(unittest.TestCase):
    """Test cases for VulkanRegistryParser."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.sample_xml = self._create_sample_xml()
        self.parser = VulkanRegistryParser(self.sample_xml)
    
    def tearDown(self):
        """Clean up test fixtures."""
        if self.sample_xml.exists():
            self.sample_xml.unlink()
    
    def _create_sample_xml(self) -> Path:
        """Create sample XML file for testing."""
        root = Element('registry')
        
        # Add types section
        types = SubElement(root, 'types')
        device_type = SubElement(types, 'type', category='handle')
        SubElement(device_type, 'name').text = 'VkDevice'
        
        # Add commands section
        commands = SubElement(root, 'commands')
        
        # Sample device command
        cmd1 = SubElement(commands, 'command')
        proto1 = SubElement(cmd1, 'proto')
        SubElement(proto1, 'type').text = 'VkResult'
        SubElement(proto1, 'name').text = 'vkCreateFence'
        
        param1 = SubElement(cmd1, 'param')
        SubElement(param1, 'type').text = 'VkDevice'
        SubElement(param1, 'name').text = 'device'
        
        param2 = SubElement(cmd1, 'param')
        param2.text = 'const '
        SubElement(param2, 'type').text = 'VkFenceCreateInfo'
        param2.text += '*'
        SubElement(param2, 'name').text = 'pCreateInfo'
        
        # Write to temporary file
        with NamedTemporaryFile(mode='w', suffix='.xml', delete=False) as f:
            tree = ElementTree(root)
            tree.write(f.name, encoding='unicode', xml_declaration=True)
            return Path(f.name)
    
    def test_parser_initialization(self):
        """Test parser initializes correctly."""
        self.assertIsNotNone(self.parser)
        self.assertTrue(self.sample_xml.exists())
    
    def test_parse_types(self):
        """Test type parsing."""
        types = self.parser.get_types()
        self.assertIn('VkDevice', types)
        self.assertEqual(types['VkDevice'], 'handle')
    
    def test_parse_commands(self):
        """Test command parsing."""
        commands = self.parser.get_commands()
        self.assertIn('vkCreateFence', commands)
        
        fence_cmd = commands['vkCreateFence']
        self.assertEqual(fence_cmd.name, 'vkCreateFence')
        self.assertEqual(fence_cmd.return_type, 'VkResult')
        self.assertEqual(fence_cmd.command_type, CommandType.DEVICE)
    
    def test_command_type_detection(self):
        """Test command type detection logic."""
        commands = self.parser.get_commands()
        device_commands = self.parser.get_commands_by_type(CommandType.DEVICE)
        
        self.assertTrue(len(device_commands) > 0)
        self.assertIn(commands['vkCreateFence'], device_commands)


class TestParameter(unittest.TestCase):
    """Test cases for Parameter class."""
    
    def test_parameter_creation(self):
        """Test parameter creation."""
        param = Parameter('device', 'VkDevice', False, False)
        
        self.assertEqual(param.name, 'device')
        self.assertEqual(param.type_name, 'VkDevice')
        self.assertFalse(param.is_pointer)
        self.assertFalse(param.is_const)
    
    def test_parameter_with_modifiers(self):
        """Test parameter with pointer and const."""
        param = Parameter('pInfo', 'VkCreateInfo', True, True)
        
        self.assertTrue(param.is_pointer)
        self.assertTrue(param.is_const)


class TestCommand(unittest.TestCase):
    """Test cases for Command class."""
    
    def test_command_creation(self):
        """Test command creation."""
        params = [
            Parameter('device', 'VkDevice'),
            Parameter('pInfo', 'VkCreateInfo', True, True)
        ]
        
        cmd = Command('vkCreateFence', 'VkResult', params, CommandType.DEVICE)
        
        self.assertEqual(cmd.name, 'vkCreateFence')
        self.assertEqual(cmd.return_type, 'VkResult')
        self.assertEqual(len(cmd.parameters), 2)
        self.assertEqual(cmd.command_type, CommandType.DEVICE)
    
    def test_get_first_handle_param(self):
        """Test getting first handle parameter."""
        params = [
            Parameter('device', 'VkDevice'),
            Parameter('count', 'uint32_t')
        ]
        
        cmd = Command('vkTest', 'void', params, CommandType.DEVICE)
        first_handle = cmd.get_first_handle_param()
        
        self.assertIsNotNone(first_handle)
        self.assertEqual(first_handle.name, 'device')
        self.assertEqual(first_handle.type_name, 'VkDevice')
    
    def test_no_handle_param(self):
        """Test command with no handle parameters."""
        params = [Parameter('count', 'uint32_t')]
        
        cmd = Command('vkTest', 'void', params, CommandType.GLOBAL)
        first_handle = cmd.get_first_handle_param()
        
        self.assertIsNone(first_handle)


if __name__ == '__main__':
    unittest.main()