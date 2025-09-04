#pragma once

// Zero-cost abstraction approach using macros to eliminate code duplication
// This generates identical assembly to the original switch statement

// Macro to define feature handler information
#define FEATURE_HANDLER(stype_enum, struct_type, update_call) \
    case stype_enum: { \
        auto* pExtInfo = reinterpret_cast<struct_type*>(pHeader); \
        if (updateFeatures) { \
            update_call; \
        } \
        structSize = sizeof(*pExtInfo); \
        break; \
    }

// Macro for simple boolean assignment handlers
#define SIMPLE_FEATURE_HANDLER(stype_enum, struct_type, ...) \
    case stype_enum: { \
        auto* pExtInfo = reinterpret_cast<struct_type*>(pHeader); \
        if (updateFeatures) { \
            __VA_ARGS__ \
        } \
        structSize = sizeof(*pExtInfo); \
        break; \
    }

// Macro for complex multi-call handlers
#define COMPLEX_FEATURE_HANDLER(stype_enum, struct_type, update_block) \
    case stype_enum: { \
        auto* pExtInfo = reinterpret_cast<struct_type*>(pHeader); \
        if (updateFeatures) { \
            update_block \
        } \
        structSize = sizeof(*pExtInfo); \
        break; \
    }

// Zero-cost implementation that expands to identical assembly
#define IMPLEMENT_GETFEATURES2_SWITCH() \
    switch (static_cast<uint32>(pHeader->sType)) { \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, \
            VkPhysicalDeviceFeatures2, \
            GetFeatures(&pExtInfo->features) \
        ) \
        \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, \
            VkPhysicalDevice16BitStorageFeatures, \
            GetPhysicalDevice16BitStorageFeatures( \
                &pExtInfo->storageBuffer16BitAccess, \
                &pExtInfo->uniformAndStorageBuffer16BitAccess, \
                &pExtInfo->storagePushConstant16, \
                &pExtInfo->storageInputOutput16) \
        ) \
        \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, \
            VkPhysicalDevice8BitStorageFeatures, \
            GetPhysicalDevice8BitStorageFeatures( \
                &pExtInfo->storageBuffer8BitAccess, \
                &pExtInfo->uniformAndStorageBuffer8BitAccess, \
                &pExtInfo->storagePushConstant8) \
        ) \
        \
        SIMPLE_FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GPA_FEATURES_AMD, \
            VkPhysicalDeviceGpaFeaturesAMD, \
            pExtInfo->clockModes = m_gpaProps.features.clockModes; \
            pExtInfo->perfCounters = m_gpaProps.features.perfCounters; \
            pExtInfo->sqThreadTracing = m_gpaProps.features.sqThreadTracing; \
            pExtInfo->streamingPerfCounters = m_gpaProps.features.streamingPerfCounters; \
        ) \
        \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES, \
            VkPhysicalDeviceTimelineSemaphoreFeatures, \
            GetPhysicalDeviceTimelineSemaphoreFeatures(&pExtInfo->timelineSemaphore) \
        ) \
        \
        SIMPLE_FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT, \
            VkPhysicalDeviceDeviceMemoryReportFeaturesEXT, \
            pExtInfo->deviceMemoryReport = VK_TRUE; \
        ) \
        \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES, \
            VkPhysicalDeviceSamplerYcbcrConversionFeatures, \
            GetPhysicalDeviceSamplerYcbcrConversionFeatures(&pExtInfo->samplerYcbcrConversion) \
        ) \
        \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES, \
            VkPhysicalDeviceVariablePointerFeatures, \
            GetPhysicalDeviceVariablePointerFeatures( \
                &pExtInfo->variablePointersStorageBuffer, \
                &pExtInfo->variablePointers) \
        ) \
        \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES, \
            VkPhysicalDeviceProtectedMemoryFeatures, \
            GetPhysicalDeviceProtectedMemoryFeatures(&pExtInfo->protectedMemory) \
        ) \
        \
        FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, \
            VkPhysicalDeviceMultiviewFeatures, \
            GetPhysicalDeviceMultiviewFeatures( \
                &pExtInfo->multiview, \
                &pExtInfo->multiviewGeometryShader, \
                &pExtInfo->multiviewTessellationShader) \
        ) \
        \
        SIMPLE_FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT, \
            VkPhysicalDeviceInlineUniformBlockFeaturesEXT, \
            pExtInfo->inlineUniformBlock = VK_TRUE; \
            pExtInfo->descriptorBindingInlineUniformBlockUpdateAfterBind = VK_TRUE; \
        ) \
        \
        SIMPLE_FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT, \
            VkPhysicalDeviceTransformFeedbackFeaturesEXT, \
            pExtInfo->geometryStreams = VK_TRUE; \
            pExtInfo->transformFeedback = VK_TRUE; \
        ) \
        \
        SIMPLE_FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT, \
            VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT, \
            pExtInfo->shaderDemoteToHelperInvocation = VK_TRUE; \
        ) \
        \
        SIMPLE_FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT, \
            VkPhysicalDeviceDepthClipEnableFeaturesEXT, \
            pExtInfo->depthClipEnable = VK_TRUE; \
        ) \
        \
        COMPLEX_FEATURE_HANDLER( \
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, \
            VkPhysicalDeviceVulkan12Features, \
            GetPhysicalDevice8BitStorageFeatures( \
                &pExtInfo->storageBuffer8BitAccess, \
                &pExtInfo->uniformAndStorageBuffer8BitAccess, \
                &pExtInfo->storagePushConstant8); \
            GetPhysicalDeviceShaderAtomicInt64Features( \
                &pExtInfo->shaderBufferInt64Atomics, \
                &pExtInfo->shaderSharedInt64Atomics); \
            GetPhysicalDeviceFloat16Int8Features(&pExtInfo->shaderFloat16, &pExtInfo->shaderInt8); \
            pExtInfo->samplerMirrorClampToEdge = VK_TRUE; \
            pExtInfo->drawIndirectCount = VK_TRUE; \
        ) \
        \
        default: { \
            break; \
        } \
    }

