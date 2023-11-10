#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanCommands.h"

namespace Otter::Graphics::Vulkan
{
    void AllocateCommandBuffer(const VkDevice& logicalDevice,
                               const VkCommandPool& commandPool,
                               VkCommandBuffer& commandBuffer,
                               bool isPrimary /*= true*/)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(commandPool != VK_NULL_HANDLE, "Vulkan command pool is null!")

        VkCommandBufferAllocateInfo allocInfo{ };
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                                                 : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandPool        = commandPool;
        allocInfo.commandBufferCount = 1;
        allocInfo.pNext              = nullptr;

        OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer))
    }

    void FreeCommandBuffer(const VkDevice& logicalDevice,
                           const VkCommandPool& commandPool,
                           VkCommandBuffer& commandBuffer)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(commandPool != VK_NULL_HANDLE, "Vulkan command pool is null!")

        vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
    }

    void BeginCommandBuffer(const VkCommandBuffer& commandBuffer,
                            bool isSingleUse,
                            bool isRenderPassContinue,
                            bool isSimultaneousUse)
    {
        OTR_INTERNAL_ASSERT_MSG(commandBuffer != VK_NULL_HANDLE, "Vulkan command buffer is null!")

        VkCommandBufferBeginInfo beginInfo{ };
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (isSingleUse)
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (isRenderPassContinue)
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        if (isSimultaneousUse)
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        beginInfo.pInheritanceInfo = nullptr;
        beginInfo.pNext            = nullptr;

        OTR_VULKAN_VALIDATE(vkBeginCommandBuffer(commandBuffer, &beginInfo))
    }

    void EndCommandBuffer(const VkCommandBuffer& commandBuffer)
    {
        OTR_INTERNAL_ASSERT_MSG(commandBuffer != VK_NULL_HANDLE, "Vulkan command buffer is null!")

        OTR_VULKAN_VALIDATE(vkEndCommandBuffer(commandBuffer))
    }

    void CopyBuffer(const VkDevice& logicalDevice,
                    const VkDeviceSize& size,
                    const VkQueue& queue,
                    const VkCommandPool& commandPool,
                    const VkBuffer& sourceBuffer,
                    VkBuffer& destinationBuffer)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Vulkan device size is 0!")
        OTR_INTERNAL_ASSERT_MSG(queue != VK_NULL_HANDLE, "Vulkan queue is null!")
        OTR_INTERNAL_ASSERT_MSG(commandPool != VK_NULL_HANDLE, "Vulkan command pool is null!")
        OTR_INTERNAL_ASSERT_MSG(sourceBuffer != VK_NULL_HANDLE, "Vulkan source buffer is null!")

        VkCommandBufferAllocateInfo allocInfo{ };
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool        = commandPool;
        allocInfo.commandBufferCount = 1;
        allocInfo.pNext              = nullptr;

        VkCommandBuffer commandBuffer;
        OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer))

        VkCommandBufferBeginInfo beginInfo{ };
        beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = nullptr;
        beginInfo.pNext            = nullptr;

        OTR_VULKAN_VALIDATE(vkBeginCommandBuffer(commandBuffer, &beginInfo))

        VkBufferCopy copyRegion{ };
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size      = size;
        vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion);

        OTR_VULKAN_VALIDATE(vkEndCommandBuffer(commandBuffer))

        VkSubmitInfo submitInfo{ };
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer;
        // submitInfo.pWaitDstStageMask    = nullptr;
        // submitInfo.waitSemaphoreCount   = 0;
        // submitInfo.pWaitSemaphores      = nullptr;
        // submitInfo.signalSemaphoreCount = 0;
        // submitInfo.pSignalSemaphores    = nullptr;
        // submitInfo.pNext                = nullptr;

        OTR_VULKAN_VALIDATE(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE))
        OTR_VULKAN_VALIDATE(vkQueueWaitIdle(queue))

        vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
    }
}