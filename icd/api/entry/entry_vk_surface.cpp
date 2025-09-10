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
 * @file  entry_vk_surface.cpp
 * @brief Entry point functions extracted from vk_surface.cpp
 */

#include "vk_instance.h"
#include "vk_physical_device_manager.h"
#include "vk_surface.h"

namespace vk
{

namespace entry
{

#if defined(__unix__)
#ifdef VK_USE_PLATFORM_XCB_KHR
// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateXcbSurfaceKHR(
    VkInstance                          instance,
    const VkXcbSurfaceCreateInfoKHR*    pCreateInfo,
    const VkAllocationCallbacks*        pAllocator,
    VkSurfaceKHR*                       pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateXlibSurfaceKHR(
    VkInstance                          instance,
    const VkXlibSurfaceCreateInfoKHR*   pCreateInfo,
    const VkAllocationCallbacks*        pAllocator,
    VkSurfaceKHR*                       pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}
#endif

// =====================================================================================================================
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
VKAPI_ATTR VkResult VKAPI_CALL vkCreateWaylandSurfaceKHR(
    VkInstance                          instance,
    const VkWaylandSurfaceCreateInfoKHR*   pCreateInfo,
    const VkAllocationCallbacks*        pAllocator,
    VkSurfaceKHR*                       pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}
#endif
#endif

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(
    VkInstance                                   instance,
    VkSurfaceKHR                                 surface,
    const VkAllocationCallbacks*                 pAllocator)
{
    Surface::ObjectFromHandle(surface)->Destroy(Instance::ObjectFromHandle(instance), pAllocator);
}

} // namespace entry

} // namespace vk
