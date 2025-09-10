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
 * @file  entry_vk_pipeline.cpp
 * @brief Entry point functions extracted from vk_pipeline.cpp
 */

#include "include/vk_compute_pipeline.h"
#include "include/vk_conv.h"
#include "include/vk_device.h"
#include "include/vk_graphics_pipeline.h"
#include "include/vk_instance.h"
#include "include/vk_memory.h"
#include "include/vk_physical_device.h"
#include "include/vk_pipeline.h"
#include "include/vk_shader.h"
#include "include/vk_pipeline_layout.h"
#include "include/vk_pipeline_cache.h"
#include "include/vk_utils.h"

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator)
{
    if (pipeline != VK_NULL_HANDLE)
    {
        Device*                      pDevice  = ApiDevice::ObjectFromHandle(device);
        const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pDevice->VkInstance()->GetAllocCallbacks();

        Pipeline::BaseObjectFromHandle(pipeline)->Destroy(pDevice, pAllocCB);
    }
}

// =====================================================================================================================
// Implementation of vkGetShaderInfoAMD for VK_AMD_shader_info
VKAPI_ATTR VkResult VKAPI_CALL vkGetShaderInfoAMD(
    VkDevice               device,
    VkPipeline             pipeline,
    VkShaderStageFlagBits  shaderStage,
    VkShaderInfoTypeAMD    infoType,
    size_t*                pBufferSize,
    void*                  pBuffer)
{
    VkResult result = VK_ERROR_FEATURE_NOT_PRESENT;

    const Device*         pDevice      = ApiDevice::ObjectFromHandle(device);
    const Pipeline*       pPipeline    = Pipeline::BaseObjectFromHandle(pipeline);
    const Pal::IPipeline* pPalPipeline = pPipeline->PalPipeline(DefaultDeviceIndex);

    if (pPipeline != nullptr)
    {
        Pal::ShaderType shaderType = VkToPalShaderType(shaderStage);

        if (infoType == VK_SHADER_INFO_TYPE_STATISTICS_AMD)
        {
            Pal::ShaderStats palStats = {};
            Pal::Result palResult = pPalPipeline->GetShaderStats(shaderType, &palStats, true);

            if ((palResult == Pal::Result::Success) ||
                (palResult == Pal::Result::ErrorInvalidMemorySize)) // This error is harmless and is a PAL bug w/around
            {
                if (pBufferSize != nullptr)
                {
                    *pBufferSize = sizeof(VkShaderStatisticsInfoAMD);
                }

                if (pBuffer != nullptr)
                {
                    VkShaderStatisticsInfoAMD* pStats = static_cast<VkShaderStatisticsInfoAMD*>(pBuffer);

                    ConvertShaderInfoStatistics(palStats, pStats);

                    const Pal::DeviceProperties& info = pDevice->GetPalProperties();

                    pStats->numPhysicalVgprs = info.gfxipProperties.shaderCore.vgprsPerSimd;
                    pStats->numPhysicalSgprs = info.gfxipProperties.shaderCore.sgprsPerSimd;
                }

                result = VK_SUCCESS;
            }
        }
        else if (infoType == VK_SHADER_INFO_TYPE_DISASSEMBLY_AMD)
        {
            result = pPipeline->GetShaderDisassembly(
                pDevice,
                pPalPipeline,
                Util::Abi::PipelineSymbolType::ShaderDisassembly,
                shaderType,
                pBufferSize,
                pBuffer);
        }
        else if (infoType == VK_SHADER_INFO_TYPE_BINARY_AMD)
        {
            PipelineBinaryInfo binaryInfo = {};
            bool hasBinary = pPipeline->GetBinary(shaderType, &binaryInfo);

            if (hasBinary && (binaryInfo.pipelineBinary.pCode != nullptr))
            {
                if (pBuffer != nullptr)
                {
                    const size_t copySize = Util::Min(*pBufferSize, binaryInfo.pipelineBinary.codeSize);

                    memcpy(pBuffer, binaryInfo.pipelineBinary.pCode, copySize);

                    result = (copySize == binaryInfo.pipelineBinary.codeSize) ? VK_SUCCESS : VK_INCOMPLETE;
                }
                else
                {
                    *pBufferSize = binaryInfo.pipelineBinary.codeSize;

                    result = VK_SUCCESS;
                }
            }
        }
    }
    else
    {
        result = VK_ERROR_INITIALIZATION_FAILED;
    }

    return result;
}

