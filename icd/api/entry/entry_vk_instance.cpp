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
 * @file  entry_vk_instance.cpp
 * @brief Entry point functions extracted from vk_instance.cpp
 */

#include "include/log.h"
#include "include/khronos/vulkan.h"
#include "include/vk_alloccb.h"
#include "include/vk_conv.h"
#include "include/vk_instance.h"
#include "include/vk_physical_device.h"
#include "include/vk_physical_device_manager.h"
#include "include/virtual_stack_mgr.h"
#include "sqtt/sqtt_layer.h"
#include "sqtt/sqtt_mgr.h"
#include "include/internal_layer_hooks.h"
#include "devmode/devmode_mgr.h"
#include "devmode/devmode_rgp.h"
#include "devmode/devmode_ubertrace.h"
#include "res/ver.h"
#include "palLib.h"
#include "palDevice.h"
#include "palPlatform.h"
#include "palOglPresent.h"
#include "palListImpl.h"
#include "palInlineFuncs.h"
#include <new>

namespace vk
{

namespace entry
{

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(
    uint32_t*                                   pApiVersion)
{
    return Instance::EnumerateVersion(pApiVersion);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
    const VkInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance)
{
    return Instance::Create(
		pCreateInfo,
		pAllocator,
		pInstance);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(
    VkInstance                                  instance,
    const VkAllocationCallbacks*                pAllocator)
{
    if (instance != VK_NULL_HANDLE)
    {
        Instance::ObjectFromHandle(instance)->Destroy();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices)
{
    return Instance::ObjectFromHandle(instance)->EnumeratePhysicalDevices(pPhysicalDeviceCount, pPhysicalDevices);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceGroups(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties)
{
    return Instance::ObjectFromHandle(instance)->EnumeratePhysicalDeviceGroups(pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties)
{
    return Instance::EnumerateExtensionProperties(
        pLayerName,
        pPropertyCount,
        pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties)
{
    // We do not export any internal layers
    if (pProperties == nullptr)
    {
        *pPropertyCount = 0;
    }

    return VK_SUCCESS;
}

} // namespace entry

} // namespace vk
