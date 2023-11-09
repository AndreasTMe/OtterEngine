#ifndef OTTERENGINE_VULKANBASE_TYPES_H
#define OTTERENGINE_VULKANBASE_TYPES_H

#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanSwapchain
    {
        VkSwapchainKHR     m_Handle            = VK_NULL_HANDLE;
        VkSurfaceFormatKHR m_SurfaceFormat     = { };
        UInt32             m_ImageCount        = 0;
        VkPresentModeKHR   m_PresentMode       = VK_PRESENT_MODE_FIFO_KHR;
        VkExtent2D         m_Extent            = { };
        UInt8              m_MaxFramesInFlight = 0;
    };

    struct SwapchainSupportInfo
    {
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        List<VkSurfaceFormatKHR> m_SurfaceFormats;
        List<VkPresentModeKHR>   m_PresentModes;
    };

    struct VulkanQueueFamily
    {
        VkQueue m_Handle = VK_NULL_HANDLE;
        UInt32  m_Index  = UINT32_MAX;
    };

    struct VulkanDevicePair
    {
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         m_LogicalDevice  = VK_NULL_HANDLE;

        VulkanQueueFamily m_GraphicsQueueFamily;
        VulkanQueueFamily m_PresentationQueueFamily;
        // TODO: VulkanQueueFamily m_ComputeQueueFamily;
        // TODO: VulkanQueueFamily m_TransferQueueFamily;
        // TODO: VulkanQueueFamily m_SparseBindingQueueFamily;

        VkCommandPool m_GraphicsCommandPool = VK_NULL_HANDLE;
        // TODO: VkCommandPool m_CopyCommandPool = VK_NULL_HANDLE;

        [[nodiscard]] OTR_INLINE bool GraphicsAndPresentationQueueFamiliesAreTheSame() const
        {
            return m_GraphicsQueueFamily.m_Index == m_PresentationQueueFamily.m_Index;
        }
    };

    struct VulkanContext
    {
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkAllocationCallbacks* m_Allocator = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

#if !OTR_RUNTIME
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
#endif

        VulkanDevicePair      m_DevicePair;
        VulkanSwapchain       m_Swapchain;
        List<VkCommandBuffer> m_CommandBuffers;
    };
}

#endif //OTTERENGINE_VULKANBASE_TYPES_H
