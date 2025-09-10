"""Unit tests for template engine module."""

import unittest

from template_engine import TemplateEngine, TemplateRepository, CodeTemplate


class TestCodeTemplate(unittest.TestCase):
    """Test cases for CodeTemplate."""
    
    def test_basic_substitution(self):
        """Test basic template substitution."""
        template = CodeTemplate("Hello $name!")
        result = template.render(name="World")
        self.assertEqual(result, "Hello World!")
    
    def test_multiple_substitutions(self):
        """Test multiple variable substitution."""
        template = CodeTemplate("$greeting $name from $location")
        result = template.render(greeting="Hello", name="Alice", location="Paris")
        self.assertEqual(result, "Hello Alice from Paris")
    
    def test_missing_variable_safe_substitute(self):
        """Test that missing variables are left as-is."""
        template = CodeTemplate("Hello $name, welcome to $place")
        result = template.render(name="Bob")
        self.assertEqual(result, "Hello Bob, welcome to $place")


class TestTemplateRepository(unittest.TestCase):
    """Test cases for TemplateRepository."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.repo = TemplateRepository()
    
    def test_file_header_template(self):
        """Test file header template."""
        result = self.repo.FILE_HEADER.render(
            filename="test.cpp",
            object_type="Device",
            includes="#include <test.h>",
            functions="// functions here"
        )
        
        self.assertIn("test.cpp", result)
        self.assertIn("Device", result)
        self.assertIn("#include <test.h>", result)
        self.assertIn("// functions here", result)
        self.assertIn("namespace vk", result)
    
    def test_entry_function_template(self):
        """Test entry function template."""
        result = self.repo.ENTRY_FUNCTION.render(
            return_type="VkResult",
            function_name="vkCreateFence",
            parameters="VkDevice device",
            function_body="\\n    return VK_SUCCESS;"
        )
        
        self.assertIn("VKAPI_ATTR VkResult VKAPI_CALL", result)
        self.assertIn("vkCreateFence", result)
        self.assertIn("VkDevice device", result)
        self.assertIn("return VK_SUCCESS;", result)
    
    def test_device_function_body(self):
        """Test device function body template."""
        result = self.repo.DEVICE_FUNCTION_BODY.render(
            handle_param="device",
            method_name="CreateFence",
            method_params="pInfo, pAllocCB, pFence"
        )
        
        self.assertIn("ApiDevice::ObjectFromHandle(device)", result)
        self.assertIn("pDevice->CreateFence", result)
        self.assertIn("pInfo, pAllocCB, pFence", result)
    
    def test_instance_function_body(self):
        """Test instance function body template."""
        result = self.repo.INSTANCE_FUNCTION_BODY.render(
            handle_param="instance",
            method_name="CreateDevice",
            method_params="pInfo, pAllocCB, pDevice"
        )
        
        self.assertIn("ApiInstance::ObjectFromHandle(instance)", result)
        self.assertIn("pInstance->CreateDevice", result)
    
    def test_void_function_body(self):
        """Test void function body template."""
        result = self.repo.VOID_FUNCTION_BODY.render(
            object_type="Device",
            handle_param="device",
            method_name="DestroyFence",
            method_params="fence, pAllocator"
        )
        
        self.assertIn("ApiDevice::ObjectFromHandle(device)->DestroyFence", result)
        self.assertIn("fence, pAllocator", result)
    
    def test_allocator_logic(self):
        """Test allocator logic template."""
        result = self.repo.ALLOCATOR_LOGIC.render(allocator_param="pAllocator")
        
        self.assertIn("pAllocCB", result)
        self.assertIn("pAllocator", result)
        self.assertIn("pDevice->VkInstance()->GetAllocCallbacks()", result)


class TestTemplateEngine(unittest.TestCase):
    """Test cases for TemplateEngine."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.engine = TemplateEngine()
    
    def test_render_file_header(self):
        """Test file header rendering."""
        result = self.engine.render_file_header(
            filename="entry_device.cpp",
            object_type="Device",
            includes="#include \\"vk_device.h\\""
        )
        
        self.assertIn("entry_device.cpp", result)
        self.assertIn("Device", result)
        self.assertIn('#include "vk_device.h"', result)
    
    def test_render_device_function_body(self):
        """Test device function body rendering."""
        context = {
            'function_type': 'device',
            'handle_param': 'device',
            'method_name': 'CreateFence',
            'method_params': 'pInfo, pAllocCB, pFence'
        }
        
        result = self.engine.render_function_body(context)
        
        self.assertIn("Device* pDevice = ApiDevice::ObjectFromHandle", result)
        self.assertIn("pDevice->CreateFence", result)
    
    def test_render_instance_function_body(self):
        """Test instance function body rendering."""
        context = {
            'function_type': 'instance',
            'handle_param': 'instance',
            'method_name': 'CreateDevice',
            'method_params': 'pInfo'
        }
        
        result = self.engine.render_function_body(context)
        
        self.assertIn("Instance* pInstance = ApiInstance::ObjectFromHandle", result)
    
    def test_render_void_function_body(self):
        """Test void function body rendering."""
        context = {
            'return_type': 'void',
            'object_type': 'Device',
            'handle_param': 'device',
            'method_name': 'DestroyFence',
            'method_params': 'fence'
        }
        
        result = self.engine.render_function_body(context)
        
        self.assertIn("ApiDevice::ObjectFromHandle(device)->DestroyFence", result)
    
    def test_render_simple_function_body(self):
        """Test simple function body rendering."""
        context = {
            'object_type': 'Buffer',
            'handle_param': 'buffer',
            'method_name': 'GetSize',
            'method_params': ''
        }
        
        result = self.engine.render_function_body(context)
        
        self.assertIn("Buffer* pObject = ApiBuffer::ObjectFromHandle", result)
        self.assertIn("pObject->GetSize", result)
    
    def test_render_allocator_logic(self):
        """Test allocator logic rendering."""
        result = self.engine.render_allocator_logic("pAllocator")
        
        self.assertIn("pAllocCB", result)
        self.assertIn("pAllocator", result)
    
    def test_complete_entry_function(self):
        """Test complete entry function rendering."""
        context = {
            'function_name': 'vkCreateFence',
            'return_type': 'VkResult',
            'parameters': 'VkDevice device,\\n    const VkFenceCreateInfo* pInfo',
            'function_body': '\\n    return VK_SUCCESS;'
        }
        
        result = self.engine.render_entry_function(context)
        
        self.assertIn("VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence", result)
        self.assertIn("VkDevice device", result)
        self.assertIn("return VK_SUCCESS;", result)


if __name__ == '__main__':
    unittest.main()