#ifndef OTTERENGINE_VULKANBUFFERS_H
#define OTTERENGINE_VULKANBUFFERS_H

#include "Graphics/API/Vulkan/Types/VulkanTypes.Buffer.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Device.h"

namespace Otter::Graphics::Vulkan
{
    bool TryCreateBuffer(const VulkanDevicePair& devicePair,
                         const VkAllocationCallbacks* allocator,
                         const UInt64& size,
                         const UInt32& bufferUsage,
                         const UInt32& memoryProperties,
                         VulkanBuffer* outBuffer);
    void DestroyBuffer(const VkDevice& logicalDevice,
                       const VkAllocationCallbacks* allocator,
                       VulkanBuffer* outBuffer);

    void BindBuffer(const VkDevice& logicalDevice, const VulkanBuffer& buffer, const UInt32& memoryOffset);
    void CopyBuffer(const VkDevice& logicalDevice,
                    const VkDeviceSize& size,
                    const VkQueue& queue,
                    const VkCommandPool& commandPool,
                    const VkBuffer& sourceBuffer,
                    VkBuffer* destinationBuffer);
}

#endif //OTTERENGINE_VULKANBUFFERS_H
