#ifndef OTTERENGINE_VULKANCOMMANDS_H
#define OTTERENGINE_VULKANCOMMANDS_H

#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    void AllocateCommandBuffer(const VkDevice& logicalDevice,
                               const VkCommandPool& commandPool,
                               VkCommandBuffer& commandBuffer,
                               bool isPrimary = true);
    void FreeCommandBuffer(const VkDevice& logicalDevice,
                           const VkCommandPool& commandPool,
                           VkCommandBuffer& commandBuffer);
    void BeginCommandBuffer(const VkCommandBuffer& commandBuffer,
                            bool isSingleUse,
                            bool isRenderPassContinue,
                            bool isSimultaneousUse);
    void EndCommandBuffer(const VkCommandBuffer& commandBuffer);
    void CopyBuffer(const VkDevice& logicalDevice,
                    const VkDeviceSize& size,
                    const VkQueue& queue,
                    const VkCommandPool& commandPool,
                    const VkBuffer& sourceBuffer,
                    VkBuffer& destinationBuffer);
}

#endif //OTTERENGINE_VULKANCOMMANDS_H
