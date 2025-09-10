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
 * @file  entry_vk_gpa_session.cpp
 * @brief Entry point functions extracted from vk_gpa_session.cpp
 */

#include "include/vk_cmdbuffer.h"
#include "include/vk_device.h"
#include "include/vk_gpa_session.h"
#include "palLib.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateGpaSessionAMD(
    VkDevice                                    device,
    const VkGpaSessionCreateInfoAMD*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkGpaSessionAMD*                            pGpaSession)
{
    VkResult result = GpaSession::Create(ApiDevice::ObjectFromHandle(device), pCreateInfo, pAllocator, pGpaSession);

    return result;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyGpaSessionAMD(
    VkDevice                                    device,
    VkGpaSessionAMD                             gpaSession,
    const VkAllocationCallbacks*                pAllocator)
{
    GpaSession::ObjectFromHandle(gpaSession)->Destroy(pAllocator);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCmdBeginGpaSessionAMD(
    VkCommandBuffer                             commandBuffer,
    VkGpaSessionAMD                             gpaSession)
{
    VkResult result = GpaSession::ObjectFromHandle(gpaSession)->CmdBegin(ApiCmdBuffer::ObjectFromHandle(commandBuffer));

    return result;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCmdEndGpaSessionAMD(
    VkCommandBuffer                             commandBuffer,
    VkGpaSessionAMD                             gpaSession)
{
    VkResult result = GpaSession::ObjectFromHandle(gpaSession)->CmdEnd(ApiCmdBuffer::ObjectFromHandle(commandBuffer));

    return result;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCmdBeginGpaSampleAMD(
    VkCommandBuffer                             commandBuffer,
    VkGpaSessionAMD                             gpaSession,
    const VkGpaSampleBeginInfoAMD*              pGpaSampleBeginInfo,
    uint32_t*                                   pSampleID)
{
    VkResult result = GpaSession::ObjectFromHandle(gpaSession)->CmdBeginSample(
        ApiCmdBuffer::ObjectFromHandle(commandBuffer),
        pGpaSampleBeginInfo,
        pSampleID);

    return result;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkCmdEndGpaSampleAMD(
    VkCommandBuffer                             commandBuffer,
    VkGpaSessionAMD                             gpaSession,
    uint32_t                                    sampleID)
{
    GpaSession::ObjectFromHandle(gpaSession)->CmdEndSample(ApiCmdBuffer::ObjectFromHandle(commandBuffer), sampleID);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetGpaSessionStatusAMD(
    VkDevice                                    device,
    VkGpaSessionAMD                             gpaSession)
{
    VkResult result = GpaSession::ObjectFromHandle(gpaSession)->GetStatus();

    return result;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetGpaSessionResultsAMD(
    VkDevice                                    device,
    VkGpaSessionAMD                             gpaSession,
    uint32_t                                    sampleID,
    size_t*                                     pSizeInBytes,
    void*                                       pData)
{
    VkResult result = GpaSession::ObjectFromHandle(gpaSession)->GetResults(
        sampleID,
        pSizeInBytes,
        pData);

    return result;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkResetGpaSessionAMD(
    VkDevice                                    device,
    VkGpaSessionAMD                             gpaSession)
{
    VkResult result = GpaSession::ObjectFromHandle(gpaSession)->Reset();

    return result;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkCmdCopyGpaSessionResultsAMD(
    VkCommandBuffer                             commandBuffer,
    VkGpaSessionAMD                             gpaSession)
{
    GpaSession::ObjectFromHandle(gpaSession)->CmdCopyResults(ApiCmdBuffer::ObjectFromHandle(commandBuffer));
}

} // namespace entry

} // namespace vk
