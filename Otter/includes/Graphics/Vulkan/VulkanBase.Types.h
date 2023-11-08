#ifndef OTTERENGINE_VULKANBASE_TYPES_H
#define OTTERENGINE_VULKANBASE_TYPES_H

#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct SwapchainSupportInfo
    {
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        List<VkSurfaceFormatKHR> m_SurfaceFormats;
        List<VkPresentModeKHR>   m_PresentModes;
    };

    struct VulkanQueueFamily
    {
        UInt32  m_Index = UINT32_MAX;
        VkQueue m_Queue = VK_NULL_HANDLE;
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
    };

    struct VulkanContext
    {
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkAllocationCallbacks* m_Allocator = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

#if !OTR_RUNTIME
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
#endif

        VulkanDevicePair m_DevicePair;
    };
}

#endif //OTTERENGINE_VULKANBASE_TYPES_H
