#ifndef OTTERENGINE_VULKANTYPES_DEVICE_H
#define OTTERENGINE_VULKANTYPES_DEVICE_H

#include "Core/Collections/List.h"
#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
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

        List<VkCommandBuffer> CommandBuffers;

        List<VkSemaphore> ImageAvailableSemaphores;
        List<VkSemaphore> RenderFinishedSemaphores;
        List<VkFence>     RenderInFlightFences;

        [[nodiscard]] OTR_INLINE bool GraphicsAndPresentationQueueFamiliesAreTheSame() const
        {
            return GraphicsQueueFamily.Index == PresentationQueueFamily.Index;
        }
    };
}

#endif //OTTERENGINE_VULKANTYPES_DEVICE_H
