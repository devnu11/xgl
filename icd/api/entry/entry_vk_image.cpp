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
 * @file  entry_vk_image.cpp
 * @brief Entry point functions extracted from vk_image.cpp
 */

#include "include/vk_cmdbuffer.h"
#include "include/vk_conv.h"
#include "include/vk_device.h"
#include "include/vk_formats.h"
#include "include/vk_graphics_pipeline.h"
#include "include/vk_image.h"
#include "include/vk_instance.h"
#include "include/vk_memory.h"
#include "include/vk_swapchain.h"
#include "include/vk_queue.h"
#include "palGpuMemory.h"
#include "palImage.h"
#include "palAutoBuffer.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator)
{
    if (image != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        Image::ObjectFromHandle(image)->Destroy(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(
    VkDevice                                    device,
    VkImage                                     image,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return Image::ObjectFromHandle(image)->BindMemory(pDevice, memory, memoryOffset, 0, nullptr, 0, nullptr);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    VkMemoryRequirements*                       pMemoryRequirements)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    *pMemoryRequirements = Image::ObjectFromHandle(image)->GetMemoryRequirements();
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Image::ObjectFromHandle(image)->GetSparseMemoryRequirements(
        pDevice,
        pSparseMemoryRequirementCount,
        utils::ArrayView<VkSparseImageMemoryRequirements>(pSparseMemoryRequirements));
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource*                   pSubresource,
    VkSubresourceLayout*                        pLayout)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Image::ObjectFromHandle(image)->GetSubresourceLayout(
        pDevice,
        pSubresource,
        pLayout);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements2(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Image* pImage = Image::ObjectFromHandle(pInfo->image);
    pMemoryRequirements->memoryRequirements = pImage->GetMemoryRequirements();

    VkMemoryDedicatedRequirements* pMemDedicatedRequirements =
        static_cast<VkMemoryDedicatedRequirements*>(pMemoryRequirements->pNext);

    if ((pMemDedicatedRequirements != nullptr) &&
        (pMemDedicatedRequirements->sType == VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS))
    {
        pMemDedicatedRequirements->prefersDedicatedAllocation  = pImage->DedicatedMemoryRequired();
        pMemDedicatedRequirements->requiresDedicatedAllocation = pImage->DedicatedMemoryRequired();
    }
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements2(
    VkDevice                                        device,
    const VkImageSparseMemoryRequirementsInfo2*     pInfo,
    uint32_t*                                       pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*               pSparseMemoryRequirements)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Image* pImage = Image::ObjectFromHandle(pInfo->image);
    auto memReqsView = utils::ArrayView<VkSparseImageMemoryRequirements>(
        pSparseMemoryRequirements,
        &pSparseMemoryRequirements->memoryRequirements);
    pImage->GetSparseMemoryRequirements(pDevice, pSparseMemoryRequirementCount, memReqsView);
}

#if defined(__unix__)
// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetImageDrmFormatModifierPropertiesEXT(
    VkDevice                                    device,
    VkImage                                     image,
    VkImageDrmFormatModifierPropertiesEXT*      pProperties)
{
    Image* pImage = Image::ObjectFromHandle(image);

    pProperties->drmFormatModifier = pImage->PalImage(DefaultDeviceIndex)->GetImageCreateInfo().modifier;

    return VK_SUCCESS;
}
#endif

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToImage(
    VkDevice                                    device,
    const VkCopyImageToImageInfo*            pCopyImageToImageInfo)
{
    VK_NOT_IMPLEMENTED;
    return VK_ERROR_UNKNOWN;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToMemory(
    VkDevice                                    device,
    const VkCopyImageToMemoryInfo*              pCopyImageToMemoryInfo)
{
    VK_NOT_IMPLEMENTED;
    return VK_ERROR_UNKNOWN;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToImage(
    VkDevice                                    device,
    const VkCopyMemoryToImageInfo*              pCopyMemoryToImageInfo)
{
    VK_NOT_IMPLEMENTED;
    return VK_ERROR_UNKNOWN;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkTransitionImageLayout(
    VkDevice                                    device,
    uint32_t                                    transitionCount,
    const VkHostImageLayoutTransitionInfo*      pTransitions)
{
    VK_NOT_IMPLEMENTED;
    return VK_ERROR_UNKNOWN;
}
} // namespace entry

} // namespace vk
