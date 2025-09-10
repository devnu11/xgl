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
 * @file  entry_vk_private_data_slot.cpp
 * @brief Entry point functions extracted from vk_private_data_slot.cpp
 */

#include "include/vk_private_data_slot.h"
#include "include/vk_device.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreatePrivateDataSlot(
    VkDevice                                device,
    const VkPrivateDataSlotCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*            pAllocator,
    VkPrivateDataSlotEXT*                   pPrivateDataSlot)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return PrivateDataSlotEXT::Create(pDevice, pCreateInfo, pAllocCB, pPrivateDataSlot);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyPrivateDataSlot(
    VkDevice                                device,
    VkPrivateDataSlotEXT                    privateDataSlot,
    const VkAllocationCallbacks*            pAllocator)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();
    PrivateDataSlotEXT*          pPrivate = PrivateDataSlotEXT::ObjectFromHandle(privateDataSlot);

    pPrivate->Destroy(pDevice, pAllocCB);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkSetPrivateData(
    VkDevice                                device,
    VkObjectType                            objectType,
    uint64_t                                objectHandle,
    VkPrivateDataSlotEXT                    privateDataSlot,
    uint64_t                                data)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    PrivateDataSlotEXT*          pPrivate = PrivateDataSlotEXT::ObjectFromHandle(privateDataSlot);

    return pPrivate->SetPrivateDataEXT(pDevice, objectType, objectHandle, data);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPrivateData(
    VkDevice                                device,
    VkObjectType                            objectType,
    uint64_t                                objectHandle,
    VkPrivateDataSlotEXT                    privateDataSlot,
    uint64_t*                               pData)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    PrivateDataSlotEXT*          pPrivate = PrivateDataSlotEXT::ObjectFromHandle(privateDataSlot);

    return pPrivate->GetPrivateDataEXT(pDevice, objectType, objectHandle, pData);
}

} // namespace entry

} // namespace vk
