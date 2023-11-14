#ifndef OTTERENGINE_VULKANSWAPCHAINS_H
#define OTTERENGINE_VULKANSWAPCHAINS_H

#include "Core/Collections/List.h"
#include "Graphics/Vulkan/VulkanBase.Includes.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Device.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Swapchain.h"

namespace Otter::Graphics::Vulkan
{
    void CreateSingleSwapchain(const VkSurfaceKHR& surface,
                               const VulkanDevicePair& devicePair,
                               const VkAllocationCallbacks* allocator,
                               VulkanSwapchain* outSwapchain);
    void QuerySwapchainSupport(const VkSurfaceKHR& surface,
                               const VkPhysicalDevice& physicalDevice,
                               SwapchainSupportInfo* outSwapchainSupportInfo);
    VkSurfaceFormatKHR SelectSwapchainSurfaceFormat(const List <VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR SelectSwapchainPresentMode(const List <VkPresentModeKHR>& presentModes);
    VkExtent2D SelectSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CreateSwapchainImages(const VkDevice& logicalDevice,
                               const VkSwapchainKHR& swapchain,
                               List <VkImage>& swapchainImages);
    void CreateSwapchainImageViews(const VkDevice& logicalDevice,
                                   const VkAllocationCallbacks* allocator,
                                   const List <VkImage>& swapchainImages,
                                   const VkFormat& imageFormat,
                                   List <VkImageView>& swapchainImageViews);
    void CreateSwapchainFrameBuffers(const VkDevice& logicalDevice,
                                     const VkAllocationCallbacks* allocator,
                                     const VkExtent2D& swapChainExtent,
                                     const List <VkImageView>& swapchainImageViews,
                                     const VkRenderPass& renderPass,
                                     List <VkFramebuffer>& swapchainFrameBuffers);
}

#endif //OTTERENGINE_VULKANSWAPCHAINS_H
