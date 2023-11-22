#ifndef OTTERENGINE_VULKANTEXTURE_H
#define OTTERENGINE_VULKANTEXTURE_H

#include "Graphics/Abstractions/Texture.h"
#include "Graphics/API/Vulkan/VulkanBase.Includes.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Device.h"

namespace Otter::Graphics::Vulkan
{
    class VulkanTexture final : public Texture
    {
    public:
        ~VulkanTexture() final;

        explicit VulkanTexture(const char* const filePath)
            : Texture(filePath)
        {
        }

        void Bind() final;
        void Unbind() final;

        OTR_INLINE void SetDevicePair(VulkanDevicePair* const devicePair) { m_DevicePair = devicePair; }
        OTR_INLINE void SetAllocator(VkAllocationCallbacks* const allocator) { m_Allocator = allocator; }

        [[nodiscard]] OTR_INLINE VkImage GetImage() const { return m_Image; }
        [[nodiscard]] OTR_INLINE VkDeviceMemory GetImageMemory() const { return m_ImageMemory; }
        [[nodiscard]] OTR_INLINE VkImageView GetImageView() const { return m_ImageView; }
        [[nodiscard]] OTR_INLINE VkSampler GetSampler() const { return m_Sampler; }

    private:
        VkImage        m_Image       = VK_NULL_HANDLE;
        VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
        VkImageView    m_ImageView   = VK_NULL_HANDLE;
        VkSampler      m_Sampler     = VK_NULL_HANDLE;

        VulkanDevicePair     * m_DevicePair = nullptr;
        VkAllocationCallbacks* m_Allocator  = VK_NULL_HANDLE;

        void CreateImage();
        void DestroyImage();

        void CreateImageView();
        void DestroyImageView();

        void CreateSampler();
        void DestroySampler();
    };
}

#endif //OTTERENGINE_VULKANTEXTURE_H
