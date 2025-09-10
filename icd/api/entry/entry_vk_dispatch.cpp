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
 * @file  entry_vk_dispatch.cpp
 * @brief Entry point functions extracted from vk_dispatch.cpp
 */

#include "include/khronos/vulkan.h"
#include "include/khronos/vk_icd.h"
#include "include/vk_buffer.h"
#include "include/vk_buffer_view.h"
#include "include/vk_cmdbuffer.h"
#include "include/vk_descriptor_buffer.h"
#include "include/vk_descriptor_pool.h"
#include "include/vk_descriptor_set.h"
#include "include/vk_descriptor_set_layout.h"
#include "include/vk_descriptor_update_template.h"
#include "include/vk_device.h"
#include "include/vk_dispatch.h"
#include "include/vk_event.h"
#include "include/vk_extensions.h"
#include "include/vk_fence.h"
#include "include/vk_framebuffer.h"
#include "include/vk_gpa_session.h"
#include "include/vk_image.h"
#include "include/vk_image_view.h"
#include "include/vk_instance.h"
#include "include/vk_memory.h"
#include "include/vk_physical_device.h"
#include "include/vk_pipeline.h"
#include "include/vk_pipeline_cache.h"
#include "include/vk_query.h"
#include "include/vk_queue.h"
#include "include/vk_render_pass.h"
#include "include/vk_sampler.h"
#include "include/vk_sampler_ycbcr_conversion.h"
#include "include/vk_semaphore.h"
#include "include/vk_shader.h"
#include "include/vk_surface.h"
#include "include/vk_swapchain.h"
#include "include/vk_debug_report.h"
#include <cstring>

namespace vk
{

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(
    VkInstance                                  instance,
    const char*                                 pName)
{
    if (instance == VK_NULL_HANDLE)
    {
        return g_GlobalDispatchTable.GetEntryPoint(pName);
    }
    else
    {
        return Instance::ObjectFromHandle(instance)->GetDispatchTable().GetEntryPoint(pName);
    }
}

// =====================================================================================================================
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetPhysicalDeviceProcAddr(
    VkInstance                                  instance,
    const char*                                 pName)
{
    return Instance::ObjectFromHandle(instance)->GetDispatchTable().GetPhysicalDeviceEntryPoint(pName);
}

// =====================================================================================================================
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(
    VkDevice                                    device,
    const char*                                 pName)
{
    return ApiDevice::ObjectFromHandle(device)->GetDispatchTable().GetEntryPoint(pName);
}

} // namespace entry

} // namespace vk
