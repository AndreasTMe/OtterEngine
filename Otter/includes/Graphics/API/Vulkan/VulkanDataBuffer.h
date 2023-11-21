#ifndef OTTERENGINE_VULKANDATABUFFER_H
#define OTTERENGINE_VULKANDATABUFFER_H

#include "Graphics/Abstractions/DataBuffer.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Device.h"

namespace Otter::Graphics::Vulkan
{
    class VulkanDataBuffer
    {
    public:
        VulkanDataBuffer() = default;
        ~VulkanDataBuffer() = default;

        bool TryInitialise(UInt64 size, UInt32 bufferUsage, UInt32 memoryProperties);
        void CleanUp();
        void Copy(const VkBuffer& sourceBuffer,
                  const VkDeviceSize& size,
                  const VkQueue& queue,
                  const VkCommandPool& commandPool);

        void SetDevicePair(VulkanDevicePair* const devicePair) { m_DevicePair = devicePair; }
        void SetAllocator(VkAllocationCallbacks* const allocator) { m_Allocator = allocator; }

        [[nodiscard]] OTR_INLINE VkBuffer GetHandle() const { return m_Handle; }
        [[nodiscard]] OTR_INLINE VkDeviceMemory GetDeviceMemory() const { return m_DeviceMemory; }
        [[nodiscard]] OTR_INLINE VkMemoryRequirements GetMemoryRequirements() const { return m_MemoryRequirements; }
        [[nodiscard]] OTR_INLINE UInt32 GetSize() const { return m_Size; }
        [[nodiscard]] OTR_INLINE UInt32 GetBufferUsage() const { return m_BufferUsage; }
        [[nodiscard]] OTR_INLINE UInt32 GetMemoryProperties() const { return m_MemoryProperties; }

    protected:
        VulkanDevicePair     * m_DevicePair = nullptr;
        VkAllocationCallbacks* m_Allocator  = nullptr;

        VkBuffer             m_Handle             = VK_NULL_HANDLE;
        VkDeviceMemory       m_DeviceMemory       = VK_NULL_HANDLE;
        VkMemoryRequirements m_MemoryRequirements = { };
        UInt32               m_Size               = 0;
        UInt32               m_BufferUsage        = 0;
        UInt32               m_MemoryProperties   = 0;
    };

    class VulkanVertexBuffer final : public VertexBuffer, public VulkanDataBuffer
    {
    public:
        VulkanVertexBuffer()
            : VertexBuffer(), VulkanDataBuffer()
        {
        }
        ~VulkanVertexBuffer() final;

        void Write(const void* data, UInt32 size) final;
        void Bind() const final;
    };

    class VulkanIndexBuffer final : public IndexBuffer, public VulkanDataBuffer
    {
    public:
        VulkanIndexBuffer()
            : IndexBuffer(), VulkanDataBuffer()
        {
        }
        ~VulkanIndexBuffer() final;

        void Write(const void* data, UInt32 size) final;
        void Bind() const final;
    };

    class VulkanUniformBuffer final : public UniformBuffer, public VulkanDataBuffer
    {
    public:
        VulkanUniformBuffer()
            : UniformBuffer(), VulkanDataBuffer()
        {
        }
        ~VulkanUniformBuffer() final;

        void Write(const void* data, UInt32 size) final;
        void Overwrite(const void* data, UInt32 size, UInt32 offset) final;
        void Bind() const final;

        void Update(const VkDescriptorSet& descriptorSet, UInt32 size, UInt32 offset) const;
        void UpdateAll(const List <VkDescriptorSet>& descriptorSets,
                       UInt8 maxFramesInFlight,
                       UInt32 size,
                       UInt32 offset) const;
    };
}

#endif //OTTERENGINE_VULKANDATABUFFER_H
