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
 * @file  entry_vk_memory.cpp
 * @brief Entry point functions extracted from vk_memory.cpp
 */

#include "include/vk_conv.h"
#include "include/vk_device.h"
#include "include/vk_instance.h"
#include "include/vk_image.h"
#include "include/vk_memory.h"
#include "include/vk_utils.h"
#include "include/vk_buffer.h"
#include "palSysMemory.h"
#include "palEventDefs.h"
#include "palGpuMemory.h"
#include "palSysUtil.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkFreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator)
{
    if (memory != VK_NULL_HANDLE)
    {
        Device* pDevice = ApiDevice::ObjectFromHandle(device);
        Memory* pMemory = Memory::ObjectFromHandle(memory);

        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        pMemory->Free(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return Memory::ObjectFromHandle(memory)->Map(pDevice, flags, offset, size, ppData);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory)
{
    Memory::ObjectFromHandle(memory)->Unmap();
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory2(
    VkDevice                                    device,
    const                                       VkMemoryMapInfoKHR* pMemoryMapInfo,
    void**                                      ppData)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return Memory::ObjectFromHandle(pMemoryMapInfo->memory)->Map(
            pDevice,
            pMemoryMapInfo->flags,
            pMemoryMapInfo->offset,
            pMemoryMapInfo->size,
            ppData);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkUnmapMemory2(
    VkDevice                                    device,
    const VkMemoryUnmapInfoKHR*                 pMemoryUnmapInfo)
{
    Memory::ObjectFromHandle(pMemoryUnmapInfo->memory)->Unmap();

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges)
{
    // All of our host visible memory heaps are coherent.

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges)
{
    // All of our host visible memory heaps are coherent.

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize*                               pCommittedMemoryInBytes)
{
    Memory::ObjectFromHandle(memory)->GetCommitment(pCommittedMemoryInBytes);
}

#if defined(__unix__)
VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdKHR(
    VkDevice                                device,
    const VkMemoryGetFdInfoKHR*             pGetFdInfo,
    int*                                    pFd)
{
    VK_ASSERT(pGetFdInfo->handleType &
        (VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT   |
         VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT));

    *pFd = Memory::ObjectFromHandle(pGetFdInfo->memory)->GetShareHandle(pGetFdInfo->handleType);

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdPropertiesKHR(
    VkDevice                                device,
    VkExternalMemoryHandleTypeFlagBits      handleType,
    int                                     fd,
    VkMemoryFdPropertiesKHR*                pMemoryFdProperties)
{
    VkResult result = VK_SUCCESS;

    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    switch (handleType)
    {
        case VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT:
        {
            pMemoryFdProperties->memoryTypeBits = pDevice->GetMemoryTypeMaskForExternalSharing();
            break;
        }
        default:
        {
            pMemoryFdProperties->memoryTypeBits = 0;
            result = VK_ERROR_INVALID_EXTERNAL_HANDLE;
        }
    }

    return result;
}
#endif

// =====================================================================================================================
VKAPI_ATTR uint64_t VKAPI_CALL vkGetDeviceMemoryOpaqueCaptureAddress(
    VkDevice                                         device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo*    pInfo)
{
    const Memory* pMemory = Memory::ObjectFromHandle(pInfo->memory);

    return pMemory->PalMemory(DefaultDeviceIndex)->Desc().gpuVirtAddr;
}

} // namespace entry

} // namespace vk
