#ifndef OTTERENGINE_VULKANCOMMANDS_H
#define OTTERENGINE_VULKANCOMMANDS_H

#include "Graphics/Vulkan/VulkanBase.Types.h"

namespace Otter::Graphics::Vulkan
{
    void CreateCommandBuffers(const VulkanContext* vulkanContext,
                              VkCommandPool& commandPool,
                              List <VkCommandBuffer>& commandBuffers);
    void DestroyCommandBuffers(const VulkanContext* vulkanContext, List <VkCommandBuffer>& commandBuffers);

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

    void CopyBuffer(const VulkanContext* vulkanContext,
                    const VkBuffer& sourceBuffer,
                    const VkDeviceSize& size,
                    VkBuffer& destinationBuffer);
}

#endif //OTTERENGINE_VULKANCOMMANDS_H