// =====================================================================================================================
static void BuildPipelineNameDescription(
    const char*              pPreName,
    const char*              pShaderName,
    char*                    pName,
    char*                    pDescription,
    Util::Abi::HardwareStage hwStage,
    uint32_t                 palShaderMask)
{
    // Build a name and description string for the HW Shader
    char shaderName[VK_MAX_DESCRIPTION_SIZE];
    Util::Strncpy(shaderName, pPreName, VK_MAX_DESCRIPTION_SIZE);
    Util::Strncat(shaderName, VK_MAX_DESCRIPTION_SIZE, pShaderName);
    strncpy(pName, shaderName, VK_MAX_DESCRIPTION_SIZE);

    // Build the description string using the VkShaderStageFlagBits
    // that correspond to the HW Shader
    char shaderDescription[VK_MAX_DESCRIPTION_SIZE];

    // Beginning of the description
    Util::Strncpy(shaderDescription, "Executable handles following Vulkan stages: ", VK_MAX_DESCRIPTION_SIZE);

    if (palShaderMask & Pal::ApiShaderStageCompute)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_COMPUTE_BIT ");
    }

    if (palShaderMask & Pal::ApiShaderStageTask)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_TASK_BIT_EXT ");
    }

    if (palShaderMask & Pal::ApiShaderStageVertex)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_VERTEX_BIT ");
    }

    if (palShaderMask & Pal::ApiShaderStageHull)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT ");
    }

    if (palShaderMask & Pal::ApiShaderStageDomain)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ");
    }

    if (palShaderMask & Pal::ApiShaderStageGeometry)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_GEOMETRY_BIT ");
    }

    if (palShaderMask & Pal::ApiShaderStageMesh)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_MESH_BIT_EXT ");
    }

    if (palShaderMask & Pal::ApiShaderStagePixel)
    {
        Util::Strncat(shaderDescription, VK_MAX_DESCRIPTION_SIZE, " VK_SHADER_STAGE_FRAGMENT_BIT ");
    }

    // Copy built string to the description with remainder of the string \0 filled.
    // Having the \0 to VK_MAX_DESCRIPTION_SIZE is a requirement to get the cts tests to pass.
    strncpy(pDescription, shaderDescription, VK_MAX_DESCRIPTION_SIZE);
}

// =====================================================================================================================
static uint32_t CountNumberOfHWStages(
    uint32_t*                            pHwStageMask,
    const Util::Abi::ApiHwShaderMapping& apiToHwShader)
{
    VK_ASSERT(pHwStageMask != nullptr);

    *pHwStageMask = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(Util::Abi::ApiShaderType::Count); i++)
    {
         uint32_t hwStage = 0;
         if (Util::BitMaskScanForward(&hwStage, apiToHwShader.apiShaders[static_cast<uint32_t>(i)]))
         {
             *pHwStageMask |= (1 << hwStage);
         }
    }

    // The number of bits set in the HW Mask is the number HW shaders used
    return Util::CountSetBits(*pHwStageMask);
}

// =====================================================================================================================
// Get HW Stage for executable index
static Util::Abi::HardwareStage GetHwStageForExecutableIndex(
    uint32_t executableIndex,
    uint32_t hwStageMask)
{
    uint32_t hwStage = 0;
    for (uint32_t i = 0; i <= executableIndex; ++i)
    {
        Util::BitMaskScanForward(&hwStage, hwStageMask);
        hwStageMask &= ~(1 << hwStage);
    }

    // HW Stage should never exceed number of available HW Stages
    VK_ASSERT(hwStage < static_cast<uint32_t>(Util::Abi::HardwareStage::Count));

    return static_cast<Util::Abi::HardwareStage>(hwStage);
}