// Alternative: X-Macro approach for even more compact definition
#define FEATURE_LIST \
    X(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, VkPhysicalDeviceFeatures2, GetFeatures(&pExtInfo->features)) \
    X(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, VkPhysicalDevice16BitStorageFeatures, \
      GetPhysicalDevice16BitStorageFeatures(&pExtInfo->storageBuffer16BitAccess, &pExtInfo->uniformAndStorageBuffer16BitAccess, &pExtInfo->storagePushConstant16, &pExtInfo->storageInputOutput16)) \
    X(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, VkPhysicalDevice8BitStorageFeatures, \
      GetPhysicalDevice8BitStorageFeatures(&pExtInfo->storageBuffer8BitAccess, &pExtInfo->uniformAndStorageBuffer8BitAccess, &pExtInfo->storagePushConstant8))

#define X(stype, struct_type, update_call) \
    case stype: { \
        auto* pExtInfo = reinterpret_cast<struct_type*>(pHeader); \
        if (updateFeatures) { \
            update_call; \
        } \
        structSize = sizeof(*pExtInfo); \
        break; \
    }

#define IMPLEMENT_GETFEATURES2_XMACRO() \
    switch (static_cast<uint32>(pHeader->sType)) { \
        FEATURE_LIST \
        default: break; \
    }

#undef X

// Constexpr validation approach (C++17)
template<uint32_t sType>
struct FeatureTypeInfo;

#define DEFINE_FEATURE_TYPE(stype_val, struct_type) \
    template<> \
    struct FeatureTypeInfo<stype_val> { \
        using type = struct_type; \
        static constexpr uint32_t sType = stype_val; \
        static constexpr size_t size = sizeof(struct_type); \
    };

// Define all known types at compile time
DEFINE_FEATURE_TYPE(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, VkPhysicalDeviceFeatures2)
DEFINE_FEATURE_TYPE(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, VkPhysicalDevice16BitStorageFeatures)
DEFINE_FEATURE_TYPE(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, VkPhysicalDevice8BitStorageFeatures)