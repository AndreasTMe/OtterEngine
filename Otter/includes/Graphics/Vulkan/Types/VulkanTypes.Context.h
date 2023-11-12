#ifndef OTTERENGINE_VULKANTYPES_CONTEXT_H
#define OTTERENGINE_VULKANTYPES_CONTEXT_H

#include "Graphics/Vulkan/Types/VulkanTypes.Device.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Swapchain.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanContext
    {
        VkInstance Instance = VK_NULL_HANDLE;
        VkAllocationCallbacks* Allocator = VK_NULL_HANDLE;
        VkSurfaceKHR Surface = VK_NULL_HANDLE;

#if !OTR_RUNTIME
        VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
#endif

        VulkanDevicePair DevicePair;
        VulkanSwapchain  Swapchain;

        VkRenderPass RenderPass = VK_NULL_HANDLE;
    };
}

#endif //OTTERENGINE_VULKANTYPES_CONTEXT_H