// =====================================================================================================================
// Convert from the HW Shader stage back to the corresponding API Stage
static Pal::ShaderType GetApiShaderFromHwShader(
    Util::Abi::HardwareStage             hwStage,
    const Util::Abi::ApiHwShaderMapping& apiToHwShader)
{
    Pal::ShaderType apiShaderType = Pal::ShaderType::Compute;
    for (uint32_t i = 0; i < static_cast<uint32_t>(Util::Abi::ApiShaderType::Count); ++i)
    {
        uint32_t apiHWStage = 0;
        Util::BitMaskScanForward(&apiHWStage, apiToHwShader.apiShaders[i]);

        if (apiToHwShader.apiShaders[i] & (1 << static_cast<uint32_t>(hwStage)))
        {
            switch (static_cast<Util::Abi::ApiShaderType>(i))
            {
            case Util::Abi::ApiShaderType::Cs:
                apiShaderType = Pal::ShaderType::Compute;
                break;
            case Util::Abi::ApiShaderType::Task:
                apiShaderType = Pal::ShaderType::Task;
                break;
            case Util::Abi::ApiShaderType::Vs:
                apiShaderType = Pal::ShaderType::Vertex;
                break;
            case Util::Abi::ApiShaderType::Hs:
                apiShaderType = Pal::ShaderType::Hull;
                break;
            case Util::Abi::ApiShaderType::Ds:
                apiShaderType = Pal::ShaderType::Domain;
                break;
            case Util::Abi::ApiShaderType::Gs:
                apiShaderType = Pal::ShaderType::Geometry;
                break;
            case Util::Abi::ApiShaderType::Mesh:
                apiShaderType = Pal::ShaderType::Mesh;
                break;
            case Util::Abi::ApiShaderType::Ps:
                apiShaderType = Pal::ShaderType::Pixel;
                break;
            default:
                // Util::Abi::ApiShaderType mapping to Pal::ShaderType does not match!
                VK_NEVER_CALLED();
                break;
            }
            break;
        }
    }

    return apiShaderType;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineExecutablePropertiesKHR(
    VkDevice                                    device,
    const VkPipelineInfoKHR*                    pPipelineInfo,
    uint32_t*                                   pExecutableCount,
    VkPipelineExecutablePropertiesKHR*          pProperties)
{
    const Pipeline*                     pPipeline     = Pipeline::BaseObjectFromHandle(pPipelineInfo->pipeline);
    const Pal::IPipeline*               pPalPipeline  = pPipeline->PalPipeline(DefaultDeviceIndex);
    const Util::Abi::ApiHwShaderMapping apiToHwShader = pPalPipeline->ApiHwShaderMapping();

#if VKI_RAY_TRACING
    if (pPipeline->GetType() == VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR)
    {
        const RayTracingPipeline* pRayTracingPipeline = static_cast<const RayTracingPipeline*>(pPipeline);

        return pRayTracingPipeline->GetPipelineExecutableProperties(pPipelineInfo,
                                                                pExecutableCount,
                                                                pProperties);
    }
#endif

    // Count the number of hardware stages that are used in this pipeline
    uint32_t hwStageMask = 0;
    uint32_t numHWStages = CountNumberOfHWStages(&hwStageMask, apiToHwShader);

    // If pProperties == nullptr the call to this function is just ment to return the number of executables
    // in the pipeline
    if (pProperties == nullptr)
    {
        *pExecutableCount = numHWStages;
        return VK_SUCCESS;
    }

    VkShaderStatisticsInfoAMD  vkShaderStats   = {};
    Pal::ShaderStats           palStats        = {};
    uint32_t                   outputCount     = 0;
    constexpr char             shaderPreName[] = "ShaderProperties";

    // Return the name / description for the pExecutableCount number of executables.
    uint32 i = 0;
    while (Util::BitMaskScanForward(&i, hwStageMask) && (outputCount < *pExecutableCount))
    {
        // Get an api shader type for the corresponding HW Shader
        Pal::ShaderType shaderType = GetApiShaderFromHwShader(static_cast<Util::Abi::HardwareStage>(i), apiToHwShader);

        // Get the shader stats from the shader in the pipeline
        Pal::Result palResult = pPalPipeline->GetShaderStats(shaderType, &palStats, true);

        // Covert to the pal statistics to VkShaderStatisticsInfoAMD
        ConvertShaderInfoStatistics(palStats, &vkShaderStats);

        // Set VkShaderStageFlagBits as an output property
        pProperties[outputCount].stages = vkShaderStats.shaderStageMask;

        // Convert HW Stage to API String Name
        Util::Abi::HardwareStage hwStage    = static_cast<Util::Abi::HardwareStage>(i);
        const char*              pHwStageString = HwStageNames[static_cast<uint32_t>(hwStage)];

        // Build the name and description of the output property
        BuildPipelineNameDescription(
            shaderPreName,
            pHwStageString,
            pProperties[outputCount].name,
            pProperties[outputCount].description,
            hwStage,
            palStats.shaderStageMask);

         // If this is a compute shader, report the workgroup size
         if (vkShaderStats.shaderStageMask & VK_SHADER_STAGE_COMPUTE_BIT)
         {
             pProperties[outputCount].subgroupSize = vkShaderStats.computeWorkGroupSize[0] *
                                                     vkShaderStats.computeWorkGroupSize[1] *
                                                     vkShaderStats.computeWorkGroupSize[2];
         }

         hwStageMask &= ~(1 << i);
         outputCount++;
     }

    // Write out the number of stages written
    *pExecutableCount = outputCount;

    // If the requested number of executables was less than the available number of hw stages, return Incomplete
    return (*pExecutableCount < numHWStages) ? VK_INCOMPLETE : VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineExecutableStatisticsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pStatisticCount,
    VkPipelineExecutableStatisticKHR*           pStatistics)
{
    const Pipeline*                     pPipeline     = Pipeline::BaseObjectFromHandle(pExecutableInfo->pipeline);
    const Pal::IPipeline*               pPalPipeline  = pPipeline->PalPipeline(DefaultDeviceIndex);
    const Util::Abi::ApiHwShaderMapping apiToHwShader = pPalPipeline->ApiHwShaderMapping();

    // If pStatisticCount == nullptr the call to this function is just ment to return the number of statistics
    // for an executable in a pipeline.
    if (pStatistics == nullptr)
    {
        *pStatisticCount = ExecutableStatisticsCount;
        return VK_SUCCESS;
    }

    // Count the number of hardware stages that are used in this pipeline
    uint32_t hwStageMask = 0;
    uint32_t numHWStages = CountNumberOfHWStages(&hwStageMask, apiToHwShader);

    // The executable index should be less than the number of HW Stages.
    VK_ASSERT(pExecutableInfo->executableIndex < numHWStages);

    // Get hwStage for executable index
    Util::Abi::HardwareStage hwStage = GetHwStageForExecutableIndex(pExecutableInfo->executableIndex, hwStageMask);

    // Get an api shader type for the corresponding HW Shader
    Pal::ShaderType shaderType = GetApiShaderFromHwShader(hwStage, apiToHwShader);

    // Get the shader stats for the corresponding API stage
    VkShaderStatisticsInfoAMD  vkShaderStats = {};
    Pal::ShaderStats           palStats      = {};

    Pal::Result palResult = pPalPipeline->GetShaderStats(shaderType, &palStats, true);

    // Return error is the there are now statics for stage.
    if (palResult != Pal::Result::Success)
    {
        return VK_ERROR_UNKNOWN;
    }

    // Convert from PAL to VK statistics
    ConvertShaderInfoStatistics(palStats, &vkShaderStats);

    VkPipelineExecutableStatisticKHR executableStatics[ExecutableStatisticsCount] =
    {
        {VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR, 0, "numUsedVgprs",
         "Number of used VGPRs", VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_UINT64_KHR, {}},
        {VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR, 0, "numUsedSgprs",
         "Number of used SGPRs", VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_UINT64_KHR, {}},
        {VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR, 0, "ldsSizePerLocalWorkGroup",
         "LDS size per local workgroup", VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_UINT64_KHR, {}},
        {VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR, 0, "ldsUsageSizeInBytes",
         "LDS usage size in Bytes", VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_UINT64_KHR, {}},
        {VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR, 0, "scratchMemUsageInBytes",
         "Scratch memory usage in Bytes", VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_UINT64_KHR, {}}
    };

    // Number of used Vgprs
    executableStatics[0].value.u64 = vkShaderStats.resourceUsage.numUsedVgprs;

    // Number of used Sgprs
    executableStatics[1].value.u64 = vkShaderStats.resourceUsage.numUsedSgprs;

    // LDS Size Per Local WorkGroup
    executableStatics[2].value.u64 = vkShaderStats.resourceUsage.ldsSizePerLocalWorkGroup;

    // LDS usage size in Bytes
    executableStatics[3].value.u64 = vkShaderStats.resourceUsage.ldsUsageSizeInBytes;

    // Scratch memory usage in Bytes
    executableStatics[4].value.u64 = vkShaderStats.resourceUsage.scratchMemUsageInBytes;

    // Overwrite the number of written statistics
    *pStatisticCount = Util::Min(*pStatisticCount, static_cast<uint32_t>(ExecutableStatisticsCount));

    // Copy pStatisticCount number of statistics
    memcpy(pStatistics, executableStatics, (sizeof(VkPipelineExecutableStatisticKHR) * (*pStatisticCount)));

    // If the requested number of statistics was less than the available number of statics,
    // return Incomplete
    return ((*pStatisticCount) < ExecutableStatisticsCount) ? VK_INCOMPLETE : VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineExecutableInternalRepresentationsKHR(
    VkDevice                                       device,
    const VkPipelineExecutableInfoKHR*             pExecutableInfo,
    uint32_t*                                      pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations)
{
    const Device*                       pDevice       = ApiDevice::ObjectFromHandle(device);
    const Pipeline*                     pPipeline     = Pipeline::BaseObjectFromHandle(pExecutableInfo->pipeline);
    const Pal::IPipeline*               pPalPipeline  = pPipeline->PalPipeline(DefaultDeviceIndex);
    const Util::Abi::ApiHwShaderMapping apiToHwShader = pPalPipeline->ApiHwShaderMapping();

#if VKI_RAY_TRACING
    if (pPipeline->GetType() == VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR)
    {
        const RayTracingPipeline* pRayTracingPipeline = static_cast<const RayTracingPipeline*>(pPipeline);

        return pRayTracingPipeline->GetPipelineExecutableInternalRepresentations(pExecutableInfo,
                                                                        pInternalRepresentationCount,
                                                                        pInternalRepresentations);
    }
#endif

    // Count the number of hardware stages that are used in this pipeline
    uint32_t hwStageMask = 0;
    uint32_t numHWStages = CountNumberOfHWStages(&hwStageMask, apiToHwShader);

    // Get hwStage for executable index
    Util::Abi::HardwareStage hwStage = GetHwStageForExecutableIndex(pExecutableInfo->executableIndex, hwStageMask);

    // Convert from the HW Shader stage back to the corresponding API Stage
    Pal::ShaderType apiShaderType = GetApiShaderFromHwShader(hwStage, apiToHwShader);

    // Get the shader stats from the shader in the pipeline
    Pal::ShaderStats palStats  = {};
    Pal::Result      palResult = pPalPipeline->GetShaderStats(apiShaderType, &palStats, true);

    // Return (Number of Intermediate Shaders) + Number of HW ISA shaders
    uint32_t numberOfInternalRepresentations = (palStats.palShaderHash.lower > 0) ?
        (Util::CountSetBits(pPipeline->GetAvailableAmdIlSymbol(palStats.shaderStageMask)) + 1) : 0;

    if (pInternalRepresentations == nullptr)
    {
        // +1 for elf binary output
        *pInternalRepresentationCount = numberOfInternalRepresentations + 1;
        return VK_SUCCESS;
    }

    // Output the Intermediate API Shaders
    uint32_t outputCount   = 0;
    uint32_t apiShaderMask = pPipeline->GetAvailableAmdIlSymbol(palStats.shaderStageMask);

    uint32_t i = 0;
    while((Util::BitMaskScanForward(&i, apiShaderMask)) &&
          (outputCount < Util::Min(numberOfInternalRepresentations, *pInternalRepresentationCount)))
    {
         // Build the name and description of the output property for IL
         const char*              pApiString    = ApiStageNames[i];
         Pal::ShaderStageFlagBits palShaderMask = IndexShaderStages[i].palFlagBits;

         BuildPipelineNameDescription(
             shaderPreNameIntermediate,
             pApiString,
             pInternalRepresentations[outputCount].name,
             pInternalRepresentations[outputCount].description,
             static_cast<Util::Abi::HardwareStage>(i),
             static_cast<uint32_t>(palShaderMask));

         // Get the text based IL disassembly of the shader
         pPipeline->GetShaderDisassembly(
             pDevice,
             pPalPipeline,
             Util::Abi::PipelineSymbolType::ShaderAmdIl,
             apiShaderType,
             &(pInternalRepresentations[outputCount].dataSize),
             pInternalRepresentations[outputCount].pData);

         // Mark that the output IL disassembly is text formated
         pInternalRepresentations[outputCount].isText = VK_TRUE;

        apiShaderMask &= ~(1 << i);
        outputCount++;
    }

    // Output the ISA shaders
    if (outputCount < Util::Min(numberOfInternalRepresentations, *pInternalRepresentationCount))
    {
        // Build the name and description of the output property for ISA Shader
        const char* pApiString = HwStageNames[static_cast<uint32_t>(hwStage)];

        BuildPipelineNameDescription(
            shaderPreNameISA,
            pApiString,
            pInternalRepresentations[outputCount].name,
            pInternalRepresentations[outputCount].description,
            static_cast<Util::Abi::HardwareStage>(hwStage),
            palStats.shaderStageMask);

        // Get the text based ISA disassembly of the shader
        pPipeline->GetShaderDisassembly(
            pDevice,
            pPalPipeline,
            Util::Abi::PipelineSymbolType::ShaderDisassembly,
            apiShaderType,
            &(pInternalRepresentations[outputCount].dataSize),
            pInternalRepresentations[outputCount].pData);

        // Mark that the output ISA disassembly is text formated
        pInternalRepresentations[outputCount].isText =
            (pInternalRepresentations[outputCount].dataSize > 0) ? VK_TRUE : VK_FALSE;

        outputCount++;
    }

    // Add 1 for ELF internal representaion and output the elf binary
    numberOfInternalRepresentations++;
    if (*pInternalRepresentationCount == numberOfInternalRepresentations)
    {
        PipelineBinaryInfo binaryInfo = {};
        const char* pApiString = HwStageNames[static_cast<uint32_t>(hwStage)];

        bool hasPipelineBinary = pPipeline->GetBinary(apiShaderType, &binaryInfo);

        if (hasPipelineBinary == true)
        {
            VkResult tempResult = VK_ERROR_UNKNOWN;

            if ((pInternalRepresentations[outputCount].dataSize == 0)
                || (pInternalRepresentations[outputCount].pData == nullptr))
            {
                pInternalRepresentations[outputCount].dataSize = binaryInfo.pipelineBinary.codeSize + 1;
                tempResult = VK_SUCCESS;
            }
            else
            {
                VK_ASSERT(binaryInfo.pipelineBinary.pCode != nullptr);
                memcpy(pInternalRepresentations[outputCount].pData,
                        binaryInfo.pipelineBinary.pCode,
                        Util::Min(pInternalRepresentations[outputCount].dataSize, binaryInfo.pipelineBinary.codeSize));

                if (pInternalRepresentations[outputCount].dataSize > binaryInfo.pipelineBinary.codeSize)
                {
                    char *pBuffer = static_cast<char*>(pInternalRepresentations[outputCount].pData);
                    pBuffer[binaryInfo.pipelineBinary.codeSize] = '\0';
                    tempResult = VK_SUCCESS;
                }
            }

            BuildPipelineNameDescription(
                shaderPreNameELF,
                pApiString,
                pInternalRepresentations[outputCount].name,
                pInternalRepresentations[outputCount].description,
                static_cast<Util::Abi::HardwareStage>(hwStage),
                palStats.shaderStageMask);

            if (tempResult == VK_SUCCESS)
            {
                outputCount++;
            }
        }
        else
        {
            Util::Strncpy(pInternalRepresentations[outputCount].name, "ELF", VK_MAX_DESCRIPTION_SIZE);
            Util::Strncat(pInternalRepresentations[outputCount].name, VK_MAX_DESCRIPTION_SIZE, pApiString);
            Util::Strncpy(pInternalRepresentations[outputCount].description,
                                            "elf shader binary not ready", VK_MAX_DESCRIPTION_SIZE);
        }
    }

    // Write out the number of shader ouputs.
    *pInternalRepresentationCount = outputCount;

    // If the requested number of executables was less than the available number of hw stages, return Incomplete
    return (*pInternalRepresentationCount < numberOfInternalRepresentations) ? VK_INCOMPLETE : VK_SUCCESS;
}

// =====================================================================================================================
VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetPipelineIndirectDeviceAddressNV(
    VkDevice                                        device,
    const VkPipelineIndirectDeviceAddressInfoNV*    pInfo)
{
    VK_NOT_IMPLEMENTED;
    return 0;
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkGetPipelineIndirectMemoryRequirementsNV(
    VkDevice                                        device,
    const VkComputePipelineCreateInfo*              pCreateInfo,
    VkMemoryRequirements2*                          pMemoryRequirements)
{
    VK_NOT_IMPLEMENTED;
}

} // namespace entry

} // namespace vk
