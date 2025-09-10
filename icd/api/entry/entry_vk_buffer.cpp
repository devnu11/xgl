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
 * @file  entry_vk_buffer.cpp
 * @brief Entry point functions extracted from vk_buffer.cpp
 */

#include "include/vk_buffer.h"
#include "include/vk_conv.h"
#include "include/vk_device.h"
#include "include/vk_instance.h"
#include "include/vk_memory.h"
#include "include/vk_physical_device.h"
#include "include/vk_queue.h"
#include "palGpuMemory.h"
#include "palDevice.h"
#include "palEventDefs.h"
#include "palQueue.h"
#include "palInlineFuncs.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator)
{
    if (buffer != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        Buffer::ObjectFromHandle(buffer)->Destroy(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Buffer::ObjectFromHandle(buffer)->BindMemory(pDevice, memory, memoryOffset, nullptr);

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Buffer::ObjectFromHandle(buffer)->GetMemoryRequirements(pDevice, pMemoryRequirements);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements2(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Buffer* pBuffer = Buffer::ObjectFromHandle(pInfo->buffer);
    VkMemoryRequirements* pRequirements = &pMemoryRequirements->memoryRequirements;
    pBuffer->GetMemoryRequirements(pDevice, pRequirements);

    VkMemoryDedicatedRequirements* pMemDedicatedRequirements =
        static_cast<VkMemoryDedicatedRequirements*>(pMemoryRequirements->pNext);

    if ((pMemDedicatedRequirements != nullptr) &&
        (pMemDedicatedRequirements->sType == VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS))
    {
        pMemDedicatedRequirements->prefersDedicatedAllocation  = pBuffer->DedicatedMemoryRequired();
        pMemDedicatedRequirements->requiresDedicatedAllocation = pBuffer->DedicatedMemoryRequired();
    }
}

// =====================================================================================================================
VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddress(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo* const      pInfo)
{
    Buffer* const pBuffer = Buffer::ObjectFromHandle(pInfo->buffer);

    return pBuffer->GpuVirtAddr(DefaultDeviceIndex);
}

// =====================================================================================================================
VKAPI_ATTR uint64_t VKAPI_CALL vkGetBufferOpaqueCaptureAddress(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo)
{
    Buffer* const pBuffer = Buffer::ObjectFromHandle(pInfo->buffer);

    uint64_t gpuVirtAddr = 0;

    if (pBuffer->IsSparse())
    {
        gpuVirtAddr = pBuffer->GpuVirtAddr(DefaultDeviceIndex);
    }

    return gpuVirtAddr;
}

} // namespace entry

} // namespace vk
