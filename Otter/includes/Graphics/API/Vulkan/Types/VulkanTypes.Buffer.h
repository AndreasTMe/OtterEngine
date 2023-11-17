#ifndef OTTERENGINE_VULKANTYPES_BUFFER_H
#define OTTERENGINE_VULKANTYPES_BUFFER_H

#include "Graphics/API/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanBuffer
    {
        VkBuffer             Handle             = VK_NULL_HANDLE;
        VkDeviceMemory       DeviceMemory       = VK_NULL_HANDLE;
        VkMemoryRequirements MemoryRequirements = { };
        UInt64               Size               = 0;
        UInt32               BufferUsage        = 0;
        UInt32               MemoryProperties   = 0;
    };
}

#endif //OTTERENGINE_VULKANTYPES_BUFFER_H
