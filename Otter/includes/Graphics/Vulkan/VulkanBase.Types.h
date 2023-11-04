#ifndef OTTERENGINE_VULKANBASE_TYPES_H
#define OTTERENGINE_VULKANBASE_TYPES_H

#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanContext
    {
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkAllocationCallbacks* m_Allocator = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

#if !OTR_RUNTIME
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
#endif
    };
}

#endif //OTTERENGINE_VULKANBASE_TYPES_H
