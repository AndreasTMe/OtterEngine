#include "Otter.PCH.h"

#include "Graphics/API/Vulkan/VulkanDataBuffer.h"

#if OTR_VULKAN_ENABLED

namespace Otter::Graphics
{
    DataBuffer* DataBuffer::Create(BufferType type)
    {
        switch (type)
        {
            case BufferType::Vertex:
                return New<Vulkan::VulkanVertexBuffer>();
            case BufferType::Index:
                return New<Vulkan::VulkanIndexBuffer>();
            case BufferType::Uniform:
                return New<Vulkan::VulkanUniformBuffer>();
            default:
            {
                OTR_INTERNAL_ASSERT_MSG(false, "Invalid buffer type")
            }
                return nullptr;
        }
    }

    void DataBuffer::Destroy(BufferType type, DataBuffer* outBuffer)
    {
        switch (type)
        {
            case BufferType::Vertex:
                Delete<Vulkan::VulkanVertexBuffer>((Vulkan::VulkanVertexBuffer*) outBuffer);
                break;
            case BufferType::Index:
                Delete<Vulkan::VulkanIndexBuffer>((Vulkan::VulkanIndexBuffer*) outBuffer);
                break;
            case BufferType::Uniform:
                Delete<Vulkan::VulkanUniformBuffer>((Vulkan::VulkanUniformBuffer*) outBuffer);
                break;
            default:
            {
                OTR_INTERNAL_ASSERT_MSG(false, "Invalid buffer type")
            }
                return;
        }
        outBuffer = nullptr;
    }
}

#endif

