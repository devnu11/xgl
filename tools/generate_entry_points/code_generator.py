"""Main code generator for XGL entry points."""

import logging
from pathlib import Path
from typing import Dict, List

from xml_parser import VulkanRegistryParser, Command, CommandType
from template_engine import TemplateEngine
from type_mapper import TypeMapper


class EntryPointGenerator:
    """Generates C++ entry point files from Vulkan XML registry."""
    
    def __init__(self, parser: VulkanRegistryParser, output_dir: Path):
        self.parser = parser
        self.output_dir = output_dir
        self.template_engine = TemplateEngine()
        self.type_mapper = TypeMapper()
        self.logger = logging.getLogger(__name__)
        self.verbose = False
    
    def enable_verbose(self) -> None:
        """Enable verbose logging."""
        self.verbose = True
        logging.basicConfig(level=logging.INFO)
    
    def generate(self) -> None:
        """Generate all entry point files."""
        self._log_info("Starting entry point generation...")
        
        commands = self.parser.get_commands()
        self._log_info(f"Found {len(commands)} commands")
        
        # Filter out NVIDIA/CUDA extensions
        filtered_commands = self._filter_excluded_extensions(commands)
        self._log_info(f"After filtering: {len(filtered_commands)} commands")
        
        # Group commands by type for organized file generation
        command_groups = self._group_commands_by_object_type(filtered_commands)
        
        for object_type, grouped_commands in command_groups.items():
            self._generate_file_for_object_type(object_type, grouped_commands)
        
        self._log_info("Entry point generation complete")
    
    def _filter_excluded_extensions(self, commands: Dict[str, Command]) -> Dict[str, Command]:
        """Filter to only allow core and select vendor extensions."""
        allowed_suffixes = [
            'khr',      # Khronos extensions
            'ext',      # Multi-vendor extensions
            'amd',      # AMD extensions
            'samsung',  # Samsung extensions
            'android',  # Android extensions
        ]
        
        filtered = {}
        for name, command in commands.items():
            func_name_lower = name.lower()
            
            # Always allow functions without vendor suffixes (core functions)
            has_vendor_suffix = any(func_name_lower.endswith(suffix) for suffix in 
                                   ['khr', 'ext', 'amd', 'nv', 'nvx', 'arm', 'qcom', 'samsung', 
                                    'google', 'huawei', 'intel', 'msft', 'qnx', 'sec', 'img', 
                                    'fuchsia', 'ggp', 'nn', 'mvk', 'android', 'win32', 'xcb', 
                                    'xlib', 'wayland', 'directfb', 'ohos'])
            
            if not has_vendor_suffix:
                # Core Vulkan function - always include
                filtered[name] = command
                continue
            
            # Check if function ends with allowed vendor suffix
            if any(func_name_lower.endswith(suffix) for suffix in allowed_suffixes):
                filtered[name] = command
            else:
                self._log_info(f"Filtering out non-allowed extension function: {name}")
                
        return filtered
    
    def _group_commands_by_object_type(self, commands: Dict[str, Command]) -> Dict[str, List[Command]]:
        """Group commands by explicit file mappings, preserving the order specified in mappings."""
        file_mappings = self._get_file_mappings()
        
        groups: Dict[str, List[Command]] = {}
        unmapped_functions = []
        
        # Iterate through file mappings in order to preserve function ordering
        for file_name, func_list in file_mappings.items():
            groups[file_name] = []
            for func_name in func_list:
                if func_name in commands:
                    groups[file_name].append(commands[func_name])
                else:
                    # Function is mapped but not found in commands (filtered out or doesn't exist)
                    pass
        
        # Check for functions in commands that aren't mapped to any file
        mapped_functions = set()
        for func_list in file_mappings.values():
            mapped_functions.update(func_list)
        
        for command in commands.values():
            if command.name not in mapped_functions:
                unmapped_functions.append(command.name)
        
        # Report and ignore unmapped functions (they don't exist in current files)
        if unmapped_functions:
            self._log_info(f"INFO: Ignoring {len(unmapped_functions)} unmapped functions (not in existing entry files):")
            for func in sorted(unmapped_functions):
                self._log_info(f"  - {func}")
            self._log_info("These functions will not be generated unless added to the file mappings.")
        
        return groups
    
    def _get_file_mappings(self) -> Dict[str, List[str]]:
        """Get explicit mapping of file names to lists of function names."""
        return {
            'buffer': 
            [
                'vkDestroyBuffer',
				'vkBindBufferMemory',
				'vkGetBufferMemoryRequirements',
				'vkGetBufferMemoryRequirements2',
				'vkGetBufferDeviceAddress',
				'vkGetBufferOpaqueCaptureAddress'
            ],
            'buffer_view': 
			[
                'vkDestroyBufferView'
            ],
            'cmd_buffer': 
            [
                'vkBeginCommandBuffer',
				'vkEndCommandBuffer',
				'vkResetCommandBuffer',
				'vkCmdBindPipeline',
				'vkCmdBindDescriptorSets',
				'vkCmdBindIndexBuffer',
				'vkCmdBindIndexBuffer2',
				'vkCmdBindDescriptorSets2',
				'vkCmdPushConstants2',
				'vkCmdPushDescriptorSet2',
				'vkCmdPushDescriptorSetWithTemplate2',
				'vkCmdSetDescriptorBufferOffsets2EXT',
				'vkCmdBindDescriptorBufferEmbeddedSamplers2EXT',
				'vkCmdBindVertexBuffers',
				'vkCmdDraw',
				'vkCmdDrawIndexed',
				'vkCmdDrawIndirect',
				'vkCmdDrawIndexedIndirect',
				'vkCmdDrawIndirectCount',
				'vkCmdDrawIndexedIndirectCount',
				'vkCmdDrawMeshTasksEXT',
				'vkCmdDrawMeshTasksIndirectEXT',
				'vkCmdDrawMeshTasksIndirectCountEXT',
				'vkCmdDispatch',
				'vkCmdDispatchIndirect',
				'vkCmdPreprocessGeneratedCommandsNV',
				'vkCmdExecuteGeneratedCommandsNV',
				'vkCmdBindPipelineShaderGroupNV',
				'vkCmdUpdatePipelineIndirectBufferNV',
				'vkCmdPreprocessGeneratedCommandsEXT',
				'vkCmdExecuteGeneratedCommandsEXT',
				'vkCmdCopyBuffer',
				'vkCmdCopyImage',
				'vkCmdBlitImage',
				'vkCmdCopyBufferToImage',
				'vkCmdCopyImageToBuffer',
				'vkCmdUpdateBuffer',
				'vkCmdFillBuffer',
				'vkCmdClearColorImage',
				'vkCmdClearDepthStencilImage',
				'vkCmdClearAttachments',
				'vkCmdResolveImage',
				'vkCmdSetEvent',
				'vkCmdResetEvent',
				'vkCmdWaitEvents',
				'vkCmdPipelineBarrier',
				'vkCmdBeginQuery',
				'vkCmdEndQuery',
				'vkCmdResetQueryPool',
				'vkCmdWriteTimestamp',
				'vkCmdCopyQueryPoolResults',
				'vkCmdPushConstants',
				'vkCmdBeginRenderPass',
				'vkCmdBeginRenderPass2',
				'vkCmdNextSubpass',
				'vkCmdNextSubpass2',
				'vkCmdEndRenderPass',
				'vkCmdEndRenderPass2',
				'vkCmdExecuteCommands',
				'vkFreeCommandBuffers',
				'vkCmdDispatchBase',
				'vkCmdSetDeviceMask',
				'vkCmdSetViewport',
				'vkCmdSetScissor',
				'vkCmdSetLineWidth',
				'vkCmdSetDepthBias',
				'vkCmdSetBlendConstants',
				'vkCmdSetDepthBounds',
				'vkCmdSetStencilCompareMask',
				'vkCmdSetStencilWriteMask',
				'vkCmdSetStencilReference',
				'vkCmdDebugMarkerBeginEXT',
				'vkCmdDebugMarkerEndEXT',
				'vkCmdDebugMarkerInsertEXT',
				'vkCmdBeginDebugUtilsLabelEXT',
				'vkCmdEndDebugUtilsLabelEXT',
				'vkCmdInsertDebugUtilsLabelEXT',
				'vkCmdSetSampleLocationsEXT',
				'vkCmdWriteBufferMarkerAMD',
				'vkCmdBindTransformFeedbackBuffersEXT',
				'vkCmdBeginTransformFeedbackEXT',
				'vkCmdEndTransformFeedbackEXT',
				'vkCmdBeginQueryIndexedEXT',
				'vkCmdEndQueryIndexedEXT',
				'vkCmdDrawIndirectByteCountEXT',
				'vkCmdBuildAccelerationStructuresKHR',
				'vkCmdBuildAccelerationStructuresIndirectKHR',
				'vkCmdTraceRaysKHR',
				'vkCmdTraceRaysIndirectKHR',
				'vkCmdCopyAccelerationStructureKHR',
				'vkCmdWriteAccelerationStructuresPropertiesKHR',
				'vkCmdCopyAccelerationStructureToMemoryKHR',
				'vkCmdCopyMemoryToAccelerationStructureKHR',
				'vkCmdSetRayTracingPipelineStackSizeKHR',
				'vkCmdTraceRaysIndirect2KHR',
				'vkCmdBuildMicromapsEXT',
				'vkCmdCopyMemoryToMicromapEXT',
				'vkCmdCopyMicromapEXT',
				'vkCmdCopyMicromapToMemoryEXT',
				'vkCmdWriteMicromapsPropertiesEXT',
				'vkCmdSetLineStipple',
				'vkCmdSetFragmentShadingRateKHR',
				'vkCmdBeginConditionalRenderingEXT',
				'vkCmdEndConditionalRenderingEXT',
				'vkCmdSetEvent2',
				'vkCmdResetEvent2',
				'vkCmdWaitEvents2',
				'vkCmdPipelineBarrier2',
				'vkCmdWriteTimestamp2',
				'vkCmdWriteBufferMarker2AMD',
				'vkCmdBeginRendering',
				'vkCmdEndRendering',
				'vkCmdSetCullMode',
				'vkCmdSetFrontFace',
				'vkCmdSetPrimitiveTopology',
				'vkCmdSetViewportWithCount',
				'vkCmdSetScissorWithCount',
				'vkCmdBindVertexBuffers2',
				'vkCmdSetDepthTestEnable',
				'vkCmdSetDepthWriteEnable',
				'vkCmdSetDepthCompareOp',
				'vkCmdSetDepthBoundsTestEnable',
				'vkCmdSetStencilTestEnable',
				'vkCmdSetStencilOp',
				'vkCmdBindDescriptorBuffersEXT',
				'vkCmdSetDescriptorBufferOffsetsEXT',
				'vkCmdBindDescriptorBufferEmbeddedSamplersEXT',
				'vkCmdSetColorWriteEnableEXT',
				'vkCmdSetRasterizerDiscardEnable',
				'vkCmdSetPrimitiveRestartEnable',
				'vkCmdSetDepthBiasEnable',
				'vkCmdSetLogicOpEXT',
				'vkCmdSetPatchControlPointsEXT',
				'vkCmdSetDepthClampRangeEXT',
				'vkCmdBlitImage2',
				'vkCmdCopyBuffer2',
				'vkCmdCopyBufferToImage2',
				'vkCmdCopyImage2',
				'vkCmdCopyImageToBuffer2',
				'vkCmdResolveImage2',
				'vkCmdPushDescriptorSet',
				'vkCmdPushDescriptorSetWithTemplate',
				'vkCmdSetTessellationDomainOriginEXT',
				'vkCmdSetDepthClampEnableEXT',
				'vkCmdSetPolygonModeEXT',
				'vkCmdSetRasterizationSamplesEXT',
				'vkCmdSetSampleMaskEXT',
				'vkCmdSetAlphaToCoverageEnableEXT',
				'vkCmdSetAlphaToOneEnableEXT',
				'vkCmdSetLogicOpEnableEXT',
				'vkCmdSetColorBlendEnableEXT',
				'vkCmdSetColorBlendEquationEXT',
				'vkCmdSetColorWriteMaskEXT',
				'vkCmdSetRasterizationStreamEXT',
				'vkCmdSetConservativeRasterizationModeEXT',
				'vkCmdSetExtraPrimitiveOverestimationSizeEXT',
				'vkCmdSetDepthClipEnableEXT',
				'vkCmdSetSampleLocationsEnableEXT',
				'vkCmdSetColorBlendAdvancedEXT',
				'vkCmdSetProvokingVertexModeEXT',
				'vkCmdSetLineRasterizationModeEXT',
				'vkCmdSetLineStippleEnableEXT',
				'vkCmdSetDepthClipNegativeOneToOneEXT',
				'vkCmdSetVertexInputEXT',
				'vkCmdSetRenderingAttachmentLocations',
				'vkCmdSetRenderingInputAttachmentIndices',
				'vkCmdSetDepthBias2EXT'
            ],
            'cmd_pool': 
            [
                'vkDestroyCommandPool',
				'vkResetCommandPool',
				'vkTrimCommandPool'
            ],
            'debug_report': 
            [
                'vkCreateDebugReportCallbackEXT',
				'vkDestroyDebugReportCallbackEXT',
				'vkDebugReportMessageEXT'
            ],
            'debug_utils': 
            [
                'vkCreateDebugUtilsMessengerEXT',
				'vkDestroyDebugUtilsMessengerEXT',
				'vkSubmitDebugUtilsMessageEXT'
            ],
            'deferred_operation': 
            [
                'vkDestroyDeferredOperationKHR',
				'vkGetDeferredOperationResultKHR',
				'vkGetDeferredOperationMaxConcurrencyKHR',
				'vkDeferredOperationJoinKHR'
            ],
            'descriptor_buffer': 
            [
                'vkGetDescriptorSetLayoutSizeEXT',
				'vkGetDescriptorSetLayoutBindingOffsetEXT',
				'vkGetDescriptorEXT',
				'vkGetBufferOpaqueCaptureDescriptorDataEXT',
				'vkGetImageOpaqueCaptureDescriptorDataEXT',
				'vkGetImageViewOpaqueCaptureDescriptorDataEXT',
				'vkGetSamplerOpaqueCaptureDescriptorDataEXT',
				'vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT'
            ],
            'descriptor_pool': 
            [
                'vkCreateDescriptorPool',
				'vkFreeDescriptorSets',
				'vkResetDescriptorPool',
				'vkDestroyDescriptorPool',
				'vkAllocateDescriptorSets'
            ],
            'descriptor_set': 
            [
                'vkUpdateDescriptorSets'
            ],
            'descriptor_set_layout': 
            [
                'vkDestroyDescriptorSetLayout'
            ],
            'descriptor_update_template': 
            [
                'vkDestroyDescriptorUpdateTemplate', 'vkUpdateDescriptorSetWithTemplate'
            ],
            'device': 
            [
                'vkCreateFence', 'vkWaitForFences', 'vkResetFences', 'vkGetDeviceQueue', 'vkGetDeviceQueue2', 'vkCreateSemaphore', 'vkDestroyDevice', 'vkDeviceWaitIdle', 'vkCreateEvent', 'vkCreateQueryPool', 'vkCreateDescriptorSetLayout', 'vkCreatePipelineLayout', 'vkCreateFramebuffer', 'vkCreateRenderPass', 'vkCreateRenderPass2', 'vkCreateBuffer', 'vkCreateBufferView', 'vkCreateImage', 'vkCreateImageView', 'vkCreateShaderModule', 'vkCreatePipelineCache', 'vkCreateGraphicsPipelines', 'vkCreateComputePipelines', 'vkCreateSampler', 'vkCreateSamplerYcbcrConversion', 'vkCreateSwapchainKHR', 'vkGetRenderAreaGranularity', 'vkGetRenderingAreaGranularity', 'vkAllocateCommandBuffers', 'vkCreateCommandPool', 'vkAllocateMemory', 'vkImportSemaphoreFdKHR', 'vkBindBufferMemory2', 'vkBindImageMemory2', 'vkCreateDescriptorUpdateTemplate', 'vkGetDeviceGroupPeerMemoryFeatures', 'vkGetDeviceGroupPresentCapabilitiesKHR', 'vkGetDeviceGroupSurfacePresentModesKHR', 'vkDebugMarkerSetObjectTagEXT', 'vkDebugMarkerSetObjectNameEXT', 'vkSetDebugUtilsObjectTagEXT', 'vkSetDebugUtilsObjectNameEXT', 'vkSetGpaDeviceClockModeAMD', 'vkGetGpaDeviceClockInfoAMD', 'vkGetDescriptorSetLayoutSupport', 'vkGetCalibratedTimestampsEXT', 'vkGetSemaphoreCounterValue', 'vkWaitSemaphores', 'vkSignalSemaphore', 'vkGetMemoryHostPointerPropertiesEXT', 'vkCreateAccelerationStructureKHR', 'vkCreateRayTracingPipelinesKHR', 'vkBuildAccelerationStructuresKHR', 'vkCopyAccelerationStructureKHR', 'vkCopyAccelerationStructureToMemoryKHR', 'vkCopyMemoryToAccelerationStructureKHR', 'vkWriteAccelerationStructuresPropertiesKHR', 'vkGetRayTracingCaptureReplayShaderGroupHandlesKHR', 'vkGetDeviceAccelerationStructureCompatibilityKHR', 'vkGetAccelerationStructureBuildSizesKHR', 'vkCreateDeferredOperationKHR', 'vkGetMicromapBuildSizesEXT', 'vkCreateMicromapEXT', 'vkCopyMemoryToMicromapEXT', 'vkCopyMicromapToMemoryEXT', 'vkBuildMicromapsEXT', 'vkCopyMicromapEXT', 'vkDestroyMicromapEXT', 'vkWriteMicromapsPropertiesEXT', 'vkGetDeviceMicromapCompatibilityEXT', 'vkGetDeviceBufferMemoryRequirements', 'vkGetDeviceImageMemoryRequirements', 'vkGetDeviceImageSparseMemoryRequirements', 'vkSetDeviceMemoryPriorityEXT', 'vkGetDeviceFaultInfoEXT', 'vkCreatePipelineBinariesKHR', 'vkDestroyPipelineBinaryKHR', 'vkGetPipelineKeyKHR', 'vkGetPipelineBinaryDataKHR', 'vkReleaseCapturedPipelineDataKHR', 'vkGetDeviceImageSubresourceLayout', 'vkGetImageSubresourceLayout2', 'vkGetGeneratedCommandsMemoryRequirementsNV', 'vkCreateIndirectCommandsLayoutNV', 'vkDestroyIndirectCommandsLayoutNV', 'vkCreateIndirectCommandsLayoutEXT', 'vkCreateIndirectExecutionSetEXT', 'vkDestroyIndirectCommandsLayoutEXT', 'vkDestroyIndirectExecutionSetEXT', 'vkGetGeneratedCommandsMemoryRequirementsEXT', 'vkUpdateIndirectExecutionSetPipelineEXT', 'vkUpdateIndirectExecutionSetShaderEXT'
            ],
            'dispatch': 
            [
                'vkGetInstanceProcAddr', 'vkGetPhysicalDeviceProcAddr', 'vkGetDeviceProcAddr'
            ],
            'event': 
            [
                'vkDestroyEvent', 'vkGetEventStatus', 'vkSetEvent', 'vkResetEvent'
            ],
            'fence': 
            [
                'vkGetFenceStatus', 'vkDestroyFence', 'vkImportFenceFdKHR', 'vkGetFenceFdKHR'
            ],
            'framebuffer': 
            [
                'vkDestroyFramebuffer'
            ],
            'gpa_session': 
            [
                'vkCreateGpaSessionAMD', 'vkDestroyGpaSessionAMD', 'vkCmdBeginGpaSessionAMD', 'vkCmdEndGpaSessionAMD', 'vkCmdBeginGpaSampleAMD', 'vkCmdEndGpaSampleAMD', 'vkGetGpaSessionStatusAMD', 'vkGetGpaSessionResultsAMD', 'vkResetGpaSessionAMD', 'vkCmdCopyGpaSessionResultsAMD'
            ],
            'image': 
            [
                'vkDestroyImage', 'vkBindImageMemory', 'vkGetImageMemoryRequirements', 'vkGetImageSparseMemoryRequirements', 'vkGetImageSubresourceLayout', 'vkGetImageMemoryRequirements2', 'vkGetImageSparseMemoryRequirements2', 'vkGetImageDrmFormatModifierPropertiesEXT', 'vkCopyImageToImage', 'vkCopyImageToMemory', 'vkCopyMemoryToImage', 'vkTransitionImageLayout'
            ],
            'image_view': 
            [
                'vkDestroyImageView'
            ],
            'instance': 
            [
                'vkEnumerateInstanceVersion', 'vkCreateInstance', 'vkDestroyInstance', 'vkEnumeratePhysicalDevices', 'vkEnumeratePhysicalDeviceGroups', 'vkEnumerateInstanceExtensionProperties', 'vkEnumerateInstanceLayerProperties'
            ],
            'memory': 
            [
                'vkFreeMemory', 'vkMapMemory', 'vkUnmapMemory', 'vkMapMemory2', 'vkUnmapMemory2', 'vkFlushMappedMemoryRanges', 'vkInvalidateMappedMemoryRanges', 'vkGetDeviceMemoryCommitment', 'vkGetMemoryFdKHR', 'vkGetMemoryFdPropertiesKHR', 'vkGetDeviceMemoryOpaqueCaptureAddress'
            ],
            'physical_device': 
            [
                'vkCreateDevice', 'vkEnumerateDeviceExtensionProperties', 'vkGetPhysicalDeviceFeatures', 'vkGetPhysicalDeviceProperties', 'vkGetPhysicalDeviceImageFormatProperties', 'vkGetPhysicalDeviceFormatProperties', 'vkEnumerateDeviceLayerProperties', 'vkGetPhysicalDeviceMemoryProperties', 'vkGetPhysicalDeviceQueueFamilyProperties', 'vkGetPhysicalDeviceSparseImageFormatProperties', 'vkGetPhysicalDeviceSurfaceSupportKHR', 'vkGetPhysicalDeviceSurfacePresentModesKHR', 'vkGetPhysicalDeviceSurfaceCapabilitiesKHR', 'vkGetPhysicalDeviceSurfaceCapabilities2KHR', 'vkGetPhysicalDeviceSurfaceFormatsKHR', 'vkGetPhysicalDeviceSurfaceFormats2KHR', 'vkGetPhysicalDeviceFeatures2', 'vkGetPhysicalDeviceProperties2', 'vkGetPhysicalDeviceFormatProperties2', 'vkGetPhysicalDeviceImageFormatProperties2', 'vkGetPhysicalDeviceMultisamplePropertiesEXT', 'vkGetPhysicalDeviceQueueFamilyProperties2', 'vkGetPhysicalDeviceMemoryProperties2', 'vkGetPhysicalDeviceSparseImageFormatProperties2', 'vkGetPhysicalDeviceExternalBufferProperties', 'vkGetPhysicalDeviceExternalSemaphoreProperties', 'vkGetPhysicalDeviceExternalFenceProperties', 'vkGetPhysicalDeviceXcbPresentationSupportKHR', 'vkGetPhysicalDeviceXlibPresentationSupportKHR', 'vkGetPhysicalDeviceWaylandPresentationSupportKHR', 'vkAcquireXlibDisplayEXT', 'vkGetRandROutputDisplayEXT', 'vkReleaseDisplayEXT', 'vkGetPhysicalDevicePresentRectanglesKHR', 'vkGetPhysicalDeviceDisplayPropertiesKHR', 'vkGetPhysicalDeviceDisplayPlanePropertiesKHR', 'vkGetDisplayPlaneSupportedDisplaysKHR', 'vkGetDisplayModePropertiesKHR', 'vkCreateDisplayModeKHR', 'vkGetDisplayPlaneCapabilitiesKHR', 'vkGetPhysicalDeviceDisplayProperties2KHR', 'vkGetPhysicalDeviceDisplayPlaneProperties2KHR', 'vkGetDisplayModeProperties2KHR', 'vkGetDisplayPlaneCapabilities2KHR', 'vkGetPhysicalDeviceSurfaceCapabilities2EXT', 'vkGetPhysicalDeviceCalibrateableTimeDomainsEXT', 'vkGetPhysicalDeviceToolProperties', 'vkGetPhysicalDeviceFragmentShadingRatesKHR', 'vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR'
            ],
            'pipeline': 
            [
                'vkDestroyPipeline', 'vkGetShaderInfoAMD', 'vkGetPipelineExecutablePropertiesKHR', 'vkGetPipelineExecutableStatisticsKHR', 'vkGetPipelineExecutableInternalRepresentationsKHR', 'vkGetPipelineIndirectDeviceAddressNV', 'vkGetPipelineIndirectMemoryRequirementsNV'
            ],
            'pipeline_cache': 
            [
                'vkDestroyPipelineCache', 'vkGetPipelineCacheData', 'vkMergePipelineCaches'
            ],
            'pipeline_layout': 
            [
                'vkDestroyPipelineLayout'
            ],
            'private_data_slot': 
            [
                'vkCreatePrivateDataSlot', 'vkDestroyPrivateDataSlot', 'vkSetPrivateData', 'vkGetPrivateData'
            ],
            'query': 
            [
                'vkGetQueryPoolResults', 'vkDestroyQueryPool', 'vkResetQueryPool'
            ],
            'queue': 
            [
                'vkQueueSubmit', 'vkQueueSubmit2', 'vkQueueWaitIdle', 'vkQueueBindSparse', 'vkQueuePresentKHR', 'vkQueueBeginDebugUtilsLabelEXT', 'vkQueueEndDebugUtilsLabelEXT', 'vkQueueInsertDebugUtilsLabelEXT'
            ],
            'render_pass': 
            [
                'vkDestroyRenderPass'
            ],
            'sampler': 
            [
                'vkDestroySampler'
            ],
            'sampler_ycbcr_conversion': 
            [
                'vkDestroySamplerYcbcrConversion'
            ],
            'semaphore': 
            [
                'vkDestroySemaphore', 'vkGetSemaphoreFdKHR'
            ],
            'shader': 
            [
                'vkDestroyShaderModule', 'vkGetShaderModuleIdentifierEXT', 'vkGetShaderModuleCreateInfoIdentifierEXT'
            ],
            'surface': 
            [
                'vkCreateXcbSurfaceKHR', 'vkCreateXlibSurfaceKHR', 'vkCreateWaylandSurfaceKHR', 'vkCreateDisplayPlaneSurfaceKHR', 'vkDestroySurfaceKHR'
            ],
            'swapchain': 
            [
                'vkDestroySwapchainKHR', 'vkGetSwapchainImagesKHR', 'vkAcquireNextImageKHR', 'vkAcquireNextImage2KHR', 'vkSetHdrMetadataEXT'
            ],
        }
    
    def _get_object_type_from_command(self, command: Command) -> str:
        """Determine object type from command based on function name patterns."""
        func_name = command.name.lower()
        
        # First priority: Handle destroy functions - categorize by object being destroyed
        if 'destroy' in func_name and len(command.parameters) >= 2:
            # For destroy functions, use the second parameter (the object being destroyed)
            destroy_param = command.parameters[1]  # Skip device parameter
            if hasattr(destroy_param, 'type_name'):
                xgl_type = self.type_mapper.get_xgl_type(destroy_param.type_name)
                return self._pascal_to_snake_case(xgl_type)
        
        # Second priority: Use first handle parameter (systematic approach)
        first_param = command.get_first_handle_param()
        if first_param:
            xgl_type = self.type_mapper.get_xgl_type(first_param.type_name)
            object_type = self._pascal_to_snake_case(xgl_type)
            
            # Special case overrides for known patterns that need different grouping
            special_cases = {
                'cmd_buffer': ['allocatecommandbuffers', 'begincommandbuffer', 'endcommandbuffer', 'resetcommandbuffer'],
                'gpa_session': ['gpasession'],
                'descriptor_set_layout': ['descriptorsetlayout'],
            }
            
            # Check if this function matches any special case patterns
            for special_type, patterns in special_cases.items():
                if any(pattern in func_name for pattern in patterns):
                    return special_type
            
            return object_type
            
        # Third priority: Pattern matching for functions without handle parameters
        pattern_mappings = {
            'instance': ['instance', 'enumerate'],
            'dispatch': ['getprocaddr'],
        }
        
        for object_type, patterns in pattern_mappings.items():
            if any(pattern in func_name for pattern in patterns):
                return object_type
            
        # Final fallback 
        return 'device'
    
    def _pascal_to_snake_case(self, pascal_str: str) -> str:
        """Convert PascalCase to snake_case."""
        import re
        
        # Strip 'Vk' prefix if present (for unmapped Vulkan types)
        if pascal_str.startswith('Vk'):
            pascal_str = pascal_str[2:]
        
        # Handle common extension suffixes as single words
        extension_suffixes = ['KHR', 'EXT', 'AMD', 'SAMSUNG']
        for suffix in extension_suffixes:
            if pascal_str.endswith(suffix):
                base = pascal_str[:-len(suffix)]
                # Convert base to snake_case and append suffix
                base_snake = re.sub(r'(?<!^)([A-Z])', r'_\1', base).lower()
                return f"{base_snake}_{suffix.lower()}"
        
        # Insert underscores before capital letters (except the first one)
        snake_str = re.sub(r'(?<!^)([A-Z])', r'_\1', pascal_str)
        
        # Convert to lowercase
        return snake_str.lower()
    
    def _generate_file_for_object_type(self, object_type: str, commands: List[Command]) -> None:
        """Generate entry point file for specific object type."""
        filename = f"entry_vk_{object_type}.cpp"
        filepath = self.output_dir / filename
        
        self._log_info(f"Generating {filename} with {len(commands)} functions")
        
        includes = self._generate_includes(commands)
        functions = self._generate_functions(commands)
        
        content = self.template_engine.render_file_header(
            filename=filename,
            object_type=object_type,
            includes=includes
        )
        
        # Replace placeholder with actual functions
        content = content.replace('$functions', functions)
        
        self._write_file(filepath, content)
    
    def _generate_includes(self, commands: List[Command]) -> str:
        """Generate include statements for commands."""
        return self.type_mapper.get_required_includes(commands)
    
    def _generate_functions(self, commands: List[Command]) -> str:
        """Generate all functions for given commands."""
        functions = []
        
        for command in commands:
            function_code = self._generate_single_function(command)
            functions.append(function_code)
        
        return '\n\n'.join(functions)
    
    def _generate_single_function(self, command: Command) -> str:
        """Generate code for a single entry point function."""
        context = self._build_function_context(command)
        return self.template_engine.render_entry_function(context)
    
    def _build_function_context(self, command: Command) -> Dict[str, str]:
        """Build template context for function generation."""
        first_handle = command.get_first_handle_param()
        method_name = self.type_mapper.get_method_name(command.name)
        
        # Build basic context
        # For global functions (no handle), don't skip first parameter
        skip_first_param = first_handle is not None
        
        context = {
            'function_name': command.name,
            'return_type': command.return_type,
            'parameters': self.type_mapper.format_parameter_list(command.parameters),
            'method_name': method_name,
            'method_params': self.type_mapper.format_method_parameters(command.parameters, skip_first=skip_first_param)
        }
        
        # Add destroy function specific context
        is_destroy_function = command.name.startswith('vkDestroy') and len(command.parameters) >= 2
        if is_destroy_function:
            context.update(self._build_destroy_context(command))
        
        # Add function body (skip handle context for destroy functions as destroy context is more specific)
        if first_handle and not is_destroy_function:
            context.update(self._build_handle_context(command, first_handle))
        
        # Add allocator logic for functions with allocator parameters (but not destroy functions or global functions)
        if not is_destroy_function and first_handle:
            allocator_param = self._find_allocator_parameter(command)
            if allocator_param:
                context['allocator_logic'] = self.template_engine.render_allocator_logic(allocator_param.name)
                # Replace allocator parameter with pAllocCB in method parameters
                context['method_params'] = self._format_method_parameters_with_allocator(command.parameters, allocator_param.name, skip_first=skip_first_param)
            else:
                context['allocator_logic'] = ''
        elif not is_destroy_function:
            # Global functions - no allocator logic needed
            context['allocator_logic'] = ''
        
        function_body = self.template_engine.render_function_body(context)
        context['function_body'] = function_body
        
        return context
    
    def _build_destroy_context(self, command: Command) -> Dict[str, str]:
        """Build context for destroy functions."""
        if len(command.parameters) == 2:
            # 2-parameter destroy functions: (handle, allocator) - handle destroys itself
            # Examples: vkDestroyInstance, vkDestroyDevice
            handle_param = command.parameters[0]  # The object being destroyed
            allocator_param = command.parameters[1]  # Allocator
            
            context = {
                'device_param': handle_param.name,  # Use the handle as device param for these functions
                'handle_param': handle_param.name,
                'allocator_param': allocator_param.name,
                'object_type': self.type_mapper.get_xgl_type(handle_param.type_name)
            }
        else:
            # 3-parameter destroy functions: (device, handle, allocator)
            device_param = command.parameters[0]  # First param is usually device
            handle_param = command.parameters[1]  # Second param is the object being destroyed
            allocator_param = command.parameters[2]  # Third param is allocator
            
            context = {
                'device_param': device_param.name,
                'handle_param': handle_param.name,
                'allocator_param': allocator_param.name,
                'object_type': self.type_mapper.get_xgl_type(handle_param.type_name)
            }
        
        return context
    
    def _find_allocator_parameter(self, command: Command):
        """Find the allocator callback parameter in a command."""
        for param in command.parameters:
            if 'VkAllocationCallbacks' in param.type_name and param.is_pointer:
                return param
        return None
    
    def _format_method_parameters_with_allocator(self, parameters, allocator_param_name: str, skip_first: bool = True) -> str:
        """Format method parameters, replacing allocator parameter with pAllocCB."""
        # Skip first parameter (handle) for non-global functions and replace allocator with pAllocCB
        params_to_use = parameters[1:] if skip_first else parameters
        param_names = []
        for param in params_to_use:
            if param.name == allocator_param_name:
                param_names.append('pAllocCB')
            else:
                param_names.append(param.name)
        return ", ".join(param_names)
    
    def _build_handle_context(self, command: Command, handle_param) -> Dict[str, str]:
        """Build context for handle-based functions."""
        context = {
            'handle_param': handle_param.name,
            'object_type': self.type_mapper.get_xgl_type(handle_param.type_name)
        }
        
        # Determine function type for body template selection
        if handle_param.type_name == 'VkDevice':
            context['function_type'] = 'device'
        elif handle_param.type_name == 'VkInstance':
            context['function_type'] = 'instance'
        else:
            context['function_type'] = 'simple'
        
        # Add allocator logic if needed
        if self.type_mapper.needs_allocator_logic(command.name):
            allocator_param = self.type_mapper.get_allocator_parameter(command.parameters)
            if allocator_param:
                context['allocator_logic'] = self.template_engine.render_allocator_logic(allocator_param)
                # Update method params to use pAllocCB  
                original_params = self.type_mapper.format_method_parameters(command.parameters)
                context['method_params'] = original_params.replace(allocator_param, 'pAllocCB')
            else:
                context['allocator_logic'] = ''
        else:
            context['allocator_logic'] = ''
        
        return context
    
    def _write_file(self, filepath: Path, content: str) -> None:
        """Write generated content to file."""
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        
        self._log_info(f"Generated: {filepath}")
    
    def _log_info(self, message: str) -> None:
        """Log info message if verbose enabled."""
        if self.verbose:
            self.logger.info(message)