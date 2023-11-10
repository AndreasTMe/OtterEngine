#ifndef OTTERENGINE_VULKANSWAPCHAINS_H
#define OTTERENGINE_VULKANSWAPCHAINS_H

#include "Core/Collections/List.h"
#include "Graphics/Vulkan/VulkanBase.Types.h"

namespace Otter::Graphics::Vulkan
{
    void CreateSingleSwapchain(const VkSurfaceKHR& surface,
                               const VulkanDevicePair& devicePair,
                               const VkAllocationCallbacks* allocator,
                               VulkanSwapchain& swapchain);
    void QuerySwapchainSupport(const VkSurfaceKHR& surface,
                               const VkPhysicalDevice& physicalDevice,
                               SwapchainSupportInfo& swapchainSupportInfo);
    VkSurfaceFormatKHR SelectSwapchainSurfaceFormat(const List <VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR SelectSwapchainPresentMode(const List <VkPresentModeKHR>& presentModes);
    VkExtent2D SelectSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CreateSwapchainImages(const VkDevice& logicalDevice,
                               const VkSwapchainKHR& swapchain,
                               List <VkImage>& swapchainImages);
    void CreateSwapchainImageViews(const VkDevice& logicalDevice,
                                   const List <VkImage>& swapchainImages,
                                   const VkFormat& imageFormat,
                                   List <VkImageView>& swapchainImageViews);
}

#endif //OTTERENGINE_VULKANSWAPCHAINS_H