namespace Otter::Graphics::Vulkan
{
    bool VulkanDataBuffer::TryInitialise(const UInt64 size, const UInt32 bufferUsage, const UInt32 memoryProperties)
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->PhysicalDevice != VK_NULL_HANDLE, "Vulkan physical device is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Vulkan device size is 0!")

        VkBufferCreateInfo bufferInfo{ };
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = size;
        bufferInfo.usage       = bufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        OTR_VULKAN_VALIDATE(vkCreateBuffer(m_DevicePair->LogicalDevice,
                                           &bufferInfo,
                                           m_Allocator,
                                           &m_Handle))

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_DevicePair->LogicalDevice, m_Handle, &memoryRequirements);

        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(m_DevicePair->PhysicalDevice, &deviceMemoryProperties);

        for (UInt32 i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
        {
            if (memoryRequirements.memoryTypeBits & (1 << i)
                && (deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
            {
                VkMemoryAllocateInfo allocInfo{ };
                allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize  = memoryRequirements.size;
                allocInfo.memoryTypeIndex = i;

                OTR_VULKAN_VALIDATE(vkAllocateMemory(m_DevicePair->LogicalDevice,
                                                     &allocInfo,
                                                     m_Allocator,
                                                     &m_DeviceMemory))

                m_MemoryRequirements = memoryRequirements;
                m_Size               = size;
                m_BufferUsage        = bufferUsage;
                m_MemoryProperties   = memoryProperties;

                return true;
            }
        }

        return false;
    }

    void VulkanDataBuffer::CleanUp()
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(m_Handle != VK_NULL_HANDLE, "Vulkan buffer handle is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DeviceMemory != VK_NULL_HANDLE, "Vulkan buffer device memory is null!")

        vkDestroyBuffer(m_DevicePair->LogicalDevice, m_Handle, m_Allocator);
        vkFreeMemory(m_DevicePair->LogicalDevice, m_DeviceMemory, m_Allocator);

        m_DevicePair         = nullptr;
        m_Allocator          = nullptr;
        m_Handle             = VK_NULL_HANDLE;
        m_DeviceMemory       = VK_NULL_HANDLE;
        m_MemoryRequirements = { };
        m_Size               = 0;
        m_BufferUsage        = 0;
        m_MemoryProperties   = 0;
    }

    void VulkanDataBuffer::Copy(const VkBuffer& sourceBuffer,
                                const VkDeviceSize& size,
                                const VkQueue& queue,
                                const VkCommandPool& commandPool)
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(sourceBuffer != VK_NULL_HANDLE, "Vulkan source buffer is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Vulkan device size is 0!")
        OTR_INTERNAL_ASSERT_MSG(queue != VK_NULL_HANDLE, "Vulkan queue is null!")
        OTR_INTERNAL_ASSERT_MSG(commandPool != VK_NULL_HANDLE, "Vulkan command pool is null!")

        VkCommandBufferAllocateInfo allocInfo{ };
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool        = commandPool;
        allocInfo.commandBufferCount = 1;
        allocInfo.pNext              = nullptr;

        VkCommandBuffer commandBuffer;
        OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(m_DevicePair->LogicalDevice, &allocInfo, &commandBuffer))

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
        vkCmdCopyBuffer(commandBuffer, sourceBuffer, m_Handle, 1, &copyRegion);

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

        vkFreeCommandBuffers(m_DevicePair->LogicalDevice, commandPool, 1, &commandBuffer);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        CleanUp();
    }

    void VulkanVertexBuffer::Write(const void* const data, const UInt32 size)
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(data != nullptr, "Data is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Size is 0!")

        VulkanDataBuffer stagingBuffer;
        stagingBuffer.SetDevicePair(m_DevicePair);
        stagingBuffer.SetAllocator(m_Allocator);
        if (!stagingBuffer.TryInitialise(size,
                                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            OTR_LOG_FATAL("Failed to create staging buffer")
            return;
        }

        OTR_VULKAN_VALIDATE(vkBindBufferMemory(m_DevicePair->LogicalDevice,
                                               stagingBuffer.GetHandle(),
                                               stagingBuffer.GetDeviceMemory(),
                                               0))

        void* mappedData;
        vkMapMemory(m_DevicePair->LogicalDevice, stagingBuffer.GetDeviceMemory(), 0, size, 0, &mappedData);
        memcpy(mappedData, (void*) data, (Size) size);
        vkUnmapMemory(m_DevicePair->LogicalDevice, stagingBuffer.GetDeviceMemory());

        if (!TryInitialise(size,
                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
        {
            OTR_LOG_FATAL("Failed to create vertex buffer")
            stagingBuffer.CleanUp();
            return;
        }

        Bind();
        Copy(stagingBuffer.GetHandle(),
             size,
             m_DevicePair->GraphicsQueueFamily.Handle,
             m_DevicePair->GraphicsCommandPool);

        stagingBuffer.CleanUp();
    }

    void VulkanVertexBuffer::Bind() const
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(m_Handle != VK_NULL_HANDLE, "Vulkan buffer handle is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DeviceMemory != VK_NULL_HANDLE, "Vulkan buffer device memory is null!")

        OTR_VULKAN_VALIDATE(vkBindBufferMemory(m_DevicePair->LogicalDevice, m_Handle, m_DeviceMemory, 0))
    }

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
        CleanUp();
    }

    void VulkanIndexBuffer::Write(const void* const data, const UInt32 size)
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(data != nullptr, "Data is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Size is 0!")

        m_Count = size / sizeof(UInt16);

        VulkanDataBuffer stagingBuffer;
        stagingBuffer.SetDevicePair(m_DevicePair);
        stagingBuffer.SetAllocator(m_Allocator);
        if (!stagingBuffer.TryInitialise(size,
                                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            OTR_LOG_FATAL("Failed to create staging buffer")
            return;
        }

        OTR_VULKAN_VALIDATE(vkBindBufferMemory(m_DevicePair->LogicalDevice,
                                               stagingBuffer.GetHandle(),
                                               stagingBuffer.GetDeviceMemory(),
                                               0))

        void* mappedData;
        vkMapMemory(m_DevicePair->LogicalDevice, stagingBuffer.GetDeviceMemory(), 0, size, 0, &mappedData);
        memcpy(mappedData, (void*) data, (Size) size);
        vkUnmapMemory(m_DevicePair->LogicalDevice, stagingBuffer.GetDeviceMemory());

        if (!TryInitialise(size,
                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
        {
            OTR_LOG_FATAL("Failed to create vertex buffer")
            stagingBuffer.CleanUp();
            return;
        }

        Bind();
        Copy(stagingBuffer.GetHandle(),
             size,
             m_DevicePair->GraphicsQueueFamily.Handle,
             m_DevicePair->GraphicsCommandPool);

        stagingBuffer.CleanUp();
    }

    void VulkanIndexBuffer::Bind() const
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(m_Handle != VK_NULL_HANDLE, "Vulkan buffer handle is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DeviceMemory != VK_NULL_HANDLE, "Vulkan buffer device memory is null!")

        OTR_VULKAN_VALIDATE(vkBindBufferMemory(m_DevicePair->LogicalDevice, m_Handle, m_DeviceMemory, 0))
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        CleanUp();
    }

    void VulkanUniformBuffer::Write(const void* const data, const UInt32 size)
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Size is 0!")

        if (!TryInitialise(size,
                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            OTR_LOG_FATAL("Failed to create uniform buffer")
            return;
        }

        Bind();

        if (data != nullptr)
            Overwrite(data, size, 0);
    }

    void VulkanUniformBuffer::Overwrite(const void* const data, const UInt32 size, const UInt32 offset)
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(data != nullptr, "Data is null!")
        OTR_INTERNAL_ASSERT_MSG(size != 0, "Size is 0!")

        void* mappedData;
        vkMapMemory(m_DevicePair->LogicalDevice, m_DeviceMemory, 0, size, 0, &mappedData);
        memcpy(mappedData, data, (Size) size);
        vkUnmapMemory(m_DevicePair->LogicalDevice, m_DeviceMemory);
    }

    void VulkanUniformBuffer::Bind() const
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Vulkan device pair is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(m_Handle != VK_NULL_HANDLE, "Vulkan buffer handle is null!")
        OTR_INTERNAL_ASSERT_MSG(m_DeviceMemory != VK_NULL_HANDLE, "Vulkan buffer device memory is null!")

        OTR_VULKAN_VALIDATE(vkBindBufferMemory(m_DevicePair->LogicalDevice, m_Handle, m_DeviceMemory, 0))
    }

    void VulkanUniformBuffer::Update(const VkDescriptorSet& descriptorSet, const UInt32 size, const UInt32 offset) const
    {
        VkDescriptorBufferInfo bufferInfo{ };
        bufferInfo.buffer = m_Handle;
        bufferInfo.offset = offset;
        bufferInfo.range  = size;

        VkWriteDescriptorSet descriptorWrite{ };
        descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet           = descriptorSet;
        descriptorWrite.dstBinding       = 0;
        descriptorWrite.dstArrayElement  = 0;
        descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pBufferInfo      = &bufferInfo;
        descriptorWrite.pImageInfo       = VK_NULL_HANDLE;
        descriptorWrite.pTexelBufferView = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(m_DevicePair->LogicalDevice, 1, &descriptorWrite, 0, VK_NULL_HANDLE);
    }

    void VulkanUniformBuffer::UpdateAll(const List <VkDescriptorSet>& descriptorSets,
                                        const UInt8 maxFramesInFlight,
                                        const UInt32 size,
                                        const UInt32 offset) const
    {
        VkDescriptorBufferInfo bufferInfo{ };
        bufferInfo.buffer = m_Handle;
        bufferInfo.offset = offset;
        bufferInfo.range  = size;

        for (UInt64 i = 0; i < maxFramesInFlight; i++)
        {
            VkWriteDescriptorSet descriptorWrite{ };
            descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet           = descriptorSets[i];
            descriptorWrite.dstBinding       = 0;
            descriptorWrite.dstArrayElement  = 0;
            descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount  = 1;
            descriptorWrite.pBufferInfo      = &bufferInfo;
            descriptorWrite.pImageInfo       = VK_NULL_HANDLE;
            descriptorWrite.pTexelBufferView = VK_NULL_HANDLE;

            vkUpdateDescriptorSets(m_DevicePair->LogicalDevice, 1, &descriptorWrite, 0, VK_NULL_HANDLE);
        }
    }
}