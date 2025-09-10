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
 * @file  entry_vk_descriptor_pool.cpp
 * @brief Entry point functions extracted from vk_descriptor_pool.cpp
 */

#include "include/vk_descriptor_pool.h"
#include "include/vk_conv.h"
#include "include/vk_device.h"
#include "include/vk_dispatch.h"
#include "include/vk_instance.h"
#include "include/vk_utils.h"
#include "include/vk_queue.h"
#include "include/vk_descriptor_set_layout.h"
#include "include/vk_descriptor_set.h"
#include "palInlineFuncs.h"
#include "palDevice.h"
#include "palEventDefs.h"
#include "palGpuMemory.h"

namespace vk
{

namespace entry
{

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->GetEntryPoints().vkCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
}

VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->GetEntryPoints().vkFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    VkDescriptorPoolResetFlags                  flags)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->GetEntryPoints().vkResetDescriptorPool(device, descriptorPool, flags);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator)
{
    if (descriptorPool != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        DescriptorPool::ObjectFromHandle(descriptorPool)->Destroy(pDevice, pAllocCB);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(
    VkDevice                                    device,
    const VkDescriptorSetAllocateInfo*          pAllocateInfo,
    VkDescriptorSet*                            pDescriptorSets)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->GetEntryPoints().vkAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
}

} // namespace entry

} // namespace vk
