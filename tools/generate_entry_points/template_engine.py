"""Template engine for C++ code generation."""

from string import Template
from typing import Dict, Any


class CodeTemplate:
    """C++ code template with substitutions."""
    
    def __init__(self, template_str: str):
        self.template = Template(template_str)
    
    def render(self, **kwargs) -> str:
        """Render template with given parameters."""
        return self.template.safe_substitute(**kwargs)


class TemplateRepository:
    """Repository of C++ code templates."""
    
    FILE_HEADER = CodeTemplate("""/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2014-2025 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
 * @file  $filename
 * @brief Generated entry point functions for $object_type
 * 
 * THIS FILE IS AUTO-GENERATED. DO NOT EDIT.
 * Generated from vk.xml using XGL entry point generator.
 */

$includes

namespace vk
{

namespace entry
{

$functions

} // namespace entry

} // namespace vk""")

    ENTRY_FUNCTION = CodeTemplate("""// =====================================================================================================================
VKAPI_ATTR $return_type VKAPI_CALL $function_name(
$parameters)
{$function_body
}""")

    DEVICE_FUNCTION_BODY = CodeTemplate("""
    Device* pDevice = ApiDevice::ObjectFromHandle($handle_param);
$allocator_logic
    return pDevice->$method_name($method_params);""")

    INSTANCE_FUNCTION_BODY = CodeTemplate("""
    Instance* pInstance = ApiInstance::ObjectFromHandle($handle_param);
$allocator_logic
    return pInstance->$method_name($method_params);""")

    SIMPLE_FUNCTION_BODY = CodeTemplate("""
    $object_type* pObject = Api$object_type::ObjectFromHandle($handle_param);
    return pObject->$method_name($method_params);""")

    DIRECT_FUNCTION_BODY = CodeTemplate("""
    return Api$object_type::ObjectFromHandle($handle_param)->$method_name($method_params);""")

    VOID_FUNCTION_BODY = CodeTemplate("""
    Api$object_type::ObjectFromHandle($handle_param)->$method_name($method_params);""")
    
    GLOBAL_FUNCTION_BODY = CodeTemplate("""
    return $method_name($method_params);""")

    ALLOCATOR_LOGIC = CodeTemplate("""    const VkAllocationCallbacks* pAllocCB = $allocator_param ? $allocator_param : pDevice->VkInstance()->GetAllocCallbacks();""")

    DESTROY_FUNCTION_BODY = CodeTemplate("""
    if ($handle_param != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle($device_param);
        const VkAllocationCallbacks* pAllocCB = $allocator_param ? $allocator_param : pDevice->VkInstance()->GetAllocCallbacks();

        $object_type::ObjectFromHandle($handle_param)->Destroy(pDevice, pAllocCB);
    }""")

    SELF_DESTROY_FUNCTION_BODY = CodeTemplate("""
    if ($handle_param != VK_NULL_HANDLE)
    {
        $object_type::ObjectFromHandle($handle_param)->Destroy();
    }""")

    # Templates for new implementation types
    SUCCESS_ONLY_FUNCTION_BODY = CodeTemplate("""
    // Stub implementation - returns success
    return $return_value;""")

    NOT_IMPLEMENTED_FUNCTION_BODY = CodeTemplate("""
    VK_NOT_IMPLEMENTED;""")

    NOT_IMPLEMENTED_WITH_RETURN_BODY = CodeTemplate("""
    VK_NOT_IMPLEMENTED;
    return $return_value;""")

    SIMPLE_INLINE_BODY = CodeTemplate("""
    // Simple inline implementation
$inline_implementation""")

    ASSERTION_BODY = CodeTemplate("""
    VK_ASSERT($assertion_condition);
$remaining_implementation""")


class TemplateEngine:
    """Engine for rendering C++ code templates."""
    
    def __init__(self):
        self.repo = TemplateRepository()
    
    def render_file_header(self, filename: str, object_type: str, includes: str) -> str:
        """Render file header template."""
        return self.repo.FILE_HEADER.render(
            filename=filename,
            object_type=object_type,
            includes=includes
        )
    
    def render_entry_function(self, context: Dict[str, Any]) -> str:
        """Render complete entry function."""
        return self.repo.ENTRY_FUNCTION.render(**context)
    
    def render_function_body(self, context: Dict[str, Any]) -> str:
        """Render function body based on context."""
        function_type = context.get('function_type', 'simple')
        function_name = context.get('function_name', '')
        impl_type = context.get('impl_type', 'standard')
        
        # Handle new implementation types first
        if impl_type == 'success_only':
            return_value = context.get('impl_return', 'VK_SUCCESS')
            return self.repo.SUCCESS_ONLY_FUNCTION_BODY.render(return_value=return_value)
        elif impl_type == 'empty':
            return_value = context.get('impl_return')
            if return_value and return_value != 'void':
                raise ValueError("Empty implementation cannot have a return value")
            else:
                return '\n    // This function is intentionally left empty.'
        elif impl_type == 'never_called':
            return_value = context.get('impl_return')
            if return_value and return_value != 'void':
                return f'\n    VK_NEVER_CALLED();\n\treturn {return_value};'
            else:
                return '\n    VK_NEVER_CALLED();'
        elif impl_type == 'not_implemented':
            return_value = context.get('impl_return')
            if return_value and return_value != 'void':
                return self.repo.NOT_IMPLEMENTED_WITH_RETURN_BODY.render(return_value=return_value)
            else:
                return self.repo.NOT_IMPLEMENTED_FUNCTION_BODY.render(**context)
        elif impl_type == 'simple':
            return self.repo.SIMPLE_FUNCTION_BODY.render(**context)
        
        elif impl_type == 'inline':
            # For simple implementations, we might have inline code or use existing simple logic
            if context.get('inline_implementation'):
                return self.repo.SIMPLE_INLINE_BODY.render(**context)
            # Fall through to existing logic for simple implementations
        
        # Check if this is a destroy function (legacy logic)
        if function_name.startswith('vkDestroy') and context.get('return_type') == 'void' and impl_type == 'standard':
            # Check if this is a self-destroy function (vkDestroyInstance, vkDestroyDevice)
            if function_name in ['vkDestroyInstance', 'vkDestroyDevice']:
                return self.repo.SELF_DESTROY_FUNCTION_BODY.render(**context)
            else:
                return self.repo.DESTROY_FUNCTION_BODY.render(**context)
        elif context.get('handle_param') and impl_type == 'standard':
            # Handle-based functions - use direct/void templates for simple cases
            if context.get('return_type') == 'void' and context.get('allocator_logic') == '':
                return self.repo.VOID_FUNCTION_BODY.render(**context)
            elif context.get('return_type') != 'void' and context.get('allocator_logic') == '':
                return self.repo.DIRECT_FUNCTION_BODY.render(**context)
            elif function_type == 'device':
                return self.repo.DEVICE_FUNCTION_BODY.render(**context)
            elif function_type == 'instance':
                return self.repo.INSTANCE_FUNCTION_BODY.render(**context)
            else:
                return self.repo.SIMPLE_FUNCTION_BODY.render(**context)
        elif function_type == 'global' or impl_type == 'global':
            return self.repo.GLOBAL_FUNCTION_BODY.render(**context)
        else:
            # Global function with no handle
            return self.repo.GLOBAL_FUNCTION_BODY.render(**context)
    
    def render_allocator_logic(self, allocator_param: str) -> str:
        """Render allocator callback logic."""
        return self.repo.ALLOCATOR_LOGIC.render(allocator_param=allocator_param)