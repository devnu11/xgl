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
 * @file  entry_vk_physical_device.cpp
 * @brief Entry point functions extracted from vk_physical_device.cpp
 */

#include "include/khronos/vulkan.h"
#include "include/color_space_helper.h"
#include "include/pipeline_binary_cache.h"
#include "include/vk_buffer_view.h"
#include "include/vk_descriptor_buffer.h"
#include "include/vk_dispatch.h"
#include "include/vk_device.h"
#include "include/vk_physical_device.h"
#include "include/vk_physical_device_manager.h"
#include "include/vk_image.h"
#include "include/vk_instance.h"
#include "include/vk_utils.h"
#include "include/vk_conv.h"
#include "include/vk_surface.h"
#include "include/vk_indirect_commands_layout.h"
#include "include/khronos/vk_icd.h"
#include "llpc.h"
#include "res/ver.h"
#include "settings/settings.h"
#include "palDevice.h"
#include "palCmdBuffer.h"
#include "palFormatInfo.h"
#include "palLib.h"
#include "palMath.h"
#include "palMsaaState.h"
#include "palPlatformKey.h"
#include "palScreen.h"
#include "palHashLiteralString.h"
#include "palVectorImpl.h"
#include <string>
#include <vector>

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice)
{
    PhysicalDevice*              pPhysicalDevice = ApiPhysicalDevice::ObjectFromHandle(physicalDevice);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pPhysicalDevice->VkInstance()->GetAllocCallbacks();

    return pPhysicalDevice->CreateDevice(pCreateInfo, pAllocCB, pDevice);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice                            physicalDevice,
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->EnumerateExtensionProperties(
        pLayerName,
        pPropertyCount,
        pProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures*                   pFeatures)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetFeatures(pFeatures);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties*                 pProperties)
{
    VK_ASSERT(pProperties != nullptr);
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDeviceProperties(pProperties);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkImageFormatProperties*                    pImageFormatProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetImageFormatProperties(
        format,
        type,
        tiling,
        usage,
        flags,
#if defined(__unix__)
        DRM_FORMAT_MOD_INVALID,
#endif
        pImageFormatProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties*                         pFormatProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetFormatProperties(format, pFormatProperties);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties)
{
    // According to SDK 1.0.33 release notes, this function is deprecated.
    // However, most apps link to older vulkan loaders so we need to keep this function active just in case the app or
    // an earlier loader works incorrectly if this function is removed from the dispatch table.
    // TODO: Remove when it is safe to do so.

    if (pProperties == nullptr)
    {
        *pPropertyCount = 0;
    }

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties)
{
    *pMemoryProperties = ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetMemoryProperties();
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetQueueFamilyProperties(pQueueFamilyPropertyCount,
        pQueueFamilyProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkSampleCountFlagBits                       samples,
    VkImageUsageFlags                           usage,
    VkImageTiling                               tiling,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties*              pProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSparseImageFormatProperties(
        format,
        type,
        samples,
        usage,
        tiling,
        pPropertyCount,
        utils::ArrayView<VkSparseImageFormatProperties>(pProperties));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported)
{
    DisplayableSurfaceInfo displayableInfo = {};

    VkResult result = PhysicalDevice::UnpackDisplayableSurface(Surface::ObjectFromHandle(surface), &displayableInfo);

    const bool supported = ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->QueueSupportsPresents(queueFamilyIndex, displayableInfo.icdPlatform);

    *pSupported = supported ? VK_TRUE : VK_FALSE;

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes)
{
    DisplayableSurfaceInfo displayableInfo = {};

    VkResult result = PhysicalDevice::UnpackDisplayableSurface(Surface::ObjectFromHandle(surface), &displayableInfo);

    if (result == VK_SUCCESS)
    {
        result = ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSurfacePresentModes(
            displayableInfo,
            Pal::PresentMode::Count,
            pPresentModeCount,
            pPresentModes);
    }

    return result;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities)
{
    Pal::OsDisplayHandle osDisplayHandle = 0;

    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSurfaceCapabilities(
        surface, osDisplayHandle, pSurfaceCapabilities);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkSurfaceCapabilities2KHR*                  pSurfaceCapabilities)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSurfaceCapabilities2KHR(
        pSurfaceInfo, pSurfaceCapabilities);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats)
{
    Pal::OsDisplayHandle osDisplayhandle = 0;

    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSurfaceFormats(
                Surface::ObjectFromHandle(surface), osDisplayhandle, pSurfaceFormatCount, pSurfaceFormats);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormat2KHR*                        pSurfaceFormats)
{
    Pal::OsDisplayHandle osDisplayhandle           = 0;
    VkResult             result                    = VK_SUCCESS;
    VkSurfaceKHR         surface                   = VK_NULL_HANDLE;
    bool                 fullScreenExplicitEnabled = false;

    VK_ASSERT(pSurfaceInfo->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR);

    surface = pSurfaceInfo->surface;
    VK_ASSERT(surface != VK_NULL_HANDLE);

    const void* pNext = pSurfaceInfo->pNext;

    while (pNext != nullptr)
    {
        const auto* pHeader = static_cast<const VkStructHeader*>(pNext);

        switch (static_cast<uint32>(pHeader->sType))
        {
            default:
                break;
        }

        pNext = pHeader->pNext;
    }

    if (surface != VK_NULL_HANDLE)
    {
        result = ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSurfaceFormats(
            Surface::ObjectFromHandle(surface),
            osDisplayhandle,
            pSurfaceFormatCount,
            pSurfaceFormats);
    }

    return result;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetFeatures2(
        reinterpret_cast<VkStructHeaderNonConst*>(pFeatures), true);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDeviceProperties2(pProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetFormatProperties2(
                format,
                pFormatProperties);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetImageFormatProperties2(
                        pImageFormatInfo,
                        pImageFormatProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice                            physicalDevice,
    VkSampleCountFlagBits                       samples,
    VkMultisamplePropertiesEXT*                 pMultisampleProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDeviceMultisampleProperties(
        samples,
        pMultisampleProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetQueueFamilyProperties(
            pQueueFamilyPropertyCount,
            pQueueFamilyProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetMemoryProperties2(pMemoryProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice                                    physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2*       pFormatInfo,
    uint32_t*                                           pPropertyCount,
    VkSparseImageFormatProperties2*                     pProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSparseImageFormatProperties2(
            pFormatInfo,
            pPropertyCount,
            pProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice                                physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*       pExternalBufferInfo,
    VkExternalBufferProperties*                     pExternalBufferProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetExternalBufferProperties(
            pExternalBufferInfo,
            pExternalBufferProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice                                physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo*    pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*                  pExternalSemaphoreProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetExternalSemaphoreProperties(
            pExternalSemaphoreInfo,
            pExternalSemaphoreProperties);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties)
{
    ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetExternalFenceProperties(
        pExternalFenceInfo,
        pExternalFenceProperties);
}

#if defined(__unix__)

#ifdef VK_USE_PLATFORM_XCB_KHR
#include <xcb/xcb.h>

// =====================================================================================================================
VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXcbPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    xcb_connection_t*                           connection,
    xcb_visualid_t                              visual_id)
{
    Pal::OsDisplayHandle displayHandle = connection;
    VkIcdWsiPlatform     platform      = VK_ICD_WSI_PLATFORM_XCB;
    int64_t              visualId      = visual_id;

    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->DeterminePresentationSupported(displayHandle,
                                                                                               platform,
                                                                                               visualId,
                                                                                               queueFamilyIndex);
}
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
// =====================================================================================================================
VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXlibPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    Display*                                    dpy,
    VisualID                                    visualId)
{
    Pal::OsDisplayHandle displayHandle = dpy;
    VkIcdWsiPlatform     platform      = VK_ICD_WSI_PLATFORM_XLIB;
    int64_t              visual        = visualId;

    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->DeterminePresentationSupported(displayHandle,
                                                                                               platform,
                                                                                               visual,
                                                                                               queueFamilyIndex);
}
#endif

// =====================================================================================================================
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWaylandPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    struct wl_display*                          display)
{
    Pal::OsDisplayHandle displayHandle = display;
    VkIcdWsiPlatform     platform      = VK_ICD_WSI_PLATFORM_WAYLAND;

    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->DeterminePresentationSupported(displayHandle,
                                                                                               platform,
                                                                                               0,
                                                                                               queueFamilyIndex);
}
#endif

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkAcquireXlibDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    VkDisplayKHR                                display)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->AcquireXlibDisplay(dpy, display);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetRandROutputDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    RROutput                                    randrOutput,
    VkDisplayKHR*                               pDisplay)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetRandROutputDisplay(dpy, randrOutput, pDisplay);
}
#endif

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->ReleaseDisplay(display);
}

#endif

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pRectCount,
    VkRect2D*                                   pRects)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetPhysicalDevicePresentRectangles(
                                                surface,
                                                pRectCount,
                                                pRects);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPropertiesKHR*                     pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayProperties(
                    pPropertyCount,
                    utils::ArrayView<VkDisplayPropertiesKHR>(pProperties));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlanePropertiesKHR*                pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayPlaneProperties(
                    pPropertyCount,
                    utils::ArrayView<VkDisplayPlanePropertiesKHR>(pProperties));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    planeIndex,
    uint32_t*                                   pDisplayCount,
    VkDisplayKHR*                               pDisplays)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayPlaneSupportedDisplays(
                                                planeIndex,
                                                pDisplayCount,
                                                pDisplays);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModePropertiesKHR*                 pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayModeProperties(
                                                display,
                                                pPropertyCount,
                                                utils::ArrayView<VkDisplayModePropertiesKHR>(pProperties));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayModeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    const VkDisplayModeCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDisplayModeKHR*                           pMode)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->CreateDisplayMode(
                                                display,
                                                pCreateInfo,
                                                pAllocator,
                                                pMode);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayModeKHR                            mode,
    uint32_t                                    planeIndex,
    VkDisplayPlaneCapabilitiesKHR*              pCapabilities)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayPlaneCapabilities(
                                                mode,
                                                planeIndex,
                                                pCapabilities);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayProperties2KHR*                    pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayProperties(
                    pPropertyCount,
                    utils::ArrayView<VkDisplayPropertiesKHR>(pProperties, &pProperties->displayProperties));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlaneProperties2KHR*               pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayPlaneProperties(
                    pPropertyCount,
                    utils::ArrayView<VkDisplayPlanePropertiesKHR>(pProperties, &pProperties->displayPlaneProperties));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModeProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModeProperties2KHR*                pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayModeProperties(
                    display,
                    pPropertyCount,
                    utils::ArrayView<VkDisplayModePropertiesKHR>(pProperties, &pProperties->displayModeProperties));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkDisplayPlaneInfo2KHR*               pDisplayPlaneInfo,
    VkDisplayPlaneCapabilities2KHR*             pCapabilities)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetDisplayPlaneCapabilities(
                                                                    pDisplayPlaneInfo->mode,
                                                                    pDisplayPlaneInfo->planeIndex,
                                                                    &pCapabilities->capabilities);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilities2EXT*                  pSurfaceCapabilities)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetSurfaceCapabilities2EXT(surface,
                                                                                           pSurfaceCapabilities);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pTimeDomainCount,
    VkTimeDomainEXT*                            pTimeDomains)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetPhysicalDeviceCalibrateableTimeDomainsEXT(pTimeDomainCount,
                                                                                                             pTimeDomains);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceToolProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pToolCount,
    VkPhysicalDeviceToolPropertiesEXT*          pToolProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetPhysicalDeviceToolPropertiesEXT(pToolCount,
                                                                                                   pToolProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceFragmentShadingRatesKHR(
    VkPhysicalDevice                        physicalDevice,
    uint32*                                 pFragmentShadingRateCount,
    VkPhysicalDeviceFragmentShadingRateKHR* pFragmentShadingRates)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetFragmentShadingRates(
        pFragmentShadingRateCount,
        pFragmentShadingRates);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkCooperativeMatrixPropertiesKHR*           pProperties)
{
    return ApiPhysicalDevice::ObjectFromHandle(physicalDevice)->GetPhysicalDeviceCooperativeMatrixPropertiesKHR(
        pPropertyCount,
        pProperties);
}

}

}

} // namespace entry

} // namespace vk
