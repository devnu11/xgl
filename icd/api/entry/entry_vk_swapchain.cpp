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
 * @file  entry_vk_swapchain.cpp
 * @brief Entry point functions extracted from vk_swapchain.cpp
 */


namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator)
{
    if (swapchain != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        SwapChain::ObjectFromHandle(swapchain)->Destroy(pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(
    VkDevice                                     device,
    VkSwapchainKHR                               swapchain,
    uint32_t*                                    pSwapchainImageCount,
    VkImage*                                     pSwapchainImages)
{
    return SwapChain::ObjectFromHandle(swapchain)->GetSwapchainImagesKHR(pSwapchainImageCount, pSwapchainImages);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(
    VkDevice                                     device,
    VkSwapchainKHR                               swapchain,
    uint64_t                                     timeout,
    VkSemaphore                                  semaphore,
    VkFence                                      fence,
    uint32_t*                                    pImageIndex)
{
    constexpr uint32_t deviceMask = 1;

    const VkAcquireNextImageInfoKHR acquireInfo =
    {
        VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
        nullptr,
        swapchain,
        timeout,
        semaphore,
        fence,
        deviceMask
    };

    union
    {
        const VkStructHeader*             pHeader;
        const VkAcquireNextImageInfoKHR*  pAcquireInfoKHR;
    };

    pAcquireInfoKHR = &acquireInfo;

    return SwapChain::ObjectFromHandle(swapchain)->AcquireNextImage(pHeader, pImageIndex);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImage2KHR(
    VkDevice                                    device,
    const VkAcquireNextImageInfoKHR*            pAcquireInfo,
    uint32_t*                                   pImageIndex)
{
    union
    {
        const VkStructHeader*             pHeader;
        const VkAcquireNextImageInfoKHR*  pAcquireInfoKHR;
    };

    pAcquireInfoKHR = pAcquireInfo;

    return SwapChain::ObjectFromHandle(pAcquireInfo->swapchain)->AcquireNextImage(pHeader, pImageIndex);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkSetHdrMetadataEXT(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainKHR*                       pSwapchains,
    const VkHdrMetadataEXT*                     pMetadata)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    for (uint32_t swapChainIndex = 0; (swapChainIndex < swapchainCount); swapChainIndex++)
    {
        SwapChain::ObjectFromHandle(pSwapchains[swapChainIndex])->SetHdrMetadata(pMetadata);
    }
}

} // namespace entry

} // namespace vk
