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
 * @file  entry_vk_deferred_operation.cpp
 * @brief Entry point functions extracted from vk_deferred_operation.cpp
 */

#include "include/vk_deferred_operation.h"
#include "include/vk_conv.h"
#include "include/vk_device.h"
#include "include/vk_instance.h"
#include "include/vk_utils.h"
#include "palFormatInfo.h"
#include <climits>

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyDeferredOperationKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation,
    const VkAllocationCallbacks*                pAllocator)
{
    Device*                      pDevice    = ApiDevice::ObjectFromHandle(device);
    DeferredHostOperation*       pOperation = DeferredHostOperation::ObjectFromHandle(operation);
    const VkAllocationCallbacks* pAllocCB   = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    pOperation->Destroy(pDevice, pAllocCB);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDeferredOperationResultKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation)
{
    DeferredHostOperation* pOperation = DeferredHostOperation::ObjectFromHandle(operation);

    return pOperation->GetOperationResult(ApiDevice::ObjectFromHandle(device));
}

// =====================================================================================================================
VKAPI_ATTR uint32_t VKAPI_CALL vkGetDeferredOperationMaxConcurrencyKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation)
{
    DeferredHostOperation* pOperation = DeferredHostOperation::ObjectFromHandle(operation);

    return pOperation->GetMaxConcurrency(ApiDevice::ObjectFromHandle(device));
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkDeferredOperationJoinKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation)
{
    DeferredHostOperation* pOperation = DeferredHostOperation::ObjectFromHandle(operation);

    return pOperation->Join(ApiDevice::ObjectFromHandle(device));
}

} // namespace entry

} // namespace vk
