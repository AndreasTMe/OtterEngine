#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanCommands.h"

namespace Otter::Graphics::Vulkan
{
    void CreateCommandBuffers(const VulkanContext* const vulkanContext,
                              VkCommandPool& commandPool,
                              List <VkCommandBuffer>& commandBuffers)
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{ };
        commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = vulkanContext->m_DevicePair.m_GraphicsQueueFamily.m_Index;
        commandPoolCreateInfo.pNext            = nullptr;

        OTR_VULKAN_VALIDATE(vkCreateCommandPool(vulkanContext->m_DevicePair.m_LogicalDevice,
                                                &commandPoolCreateInfo,
                                                vulkanContext->m_Allocator,
                                                &commandPool))

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ };
        commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool        = commandPool;
        commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = vulkanContext->m_Swapchain.m_MaxFramesInFlight;
        commandBufferAllocateInfo.pNext              = nullptr;

        VkCommandBuffer tempCommandBuffers[vulkanContext->m_Swapchain.m_MaxFramesInFlight];
        OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(vulkanContext->m_DevicePair.m_LogicalDevice,
                                                     &commandBufferAllocateInfo,
                                                     tempCommandBuffers))

        Collections::New<VkCommandBuffer>(tempCommandBuffers,
                                          vulkanContext->m_Swapchain.m_MaxFramesInFlight,
                                          commandBuffers);
    }

    void DestroyCommandBuffers(const VulkanContext* const vulkanContext, List <VkCommandBuffer>& commandBuffers)
    {
        OTR_INTERNAL_ASSERT_MSG(vulkanContext->m_DevicePair.m_GraphicsCommandPool != VK_NULL_HANDLE,
                                "Vulkan command pool is null!")
        OTR_INTERNAL_ASSERT_MSG(vulkanContext->m_DevicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                "Vulkan logical device is null!")

        vkDestroyCommandPool(vulkanContext->m_DevicePair.m_LogicalDevice,
                             vulkanContext->m_DevicePair.m_GraphicsCommandPool,
                             vulkanContext->m_Allocator);

        commandBuffers.ClearDestructive();
    }

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
        OTR_VULKAN_VALIDATE(vkEndCommandBuffer(commandBuffer))
    }

    // TODO: Move this to elsewhere maybe?
    void CopyBuffer(const VulkanContext* const vulkanContext,
                    const VkBuffer& sourceBuffer,
                    const VkDeviceSize& size,
                    VkBuffer& destinationBuffer)
    {
        OTR_INTERNAL_ASSERT_MSG(sourceBuffer != VK_NULL_HANDLE, "Vulkan source buffer is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Vulkan device size is 0!")

        VkCommandBufferAllocateInfo allocInfo{ };
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool        = vulkanContext->m_DevicePair.m_GraphicsCommandPool; // TODO: Use copy command pool
        allocInfo.commandBufferCount = 1;
        allocInfo.pNext              = nullptr;

        VkCommandBuffer commandBuffer;
        OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(vulkanContext->m_DevicePair.m_LogicalDevice,
                                                     &allocInfo,
                                                     &commandBuffer))

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

        OTR_VULKAN_VALIDATE(vkQueueSubmit(vulkanContext->m_DevicePair.m_GraphicsQueueFamily.m_Handle,
                                          1,
                                          &submitInfo,
                                          VK_NULL_HANDLE))
        OTR_VULKAN_VALIDATE(vkQueueWaitIdle(vulkanContext->m_DevicePair.m_GraphicsQueueFamily.m_Handle))

        vkFreeCommandBuffers(vulkanContext->m_DevicePair.m_LogicalDevice,
                             vulkanContext->m_DevicePair.m_GraphicsCommandPool, // TODO: Use copy command pool
                             1,
                             &commandBuffer);
    }
}