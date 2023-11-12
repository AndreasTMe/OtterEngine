#ifndef OTTERENGINE_VULKANTYPES_SWAPCHAIN_H
#define OTTERENGINE_VULKANTYPES_SWAPCHAIN_H

#include "Core/Collections/List.h"
#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanSwapchain
    {
        VkSwapchainKHR     Handle            = VK_NULL_HANDLE;
        VkExtent2D         Extent            = { };
        VkSurfaceFormatKHR SurfaceFormat     = { };
        VkPresentModeKHR   PresentMode       = VK_PRESENT_MODE_FIFO_KHR;
        UInt8              CurrentFrame      = 0;
        UInt8              MaxFramesInFlight = 0;
    };

    struct SwapchainSupportInfo
    {
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;
        List<VkSurfaceFormatKHR> SurfaceFormats;
        List<VkPresentModeKHR>   PresentModes;
    };
}

#endif //OTTERENGINE_VULKANTYPES_SWAPCHAIN_H
