/*
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
 * @file  entry_vk_debug_utils.cpp
 * @brief Entry point functions extracted from vk_debug_utils.cpp
 */

#include "include/vk_debug_utils.h"
#include "include/vk_instance.h"
#include "include/vk_device.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger)
{
    Instance* pInstance = Instance::ObjectFromHandle(instance);

    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pInstance->GetAllocCallbacks();

    return DebugUtilsMessenger::Create(
		pInstance,
		pCreateInfo,
		pAllocCB,
		pMessenger);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator)
{
    Instance* pInstance = Instance::ObjectFromHandle(instance);

    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pInstance->GetAllocCallbacks();

    DebugUtilsMessenger::ObjectFromHandle(messenger)->Destroy(pInstance, pAllocCB);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkSubmitDebugUtilsMessageEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
    Instance* pInstance = Instance::ObjectFromHandle(instance);

    pInstance->CallExternalMessengers(
		messageSeverity,
		messageTypes,
		pCallbackData);
}
} // namespace entry

} // namespace vk
