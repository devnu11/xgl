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
 * @file  entry_vk_shader.cpp
 * @brief Entry point functions extracted from vk_shader.cpp
 */

#include "include/vk_shader.h"
#include "include/vk_device.h"
#include "include/vk_instance.h"
#include "palGpuMemoryBindable.h"
#include "palPipeline.h"
#include "palMetroHash.h"
#include <climits>

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator)
{
    if (shaderModule != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        ShaderModule::ObjectFromHandle(shaderModule)->Destroy(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetShaderModuleIdentifierEXT(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    VkShaderModuleIdentifierEXT*                pIdentifier)
{
    const ShaderModule* pShaderModule = ShaderModule::ObjectFromHandle(shaderModule);
    Pal::ShaderHash shaderHash        = pShaderModule->GetCodeHash();

    // Get the 128 bit ShaderModule Hash
    memcpy(&pIdentifier->identifier[0], &shaderHash.lower, sizeof(shaderHash.lower));
    memcpy(&pIdentifier->identifier[8], &shaderHash.upper, sizeof(shaderHash.upper));
    pIdentifier->identifierSize = sizeof(shaderHash);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetShaderModuleCreateInfoIdentifierEXT(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    VkShaderModuleIdentifierEXT*                pIdentifier)
{
    MetroHash::Hash moduleHash = {};

    Device* pDevice = ApiDevice::ObjectFromHandle(device);

    MetroHash64::Hash(
        reinterpret_cast<const uint8_t*>(pCreateInfo->pCode),
        pCreateInfo->codeSize,
        moduleHash.bytes);

    Pal::ShaderHash shaderModuleHash = ShaderModule::BuildCodeHash(
        pCreateInfo->pCode,
        pCreateInfo->codeSize);

    // Get the 128 bit ShaderModule Hash (Profile Hash)
    memcpy(&pIdentifier->identifier[0], &shaderModuleHash.lower, sizeof(shaderModuleHash.lower));
    memcpy(&pIdentifier->identifier[8], &shaderModuleHash.upper, sizeof(shaderModuleHash.upper));
    pIdentifier->identifierSize = sizeof(shaderModuleHash);
}

} // namespace entry

} // namespace vk
