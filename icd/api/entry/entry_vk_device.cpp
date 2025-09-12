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
 * @file  entry_vk_device.cpp
 * @brief Entry point functions extracted from vk_device.cpp
 */


namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateFence(
		pCreateInfo,
		pAllocCB,
		pFence);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout)
{
     return ApiDevice::ObjectFromHandle(device)->WaitForFences(
	 	fenceCount,
	 	pFences,
		waitAll,
		timeout);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences)
{
    return ApiDevice::ObjectFromHandle(device)->ResetFences(fenceCount, pFences);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue)
{
    ApiDevice::ObjectFromHandle(device)->GetQueue(
		queueFamilyIndex,
		queueIndex,
		pQueue);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue2(
    VkDevice                                    device,
    const VkDeviceQueueInfo2*                   pQueueInfo,
    VkQueue*                                    pQueue)
{
    ApiDevice::ObjectFromHandle(device)->GetQueue2(pQueueInfo, pQueue);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateSemaphore(
		pCreateInfo,
		pAllocCB,
		pSemaphore);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator)
{
    if (device != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        pDevice->Destroy(pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(
    VkDevice                                    device)
{
    return ApiDevice::ObjectFromHandle(device)->WaitIdle();
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(
    VkDevice                                    device,
    const VkEventCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkEvent*                                    pEvent)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateEvent(
		pCreateInfo,
		pAllocCB,
		pEvent);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(
    VkDevice                                    device,
    const VkQueryPoolCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkQueryPool*                                pQueryPool)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateQueryPool(
		pCreateInfo,
		pAllocCB,
		pQueryPool);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateDescriptorSetLayout(
		pCreateInfo,
		pAllocCB,
		pSetLayout);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreatePipelineLayout(
		pCreateInfo,
		pAllocCB,
		pPipelineLayout);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateFramebuffer(
		pCreateInfo,
		pAllocCB,
		pFramebuffer);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateRenderPass(
		pCreateInfo,
		pAllocCB,
		pRenderPass);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass2(
    VkDevice                                    device,
    const VkRenderPassCreateInfo2*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateRenderPass2(
		pCreateInfo,
		pAllocCB,
		pRenderPass);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateBuffer(
        pCreateInfo,
        pAllocCB,
        pBuffer);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateBufferView(
        pCreateInfo,
        pAllocCB,
        pView);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateImage(
        pCreateInfo,
        pAllocCB,
        pImage);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateImageView(
        pCreateInfo,
        pAllocCB,
        pView);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateShaderModule(
        pCreateInfo,
        pAllocCB,
        pShaderModule);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(
    VkDevice                                    device,
    const VkPipelineCacheCreateInfo*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineCache*                            pPipelineCache)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreatePipelineCache(
        pCreateInfo,
        pAllocCB,
        pPipelineCache);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateGraphicsPipelines(
        pipelineCache,
        createInfoCount,
        pCreateInfos,
        pAllocCB,
        pPipelines);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateComputePipelines(
        pipelineCache,
        createInfoCount,
        pCreateInfos,
        pAllocCB,
        pPipelines);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(
    VkDevice                                    device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateSampler(
        pCreateInfo,
        pAllocCB,
        pSampler);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateSamplerYcbcrConversion(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateSamplerYcbcrConversion(
        pCreateInfo,
        pAllocCB,
        pYcbcrConversion);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(
    VkDevice                                     device,
    const VkSwapchainCreateInfoKHR*              pCreateInfo,
    const VkAllocationCallbacks*                 pAllocator,
    VkSwapchainKHR*                              pSwapchain)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateSwapchain(
        pCreateInfo,
        pAllocCB,
        pSwapchain);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    VkExtent2D*                                 pGranularity)
{
    pGranularity->width  = 1;
    pGranularity->height = 1;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetRenderingAreaGranularity(
    VkDevice                                    device,
    const VkRenderingAreaInfoKHR*               pRenderingAreaInfo,
    VkExtent2D*                                 pGranularity)
{
    pGranularity->width  = 1;
    pGranularity->height = 1;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(
    VkDevice                                    device,
    const VkCommandBufferAllocateInfo*          pAllocateInfo,
    VkCommandBuffer*                            pCommandBuffers)
{
    return ApiDevice::ObjectFromHandle(device)->AllocateCommandBuffers(pAllocateInfo, pCommandBuffers);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateCommandPool(
        pCreateInfo,
        pAllocCB,
        pCommandPool);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();
    return pDevice->AllocMemory(
        pAllocateInfo,
        pAllocCB,
        pMemory);
}

#if defined(__unix__)
VKAPI_ATTR VkResult VKAPI_CALL vkImportSemaphoreFdKHR(
    VkDevice device,
    const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo)
{
    ImportSemaphoreInfo importInfo = {};
    importInfo.pNext            = pImportSemaphoreFdInfo->pNext;
    importInfo.handleType       = pImportSemaphoreFdInfo->handleType;
    importInfo.handle           = pImportSemaphoreFdInfo->fd;
    importInfo.importFlags      = pImportSemaphoreFdInfo->flags;
    importInfo.crossProcess     = true;

    return ApiDevice::ObjectFromHandle(device)->ImportSemaphore(pImportSemaphoreFdInfo->semaphore, importInfo);
}
#endif

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos)
{
    ApiDevice::ObjectFromHandle(device)->BindBufferMemory(bindInfoCount, pBindInfos);

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos)
{
    return ApiDevice::ObjectFromHandle(device)->BindImageMemory(bindInfoCount, pBindInfos);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorUpdateTemplate(
    VkDevice                                        device,
    const VkDescriptorUpdateTemplateCreateInfo*     pCreateInfo,
    const VkAllocationCallbacks*                    pAllocator,
    VkDescriptorUpdateTemplate*                     pDescriptorUpdateTemplate)
{
    Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateDescriptorUpdateTemplate(
        pCreateInfo,
        pAllocCB,
        pDescriptorUpdateTemplate);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceGroupPeerMemoryFeatures(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures)
{
    ApiDevice::ObjectFromHandle(device)->GetDeviceGroupPeerMemoryFeatures(
        heapIndex,
        localDeviceIndex,
        remoteDeviceIndex,
        pPeerMemoryFeatures);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupPresentCapabilitiesKHR(
    VkDevice                                    device,
    VkDeviceGroupPresentCapabilitiesKHR*        pDeviceGroupPresentCapabilities)
{
    return ApiDevice::ObjectFromHandle(device)->GetDeviceGroupPresentCapabilities(pDeviceGroupPresentCapabilities);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupSurfacePresentModesKHR(
    VkDevice                                    device,
    VkSurfaceKHR                                surface,
    VkDeviceGroupPresentModeFlagsKHR*           pModes)
{
    return ApiDevice::ObjectFromHandle(device)->GetDeviceGroupSurfacePresentModes(surface, pModes);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkDebugMarkerSetObjectTagEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectTagInfoEXT*        pTagInfo)
{
    // The SQTT layer shadows this extension's functions and contains extra code to make use of them.  This
    // extension is not enabled when the SQTT layer is not also enabled, so these functions are currently
    // just blank placeholder functions in case there will be a time where we need to do something with them
    // on this path also.

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkDebugMarkerSetObjectNameEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectNameInfoEXT*       pNameInfo)
{
    // The SQTT layer shadows this extension's functions and contains extra code to make use of them.  This
    // extension is not enabled when the SQTT layer is not also enabled, so these functions are currently
    // just blank placeholder functions in case there will be a time where we need to do something with them
    // on this path also.

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectTagEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectTagInfoEXT*         pTagInfo)
{

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectNameEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectNameInfoEXT*        pNameInfo)
{
    return ApiDevice::ObjectFromHandle(device)->SetDebugUtilsObjectName(pNameInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkSetGpaDeviceClockModeAMD(
    VkDevice                                    device,
    VkGpaDeviceClockModeInfoAMD*                pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Pal::SetClockModeInput input = {};

    input.clockMode = VkToPalDeviceClockMode(pInfo->clockMode);

    Pal::SetClockModeOutput output = {};
    Pal::Result palResult = Pal::Result::Success;

    // Set clock mode for all devices in the group unless we are querying
    if (input.clockMode != Pal::DeviceClockMode::Query)
    {
        for (uint32_t deviceIdx = 0;
            (deviceIdx < pDevice->NumPalDevices()) && (palResult == Pal::Result::Success);
            ++deviceIdx)
        {
            palResult = pDevice->PalDevice(deviceIdx)->SetClockMode(input, &output);
        }
    }
    else
    {
        palResult = pDevice->PalDevice(DefaultDeviceIndex)->SetClockMode(input, &output);

        if (palResult == Pal::Result::Success)
        {
            const Pal::DeviceProperties& properties = pDevice->VkPhysicalDevice(DefaultDeviceIndex)->PalProperties();

            pInfo->engineClockRatioToPeak = static_cast<float>(output.engineClockFrequency) /
                                            properties.gfxipProperties.performance.maxGpuClock;
            pInfo->memoryClockRatioToPeak = static_cast<float>(output.memoryClockFrequency) /
                                            properties.gpuMemoryProperties.performance.maxMemClock;
        }
    }

    return PalToVkResult(palResult);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetGpaDeviceClockInfoAMD(
    VkDevice                                  device,
    VkGpaDeviceGetClockInfoAMD*               pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Pal::SetClockModeInput input = {};

    input.clockMode = Pal::DeviceClockMode::Query;

    Pal::SetClockModeOutput output = {};

    // Get clock info for default device
    Pal::Result palResult = pDevice->PalDevice(DefaultDeviceIndex)->SetClockMode(input, &output);

    if (palResult == Pal::Result::Success)
    {
        const Pal::DeviceProperties& properties = pDevice->VkPhysicalDevice(DefaultDeviceIndex)->PalProperties();

        pInfo->engineClockRatioToPeak = static_cast<float>(output.engineClockFrequency) /
                                        properties.gfxipProperties.performance.maxGpuClock;
        pInfo->memoryClockRatioToPeak = static_cast<float>(output.memoryClockFrequency) /
                                        properties.gpuMemoryProperties.performance.maxMemClock;

        pInfo->engineClockFrequency = output.engineClockFrequency;
        pInfo->memoryClockFrequency = output.memoryClockFrequency;
    }

    return PalToVkResult(palResult);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSupport(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport)
{
    // Validate mutable descriptor support and variable descriptor count limits
    const VkMutableDescriptorTypeCreateInfoEXT* pMutableDescriptorTypeCreateInfoEXT = nullptr;
    const VkDescriptorSetLayoutBindingFlagsCreateInfo* pDescriptorSetLayoutBindingFlagsCreateInfo = nullptr;
    bool usesVariableDescriptors = false;
    {
        const VkStructHeader* pHeader = static_cast<const VkStructHeader*>(pCreateInfo->pNext);

        while (pHeader != nullptr)
        {
            switch (static_cast<uint32_t>(pHeader->sType))
            {
                case VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT:
                {
                    pMutableDescriptorTypeCreateInfoEXT =
                        reinterpret_cast<const VkMutableDescriptorTypeCreateInfoEXT*>(pHeader);

                    break;
                }

                case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO:
                {
                    pDescriptorSetLayoutBindingFlagsCreateInfo =
                        reinterpret_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(pHeader);

                    for (uint32_t i = 0; i < pDescriptorSetLayoutBindingFlagsCreateInfo->bindingCount; ++i)
                    {
                        if ((pDescriptorSetLayoutBindingFlagsCreateInfo->pBindingFlags[i] &
                            VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT) != 0)
                        {
                            usesVariableDescriptors = true;
                        }
                    }
                    break;
                }

                default:
                    break;
            }

            pHeader = reinterpret_cast<const VkStructHeader*>(pHeader->pNext);
        }
    }

    {
        VkStructHeaderNonConst* pHeader = static_cast<VkStructHeaderNonConst*>(pSupport->pNext);

        while (pHeader != nullptr)
        {
            switch (static_cast<uint32_t>(pHeader->sType))
            {
                case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_LAYOUT_SUPPORT_EXT:
                {
                    VkDescriptorSetVariableDescriptorCountLayoutSupportEXT * pDescCountLayoutSupport =
                        reinterpret_cast<VkDescriptorSetVariableDescriptorCountLayoutSupportEXT*>(pHeader);

                    if (usesVariableDescriptors == true)
                    {
                        pDescCountLayoutSupport->maxVariableDescriptorCount = UINT_MAX;
                    }
                    else
                    {
                        pDescCountLayoutSupport->maxVariableDescriptorCount = 0;
                    }

                    break;
                }

                default:
                    break;
            }

            pHeader = reinterpret_cast<VkStructHeaderNonConst*>(pHeader->pNext);
        }
    }

    pSupport->supported = VK_TRUE;

    for (uint32_t i = 0; i < pCreateInfo->bindingCount; ++i)
    {
        if (pCreateInfo->pBindings[i].descriptorType == VK_DESCRIPTOR_TYPE_MUTABLE_EXT)
        {
            if (pMutableDescriptorTypeCreateInfoEXT != nullptr)
            {
                const VkMutableDescriptorTypeListEXT& list =
                    pMutableDescriptorTypeCreateInfoEXT->pMutableDescriptorTypeLists[i];

                for (uint32_t j = 0; j < list.descriptorTypeCount; ++j)
                {
                    switch(list.pDescriptorTypes[j])
                    {
                        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                        case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                        case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                        case VK_DESCRIPTOR_TYPE_SAMPLER:
                        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                            break;
                        default:
                            pSupport->supported = VK_FALSE;
                            break;
                    }
                }
            }

            if (pSupport->supported == VK_FALSE)
            {
                break;
            }
        }
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetCalibratedTimestampsEXT(
    VkDevice                                    device,
    uint32_t                                    timestampCount,
    const VkCalibratedTimestampInfoEXT*         pTimestampInfos,
    uint64_t*                                   pTimestamps,
    uint64_t*                                   pMaxDeviation)
{
    return ApiDevice::ObjectFromHandle(device)->GetCalibratedTimestamps(
        timestampCount,
        pTimestampInfos,
        pTimestamps,
        pMaxDeviation);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreCounterValue(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    uint64_t*                                   pValue)
{
    return ApiDevice::ObjectFromHandle(device)->GetSemaphoreCounterValue(semaphore, pValue);
}

VKAPI_ATTR VkResult VKAPI_CALL vkWaitSemaphores(
    VkDevice                                    device,
    const VkSemaphoreWaitInfo*                  pWaitInfo,
    uint64_t                                    timeout)
{
    return ApiDevice::ObjectFromHandle(device)->WaitSemaphores(pWaitInfo, timeout);
}

VKAPI_ATTR VkResult VKAPI_CALL vkSignalSemaphore(
    VkDevice                                    device,
    const VkSemaphoreSignalInfo*                pSignalInfo)
{
    return ApiDevice::ObjectFromHandle(device)->SignalSemaphore(
        pSignalInfo->semaphore,
        pSignalInfo->value);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryHostPointerPropertiesEXT(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    const void*                                 pHostPointer,
    VkMemoryHostPointerPropertiesEXT*           pMemoryHostPointerProperties)
{
    VkResult result         = VK_ERROR_INVALID_EXTERNAL_HANDLE;
    const Device* pDevice   = ApiDevice::ObjectFromHandle(device);
    const uint32_t memTypes = pDevice->GetExternalHostMemoryTypes(handleType, pHostPointer);

    if (memTypes != 0)
    {
        pMemoryHostPointerProperties->memoryTypeBits = memTypes;

        result = VK_SUCCESS;
    }

    return result;
}

#if VKI_RAY_TRACING
// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureKHR(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureKHR*                 pAccelerationStructure)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateAccelerationStructureKHR(
		pCreateInfo,
		pAllocCB,
		pAccelerationStructure);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR*    pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateRayTracingPipelines(
        deferredOperation,
        pipelineCache,
        createInfoCount,
        pCreateInfos,
        pAllocCB,
        pPipelines);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkBuildAccelerationStructuresKHR(
    VkDevice                                                device,
    VkDeferredOperationKHR                                  deferredOperation,
    uint32_t                                                infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR*      pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const*  ppBuildRangeInfos)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->BuildAccelerationStructure(
        deferredOperation,
        infoCount,
        pInfos,
        ppBuildRangeInfos);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyAccelerationStructureKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    const VkCopyAccelerationStructureInfoKHR*   pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->CopyAccelerationStructure(deferredOperation, pInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyAccelerationStructureToMemoryKHR(
    VkDevice                                          device,
    VkDeferredOperationKHR                            deferredOperation,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->CopyAccelerationStructureToMemory(deferredOperation, pInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToAccelerationStructureKHR(
    VkDevice                                          device,
    VkDeferredOperationKHR                            deferredOperation,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->CopyMemoryToAccelerationStructure(deferredOperation, pInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkWriteAccelerationStructuresPropertiesKHR(
    VkDevice                                    device,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureKHR*           pAccelerationStructures,
    VkQueryType                                 queryType,
    size_t                                      dataSize,
    void*                                       pData,
    size_t                                      stride)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->WriteAccelerationStructuresProperties(
        accelerationStructureCount,
        pAccelerationStructures,
        queryType,
        dataSize,
        pData,
        stride);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingCaptureReplayShaderGroupHandlesKHR(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData)
{
    // Do exactly the same as vkGetRayTracingShaderGroupHandlesKHR. It is expected that these handles show up in
    // VkRayTracingPipelineCreateInfoKHR::VkRayTracingShaderGroupCreateInfoKHR::pShaderGroupCaptureReplayHandle when
    // replaying and we will make use of them in vkCreateRayTracingPipelinesKHR.
    RayTracingPipeline* pPipeline = RayTracingPipeline::ObjectFromHandle(pipeline);

    pPipeline->GetRayTracingShaderGroupHandles(
		DefaultDeviceIndex,
		firstGroup,
		groupCount,
		dataSize,
		pData);

    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceAccelerationStructureCompatibilityKHR(
    VkDevice                                     device,
    const VkAccelerationStructureVersionInfoKHR* pVersionInfo,
    VkAccelerationStructureCompatibilityKHR*     pCompatibility)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    pDevice->GetDeviceAccelerationStructureCompatibility(
        pVersionInfo->pVersionData,
        pCompatibility);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureBuildSizesKHR(
    VkDevice                                             device,
    VkAccelerationStructureBuildTypeKHR                  buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR*   pBuildInfo,
    const uint32_t*                                      pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR*            pSizeInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    pDevice->GetAccelerationStructureBuildSizesKHR(
        buildType,
        pBuildInfo,
        pMaxPrimitiveCounts,
        pSizeInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDeferredOperationKHR(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator,
    VkDeferredOperationKHR*                     pDeferredOperation)
{
    Device*                      pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateDeferredOperation(pAllocCB, pDeferredOperation);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetMicromapBuildSizesEXT(
    VkDevice                                    device,
    VkAccelerationStructureBuildTypeKHR         buildType,
    const VkMicromapBuildInfoEXT*               pBuildInfo,
    VkMicromapBuildSizesInfoEXT*                pSizeInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    pDevice->GetMicromapBuildSizesEXT(
        buildType,
        pBuildInfo,
        pSizeInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateMicromapEXT(
    VkDevice                                    device,
    const VkMicromapCreateInfoEXT*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkMicromapEXT*                              pMicromap)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->CreateMicromapEXT(
        pCreateInfo,
        pAllocator,
        pMicromap);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToMicromapEXT(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    const VkCopyMemoryToMicromapInfoEXT*        pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->CopyMemoryToMicromapEXT(deferredOperation, pInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyMicromapToMemoryEXT(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    const VkCopyMicromapToMemoryInfoEXT*        pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->CopyMicromapToMemoryEXT(deferredOperation, pInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkBuildMicromapsEXT(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    uint32_t                                    infoCount,
    const VkMicromapBuildInfoEXT*               pInfos)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->BuildMicromapsEXT(
        deferredOperation,
        infoCount,
        pInfos);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCopyMicromapEXT(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    const VkCopyMicromapInfoEXT*                pInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->CopyMicromapEXT(deferredOperation, pInfo);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyMicromapEXT(
    VkDevice                                    device,
    VkMicromapEXT                               micromap,
    const VkAllocationCallbacks*                pAllocator)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    pDevice->DestroyMicromapEXT(
        micromap,
        pAllocator);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkWriteMicromapsPropertiesEXT(
    VkDevice                                    device,
    uint32_t                                    micromapCount,
    const VkMicromapEXT*                        pMicromaps,
    VkQueryType                                 queryType,
    size_t                                      dataSize,
    void*                                       pData,
    size_t                                      stride)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->WriteMicromapsPropertiesEXT(
        micromapCount,
        pMicromaps,
        queryType,
        dataSize,
        pData,
        stride);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceMicromapCompatibilityEXT(
    VkDevice                                    device,
    const VkMicromapVersionInfoEXT*             pVersionInfo,
    VkAccelerationStructureCompatibilityKHR*    pCompatibility)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    return pDevice->GetDeviceMicromapCompatibilityEXT(pVersionInfo, pCompatibility);
}
#endif

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceBufferMemoryRequirements(
    VkDevice                                   device,
    const VkDeviceBufferMemoryRequirementsKHR* pInfo,
    VkMemoryRequirements2*                     pMemoryRequirements)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);
    Buffer::CalculateMemoryRequirements(pDevice,
                                        pInfo,
                                        pMemoryRequirements);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageMemoryRequirements(
    VkDevice                                  device,
    const VkDeviceImageMemoryRequirementsKHR* pInfo,
    VkMemoryRequirements2*                    pMemoryRequirements)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);
    Image::CalculateMemoryRequirements(pDevice,
                                       pInfo,
                                       pMemoryRequirements);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSparseMemoryRequirements(
    VkDevice                                  device,
    const VkDeviceImageMemoryRequirementsKHR* pInfo,
    uint32_t*                                 pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*         pSparseMemoryRequirements)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);
    Image::CalculateSparseMemoryRequirements(pDevice,
                                             pInfo,
                                             pSparseMemoryRequirementCount,
                                             pSparseMemoryRequirements);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkSetDeviceMemoryPriorityEXT(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    float                                       priority)
{
    Memory* pMemory = Memory::ObjectFromHandle(memory);
    pMemory->SetPriority(MemoryPriority::FromVkMemoryPriority(priority), false);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceFaultInfoEXT(
    VkDevice                                    device,
    VkDeviceFaultCountsEXT*                     pFaultCounts,
    VkDeviceFaultInfoEXT*                       pFaultInfo)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);
    return pDevice->GetDeviceFaultInfoEXT(pFaultCounts, pFaultInfo);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineBinariesKHR(
    VkDevice                                    device,
    const VkPipelineBinaryCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineBinaryHandlesInfoKHR*             pBinaries)
{
    const auto pDevice  = ApiDevice::ObjectFromHandle(device);
    const auto pAllocCB = (pAllocator != nullptr) ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return PipelineBinary::CreatePipelineBinaries(pDevice, pCreateInfo, pAllocCB, pBinaries);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineBinaryKHR(
    VkDevice                                    device,
    VkPipelineBinaryKHR                         pipelineBinary,
    const VkAllocationCallbacks*                pAllocator)
{
    const auto pDevice  = ApiDevice::ObjectFromHandle(device);
    const auto pAllocCB = (pAllocator != nullptr) ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();
    const auto pBinary  = PipelineBinary::ObjectFromHandle(pipelineBinary);

    pBinary->DestroyPipelineBinary(pDevice, pAllocCB);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineKeyKHR(
    VkDevice                                    device,
    const VkPipelineCreateInfoKHR*              pPipelineCreateInfo,
    VkPipelineBinaryKeyKHR*                     pPipelineBinaryKey)
{
    const auto pDevice = ApiDevice::ObjectFromHandle(device);

    return PipelineBinary::GetPipelineKey(pDevice, pPipelineCreateInfo, pPipelineBinaryKey);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineBinaryDataKHR(
    VkDevice                                    device,
    const VkPipelineBinaryDataInfoKHR*          pInfo,
    VkPipelineBinaryKeyKHR*                     pPipelineBinaryKey,
    size_t*                                     pPipelineBinaryDataSize,
    void*                                       pPipelineBinaryData)
{
    const auto pBinary = PipelineBinary::ObjectFromHandle(pInfo->pipelineBinary);

    return pBinary->GetPipelineBinaryData(
		pPipelineBinaryKey,
		pPipelineBinaryDataSize,
		pPipelineBinaryData);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkReleaseCapturedPipelineDataKHR(
    VkDevice                                    device,
    const VkReleaseCapturedPipelineDataInfoKHR* pInfo,
    const VkAllocationCallbacks*                pAllocator)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);
    const auto pPipeline = Pipeline::BaseObjectFromHandle(pInfo->pipeline);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

    return PipelineBinary::ReleaseCapturedPipelineData(
		pDevice,
		pPipeline,
		pAllocCB);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSubresourceLayout(
    VkDevice                                    device,
    const VkDeviceImageSubresourceInfoKHR*      pInfo,
    VkSubresourceLayout2KHR*                    pLayout)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);
    Image::CalculateSubresourceLayout(pDevice, pInfo->pCreateInfo, &pInfo->pSubresource->imageSubresource,
        &pLayout->subresourceLayout);
}
// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout2(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource2KHR*               pSubresource,
    VkSubresourceLayout2KHR*                    pLayout)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);

    Image::ObjectFromHandle(image)->GetSubresourceLayout(
        pDevice,
        &pSubresource->imageSubresource,
        &pLayout->subresourceLayout);
}
// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetGeneratedCommandsMemoryRequirementsNV(
    VkDevice                                            device,
    const VkGeneratedCommandsMemoryRequirementsInfoNV*  pInfo,
    VkMemoryRequirements2*                              pMemoryRequirements)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);
    const IndirectCommandsLayoutNV* pLayout = IndirectCommandsLayoutNV::ObjectFromHandle(pInfo->indirectCommandsLayout);

    pLayout->CalculateMemoryRequirements(pDevice, pMemoryRequirements);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateIndirectCommandsLayoutNV(
    VkDevice                                            device,
    const VkIndirectCommandsLayoutCreateInfoNV*         pCreateInfo,
    const VkAllocationCallbacks*                        pAllocator,
    VkIndirectCommandsLayoutNV*                         pIndirectCommandsLayout)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = (pAllocator != nullptr) ? pAllocator :
                                                                      pDevice->VkInstance()->GetAllocCallbacks();

    return pDevice->CreateIndirectCommandsLayout(pCreateInfo, pAllocCB, pIndirectCommandsLayout);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyIndirectCommandsLayoutNV(
    VkDevice                                            device,
    VkIndirectCommandsLayoutNV                          indirectCommandsLayout,
    const VkAllocationCallbacks*                        pAllocator)
{
    if (indirectCommandsLayout != VK_NULL_HANDLE)
    {
        Device* pDevice = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = (pAllocator != nullptr) ? pAllocator :
                                                                          pDevice->VkInstance()->GetAllocCallbacks();

        IndirectCommandsLayoutNV::ObjectFromHandle(indirectCommandsLayout)->Destroy(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateIndirectCommandsLayoutEXT(
    VkDevice                                        device,
    const VkIndirectCommandsLayoutCreateInfoEXT*    pCreateInfo,
    const VkAllocationCallbacks*                    pAllocator,
    VkIndirectCommandsLayoutEXT*                    pIndirectCommandsLayout)
{
    Device* pDevice = ApiDevice::ObjectFromHandle(device);
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();
    return pDevice->CreateIndirectCommandsLayout(
		pCreateInfo,
		pAllocCB,
		pIndirectCommandsLayout);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateIndirectExecutionSetEXT(
    VkDevice                                    device,
    const VkIndirectExecutionSetCreateInfoEXT*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkIndirectExecutionSetEXT*                  pIndirectExecutionSet)
{
    return VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyIndirectCommandsLayoutEXT(
    VkDevice                                    device,
    VkIndirectCommandsLayoutEXT                 indirectCommandsLayout,
    const VkAllocationCallbacks*                pAllocator)
{
    if (indirectCommandsLayout != VK_NULL_HANDLE)
    {
        Device* pDevice = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        IndirectCommandsLayout::ObjectFromHandle(indirectCommandsLayout)->Destroy(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyIndirectExecutionSetEXT(
    VkDevice                                    device,
    VkIndirectExecutionSetEXT                   indirectExecutionSet,
    const VkAllocationCallbacks*                pAllocator)
{

}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetGeneratedCommandsMemoryRequirementsEXT(
    VkDevice                                            device,
    const VkGeneratedCommandsMemoryRequirementsInfoEXT* pInfo,
    VkMemoryRequirements2*                              pMemoryRequirements)
{
    const Device* pDevice = ApiDevice::ObjectFromHandle(device);
    const IndirectCommandsLayout* pLayout = IndirectCommandsLayout::ObjectFromHandle(pInfo->indirectCommandsLayout);

    pLayout->CalculateMemoryRequirements(pDevice, pMemoryRequirements);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkUpdateIndirectExecutionSetPipelineEXT(
    VkDevice                                        device,
    VkIndirectExecutionSetEXT                       indirectExecutionSet,
    uint32_t                                        executionSetWriteCount,
    const VkWriteIndirectExecutionSetPipelineEXT*   pExecutionSetWrites)
{

}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkUpdateIndirectExecutionSetShaderEXT(
    VkDevice                                    device,
    VkIndirectExecutionSetEXT                   indirectExecutionSet,
    uint32_t                                    executionSetWriteCount,
    const VkWriteIndirectExecutionSetShaderEXT* pExecutionSetWrites)
{

}

} // namespace entry

} // namespace vk
