#ifndef OTTERENGINE_VULKANBASE_TYPES_H
#define OTTERENGINE_VULKANBASE_TYPES_H

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
        VkSurfaceCapabilitiesKHR  SurfaceCapabilities;
        List <VkSurfaceFormatKHR> SurfaceFormats;
        List <VkPresentModeKHR>   PresentModes;
    };

    struct VulkanQueueFamily
    {
        VkQueue Handle = VK_NULL_HANDLE;
        UInt32  Index  = UINT32_MAX;
    };

    struct VulkanDevicePair
    {
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         LogicalDevice  = VK_NULL_HANDLE;

        VulkanQueueFamily GraphicsQueueFamily;
        VulkanQueueFamily PresentationQueueFamily;
        // TODO: VulkanQueueFamily ComputeQueueFamily;
        // TODO: VulkanQueueFamily TransferQueueFamily;
        // TODO: VulkanQueueFamily SparseBindingQueueFamily;

        VkCommandPool GraphicsCommandPool = VK_NULL_HANDLE;
        // TODO: VkCommandPool CopyCommandPool = VK_NULL_HANDLE;

        List <VkCommandBuffer> CommandBuffers;

        List <VkSemaphore> ImageAvailableSemaphores;
        List <VkSemaphore> RenderFinishedSemaphores;
        List <VkFence>     RenderInFlightFences;

        [[nodiscard]] OTR_INLINE bool GraphicsAndPresentationQueueFamiliesAreTheSame() const
        {
            return GraphicsQueueFamily.Index == PresentationQueueFamily.Index;
        }
    };

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

#endif //OTTERENGINE_VULKANBASE_TYPES_H
