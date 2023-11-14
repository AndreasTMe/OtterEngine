#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanBuffers.h"

namespace Otter::Graphics::Vulkan
{
    bool TryCreateBuffer(const VulkanDevicePair& devicePair,
                         const VkAllocationCallbacks* const allocator,
                         const UInt64& size,
                         const UInt32& bufferUsage,
                         const UInt32& memoryProperties,
                         VulkanBuffer* outBuffer)
    {
        OTR_INTERNAL_ASSERT_MSG(devicePair.PhysicalDevice != VK_NULL_HANDLE, "Vulkan physical device is null!")
        OTR_INTERNAL_ASSERT_MSG(devicePair.LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Vulkan device size is 0!")

        VkBufferCreateInfo bufferInfo{ };
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = size;
        bufferInfo.usage       = bufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        OTR_VULKAN_VALIDATE(vkCreateBuffer(devicePair.LogicalDevice,
                                           &bufferInfo,
                                           allocator,
                                           &outBuffer->Handle))

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(devicePair.LogicalDevice, outBuffer->Handle, &memoryRequirements);

        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(devicePair.PhysicalDevice, &deviceMemoryProperties);

        for (UInt32 i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
        {
            if (memoryRequirements.memoryTypeBits & (1 << i)
                && (deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
            {
                VkMemoryAllocateInfo allocInfo{ };
                allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize  = memoryRequirements.size;
                allocInfo.memoryTypeIndex = i;

                OTR_VULKAN_VALIDATE(vkAllocateMemory(devicePair.LogicalDevice,
                                                     &allocInfo,
                                                     allocator,
                                                     &outBuffer->DeviceMemory))

                outBuffer->MemoryRequirements = memoryRequirements;
                outBuffer->Size               = size;
                outBuffer->BufferUsage        = bufferUsage;
                outBuffer->MemoryProperties   = memoryProperties;

                return true;
            }
        }

        return false;
    }

    void DestroyBuffer(const VkDevice& logicalDevice,
                       const VkAllocationCallbacks* const allocator,
                       VulkanBuffer* outBuffer)
    {
        OTR_INTERNAL_ASSERT_MSG(outBuffer->Handle != VK_NULL_HANDLE, "Vulkan buffer handle is null!")
        OTR_INTERNAL_ASSERT_MSG(outBuffer->DeviceMemory != VK_NULL_HANDLE, "Vulkan buffer device memory is null!")

        vkDestroyBuffer(logicalDevice, outBuffer->Handle, allocator);
        vkFreeMemory(logicalDevice, outBuffer->DeviceMemory, allocator);

        outBuffer->Handle             = VK_NULL_HANDLE;
        outBuffer->DeviceMemory       = VK_NULL_HANDLE;
        outBuffer->MemoryRequirements = { };
        outBuffer->Size               = 0;
        outBuffer->BufferUsage        = 0;
        outBuffer->MemoryProperties   = 0;
    }

    void BindBuffer(VkDevice const& logicalDevice, const VulkanBuffer& buffer, const UInt32& memoryOffset)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(buffer.Handle != VK_NULL_HANDLE, "Vulkan buffer handle is null!")
        OTR_INTERNAL_ASSERT_MSG(buffer.DeviceMemory != VK_NULL_HANDLE, "Vulkan buffer device memory is null!")

        OTR_VULKAN_VALIDATE(vkBindBufferMemory(logicalDevice, buffer.Handle, buffer.DeviceMemory, memoryOffset))
    }

    void CopyBuffer(const VkDevice& logicalDevice,
                    const VkDeviceSize& size,
                    const VkQueue& queue,
                    const VkCommandPool& commandPool,
                    const VkBuffer& sourceBuffer,
                    VkBuffer* destinationBuffer)
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
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size      = size;
        vkCmdCopyBuffer(commandBuffer, sourceBuffer, *destinationBuffer, 1, &copyRegion);

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