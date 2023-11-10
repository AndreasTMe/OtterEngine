#ifndef OTTERENGINE_VULKANBASE_INTERNAL_H
#define OTTERENGINE_VULKANBASE_INTERNAL_H

#include "VulkanBase.Types.h"

namespace Otter::Graphics::Vulkan::Internal
{
    // HELP: VkInstance related
    static void CreateVulkanInstance();
    static void DestroyVulkanInstance();
    static void GetRequiredInstanceExtensions(List<const char*>& extensions);

    // HELP: VkSurfaceKHR related
    static void CreateSurface(const void* platformContext);
    static void DestroySurface();

    // HELP: VkPhysicalDevice & VkDevice related
    static void CreateDevicePairs();
    static void DestroyDevicePairs();
    static void SelectPhysicalDevice();
    static void CreateLogicalDevice();
    static void GetDeviceRequiredExtensions(List<const char*>& requiredExtensions);
    static void QueryQueueFamilies(const VkPhysicalDevice& physicalDevice,
                                   UInt32& graphicsFamily,
                                   UInt32& presentFamily);
    static bool DeviceSupportsRequiredExtensions(const VkPhysicalDevice& physicalDevice);

    // HELP: VkSwapchainKHR related
    static void CreateSwapchain();
    static void RecreateSwapchain();
    static void CreateSwapchainInternal(const VulkanDevicePair& devicePair,
                                        const VkAllocationCallbacks* allocator,
                                        VulkanSwapchain& swapchain);
    static void DestroySwapchain();
    static void QuerySwapchainSupport(const VkPhysicalDevice& physicalDevice,
                                      SwapchainSupportInfo& swapchainSupportInfo);
    static VkSurfaceFormatKHR SelectSwapchainSurfaceFormat(const List<VkSurfaceFormatKHR>& surfaceFormats);
    static VkPresentModeKHR SelectSwapchainPresentMode(const List<VkPresentModeKHR>& presentModes);
    static VkExtent2D SelectSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    static void CreateSwapchainImages(const VkDevice& logicalDevice,
                                      const VkSwapchainKHR& swapchain,
                                      UInt32& imageCount);
    static void CreateSwapchainImageViews(const VkDevice& logicalDevice, const VkFormat& imageFormat);

    // HELP: VkRenderPass related
    static void CreateRenderPass();
    static void DestroyRenderPass();

    // HELP: VkCommandPool & VkCommandBuffer related
    static void CreateCommandBuffers(VkCommandPool& commandPool, List<VkCommandBuffer>& commandBuffers);
    static void DestroyCommandBuffers(List<VkCommandBuffer>& commandBuffers);
    static void AllocateCommandBuffer(const VkDevice& logicalDevice,
                                      const VkCommandPool& commandPool,
                                      VkCommandBuffer& commandBuffer,
                                      bool isPrimary = true);
    static void FreeCommandBuffer(const VkDevice& logicalDevice,
                                  const VkCommandPool& commandPool,
                                  VkCommandBuffer& commandBuffer);
    static void BeginCommandBuffer(const VkCommandBuffer& commandBuffer,
                                   bool isSingleUse,
                                   bool isRenderPassContinue,
                                   bool isSimultaneousUse);
    static void EndCommandBuffer(const VkCommandBuffer& commandBuffer);
    static void CopyBuffer(const VulkanContext* vulkanContext,
                           const VkBuffer& sourceBuffer,
                           const VkDeviceSize& size,
                           VkBuffer& destinationBuffer);

#if !OTR_RUNTIME
    // HELP: VkDebugUtilsMessenger related
    static void CreateVulkanDebugMessenger();
    static void DestroyVulkanDebugMessenger();

    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                                  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                  void* userData);

    static void GetRequiredInstanceValidationLayers(List<const char*>& layers);
#endif
}

#endif //OTTERENGINE_VULKANBASE_INTERNAL_H
