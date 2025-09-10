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
 * @file  entry_vk_pipeline_cache.cpp
 * @brief Entry point functions extracted from vk_pipeline_cache.cpp
 */

#include "include/vk_conv.h"
#include "include/vk_device.h"
#include "include/vk_instance.h"
#include "include/vk_memory.h"
#include "include/vk_physical_device.h"
#include "include/vk_pipeline_cache.h"
#include "palAutoBuffer.h"
#include "include/binary_cache_serialization.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    const VkAllocationCallbacks*                pAllocator)
{
    if (pipelineCache != VK_NULL_HANDLE)
    {
        Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        PipelineCache::ObjectFromHandle(pipelineCache)->Destroy(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    size_t*                                     pDataSize,
    void*                                       pData)
{
    size_t                     nBytesRequested = *pDataSize;
    VkPhysicalDeviceProperties physicalDeviceProps;
    VkResult                   result = VK_SUCCESS;

    Device*        pDevice = ApiDevice::ObjectFromHandle(device);
    PipelineCache* pCache = PipelineCache::ObjectFromHandle(pipelineCache);

    const Pal::DeviceProperties& palProps = pDevice->VkPhysicalDevice(DefaultDeviceIndex)->PalProperties();

    size_t privateDataSize = 0;

    result = pCache->GetData(nullptr, &privateDataSize);
    VK_ASSERT(result == VK_SUCCESS);

    const size_t       fullDataSize = VkPipelineCacheHeaderDataSize + privateDataSize;

    if (pData == nullptr)
    {
        *pDataSize = fullDataSize;

        return VK_SUCCESS;
    }

    if (nBytesRequested < fullDataSize)
    {
        // "If pDataSize is less than what is necessary to store this header, nothing will be written to pData and
        // zero will be written to pDataSize."
        *pDataSize = 0;

        result = VK_INCOMPLETE;
    }

    // The vk spec says the data should be written least significant byte first.
#ifdef BIGENDIAN_CPU
#error we need to byte-swap the data we are about to write to pData
#endif

    if (result == VK_SUCCESS)
    {
        ApiDevice::ObjectFromHandle(device)->VkPhysicalDevice(DefaultDeviceIndex)->GetDeviceProperties(&physicalDeviceProps);

        size_t       headerBytesWritten = 0;
        Util::Result headerWriteRes     =  WriteVkPipelineCacheHeaderData(
                                            pData,
                                            fullDataSize,
                                            palProps.vendorId,
                                            palProps.deviceId,
                                            physicalDeviceProps.pipelineCacheUUID,
                                            sizeof(physicalDeviceProps.pipelineCacheUUID),
                                            &headerBytesWritten);

        if (headerWriteRes != Util::Result::Success)
        {
            *pDataSize = 0;
            result = VK_INCOMPLETE;
        }
        else
        {
            VK_ASSERT(headerBytesWritten == VkPipelineCacheHeaderDataSize);
            if (privateDataSize > 0)
            {
                void* pPrivateData = Util::VoidPtrInc(pData, headerBytesWritten);
                result = pCache->GetData(pPrivateData, &privateDataSize);
            }
            // set pDataSize, privateDataSize can be 0.
            *pDataSize = privateDataSize + headerBytesWritten;
        }
    }

    return result;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(
    VkDevice                                    device,
    VkPipelineCache                             dstCache,
    uint32_t                                    srcCacheCount,
    const VkPipelineCache*                      pSrcCaches)
{
    Device*        pDevice = ApiDevice::ObjectFromHandle(device);
    PipelineCache* pDstCache = PipelineCache::ObjectFromHandle(dstCache);

    Util::AutoBuffer<const PipelineCache*, 8, PalAllocator> srcCaches(srcCacheCount, pDevice->VkInstance()->Allocator());

    for (uint32_t i = 0; i < srcCacheCount; i++)
    {
        srcCaches[i] = PipelineCache::ObjectFromHandle(pSrcCaches[i]);
    }

    VkResult result = pDstCache->Merge(srcCacheCount, &srcCaches[0]);

    return result;
}

} // namespace entry

} // namespace vk
