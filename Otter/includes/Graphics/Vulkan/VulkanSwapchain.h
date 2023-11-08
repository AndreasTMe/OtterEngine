#ifndef OTTERENGINE_VULKANSWAPCHAIN_H
#define OTTERENGINE_VULKANSWAPCHAIN_H

#include "Graphics/Vulkan/VulkanBase.Types.h"

namespace Otter::Graphics::Vulkan
{
    void CreateSwapchain(const VulkanContext* vulkanContext, VulkanSwapchain& swapchain);
    void RecreateSwapchain(const VulkanContext* vulkanContext, VulkanSwapchain& swapchain);
    void DestroySwapchain(const VulkanContext* vulkanContext);

    void QuerySwapchainSupport(const VkSurfaceKHR& surface,
                               const VkPhysicalDevice& physicalDevice,
                               SwapchainSupportInfo& swapchainSupportInfo);
}

#endif //OTTERENGINE_VULKANSWAPCHAIN_H
