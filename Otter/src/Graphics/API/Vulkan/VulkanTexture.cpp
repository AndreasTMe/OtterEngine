#include "Graphics/API/Vulkan/VulkanTexture.h"

#include "Graphics/API/Vulkan/VulkanDataBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if OTR_GRAPHICS_VULKAN_ENABLED

namespace Otter::Graphics
{
    Texture* Texture::Create(const char* const filePath)
    {
        return New<Vulkan::VulkanTexture>(filePath);
    }

    void Texture::Destroy(Texture* outTexture)
    {
        Delete<Vulkan::VulkanTexture>((Vulkan::VulkanTexture*) outTexture);
        outTexture = nullptr;
    }
}

#endif

namespace Otter::Graphics::Vulkan
{
    VulkanTexture::~VulkanTexture()
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Device pair is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Logical device is null")

        Unbind();

        m_DevicePair = nullptr;
        m_Allocator  = VK_NULL_HANDLE;
    }

    void VulkanTexture::Bind()
    {
        CreateImage();
        CreateImageView();
        CreateSampler();
    }

    void VulkanTexture::Unbind()
    {
        DestroySampler();
        DestroyImageView();
        DestroyImage();
    }

    void VulkanTexture::CreateImage()
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Device pair is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Logical device is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->PhysicalDevice != VK_NULL_HANDLE, "Physical device is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->GraphicsQueueFamily.Handle != VK_NULL_HANDLE,
                                "Graphics queue family is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->GraphicsCommandPool != VK_NULL_HANDLE, "Graphics command pool is null")

        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(m_FilePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        m_Width  = static_cast<UInt32>(texWidth);
        m_Height = static_cast<UInt32>(texHeight);
        VkDeviceSize imageSize = m_Width * m_Height * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VulkanDataBuffer stagingBuffer;
        stagingBuffer.SetDevicePair(m_DevicePair);
        stagingBuffer.SetAllocator(m_Allocator);
        if (!stagingBuffer.TryInitialise(imageSize,
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

        void* data;
        vkMapMemory(m_DevicePair->LogicalDevice, stagingBuffer.GetDeviceMemory(), 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<Size>(imageSize));
        vkUnmapMemory(m_DevicePair->LogicalDevice, stagingBuffer.GetDeviceMemory());

        stbi_image_free(pixels);

        // HELP: Create Image
        VkImageCreateInfo imageInfo{ };
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = m_Width;
        imageInfo.extent.height = m_Height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

        OTR_VULKAN_VALIDATE(vkCreateImage(m_DevicePair->LogicalDevice, &imageInfo, m_Allocator, &m_Image))

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_DevicePair->LogicalDevice, m_Image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{ };
        allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_DevicePair->PhysicalDevice, &memProperties);

        for (UInt32 i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((memRequirements.memoryTypeBits & (1 << i))
                && (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            {
                allocInfo.memoryTypeIndex = i;
                break;
            }
        }

        OTR_VULKAN_VALIDATE(vkAllocateMemory(m_DevicePair->LogicalDevice,
                                             &allocInfo,
                                             m_Allocator,
                                             &m_ImageMemory))

        vkBindImageMemory(m_DevicePair->LogicalDevice, m_Image, m_ImageMemory, 0);

        // HELP: Transition Image Layout
        VkCommandBufferAllocateInfo allocInfoA{ };
        allocInfoA.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfoA.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfoA.commandPool        = m_DevicePair->GraphicsCommandPool;
        allocInfoA.commandBufferCount = 1;

        VkCommandBuffer commandBufferA;
        vkAllocateCommandBuffers(m_DevicePair->LogicalDevice, &allocInfoA, &commandBufferA);

        VkCommandBufferBeginInfo beginInfoA{ };
        beginInfoA.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoA.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBufferA, &beginInfoA);

        VkImageMemoryBarrier barrierA{ };
        barrierA.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierA.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
        barrierA.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierA.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrierA.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrierA.image                           = m_Image;
        barrierA.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrierA.subresourceRange.baseMipLevel   = 0;
        barrierA.subresourceRange.levelCount     = 1;
        barrierA.subresourceRange.baseArrayLayer = 0;
        barrierA.subresourceRange.layerCount     = 1;
        barrierA.srcAccessMask                   = 0;
        barrierA.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

        VkPipelineStageFlags sourceStageA;
        sourceStageA = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        VkPipelineStageFlags destinationStageA;
        destinationStageA = VK_PIPELINE_STAGE_TRANSFER_BIT;

        vkCmdPipelineBarrier(
            commandBufferA,
            sourceStageA, destinationStageA,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrierA
        );

        vkEndCommandBuffer(commandBufferA);

        VkSubmitInfo submitInfoA{ };
        submitInfoA.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfoA.commandBufferCount = 1;
        submitInfoA.pCommandBuffers    = &commandBufferA;

        vkQueueSubmit(m_DevicePair->GraphicsQueueFamily.Handle, 1, &submitInfoA, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_DevicePair->GraphicsQueueFamily.Handle);

        vkFreeCommandBuffers(m_DevicePair->LogicalDevice, m_DevicePair->GraphicsCommandPool, 1, &commandBufferA);

        // HELP: Copy Buffer to Image
        VkCommandBufferAllocateInfo allocInfoB{ };
        allocInfoB.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfoB.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfoB.commandPool        = m_DevicePair->GraphicsCommandPool;
        allocInfoB.commandBufferCount = 1;

        VkCommandBuffer commandBufferB;
        vkAllocateCommandBuffers(m_DevicePair->LogicalDevice, &allocInfoB, &commandBufferB);

        VkCommandBufferBeginInfo beginInfoB{ };
        beginInfoB.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoB.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBufferB, &beginInfoB);

        VkBufferImageCopy region{ };
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = { 0, 0, 0 };
        region.imageExtent                     = { m_Width, m_Height, 1 };

        vkCmdCopyBufferToImage(commandBufferB,
                               stagingBuffer.GetHandle(),
                               m_Image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

        vkEndCommandBuffer(commandBufferB);

        VkSubmitInfo submitInfoB{ };
        submitInfoB.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfoB.commandBufferCount = 1;
        submitInfoB.pCommandBuffers    = &commandBufferB;

        vkQueueSubmit(m_DevicePair->GraphicsQueueFamily.Handle, 1, &submitInfoB, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_DevicePair->GraphicsQueueFamily.Handle);

        vkFreeCommandBuffers(m_DevicePair->LogicalDevice, m_DevicePair->GraphicsCommandPool, 1, &commandBufferB);

        // HELP: Transition Image Layout
        VkCommandBufferAllocateInfo allocInfoC{ };
        allocInfoC.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfoC.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfoC.commandPool        = m_DevicePair->GraphicsCommandPool;
        allocInfoC.commandBufferCount = 1;

        VkCommandBuffer commandBufferC;
        vkAllocateCommandBuffers(m_DevicePair->LogicalDevice, &allocInfoC, &commandBufferC);

        VkCommandBufferBeginInfo beginInfoC{ };
        beginInfoC.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoC.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBufferC, &beginInfoC);

        VkImageMemoryBarrier barrierC{ };
        barrierC.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierC.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierC.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrierC.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrierC.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrierC.image                           = m_Image;
        barrierC.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrierC.subresourceRange.baseMipLevel   = 0;
        barrierC.subresourceRange.levelCount     = 1;
        barrierC.subresourceRange.baseArrayLayer = 0;
        barrierC.subresourceRange.layerCount     = 1;
        barrierC.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrierC.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;

        VkPipelineStageFlags sourceStageC;
        sourceStageC = VK_PIPELINE_STAGE_TRANSFER_BIT;

        VkPipelineStageFlags destinationStageC;
        destinationStageC = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        vkCmdPipelineBarrier(
            commandBufferC,
            sourceStageC, destinationStageC,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrierC
        );

        vkEndCommandBuffer(commandBufferC);

        VkSubmitInfo submitInfoC{ };
        submitInfoC.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfoC.commandBufferCount = 1;
        submitInfoC.pCommandBuffers    = &commandBufferC;

        vkQueueSubmit(m_DevicePair->GraphicsQueueFamily.Handle, 1, &submitInfoC, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_DevicePair->GraphicsQueueFamily.Handle);

        vkFreeCommandBuffers(m_DevicePair->LogicalDevice, m_DevicePair->GraphicsCommandPool, 1, &commandBufferC);

        stagingBuffer.CleanUp();
    }

    void VulkanTexture::DestroyImage()
    {
        if (m_Image != VK_NULL_HANDLE)
        {
            vkDestroyImage(m_DevicePair->LogicalDevice, m_Image, m_Allocator);
            m_Image = VK_NULL_HANDLE;
        }
        if (m_ImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_DevicePair->LogicalDevice, m_ImageMemory, m_Allocator);
            m_ImageMemory = VK_NULL_HANDLE;
        }
    }

    void VulkanTexture::CreateImageView()
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Device pair is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Logical device is null")
        OTR_INTERNAL_ASSERT_MSG(m_Image != VK_NULL_HANDLE, "Image is null")

        VkImageViewCreateInfo viewInfo{ };
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = m_Image;
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        OTR_VULKAN_VALIDATE(vkCreateImageView(m_DevicePair->LogicalDevice, &viewInfo, m_Allocator, &m_ImageView))
    }

    void VulkanTexture::DestroyImageView()
    {
        if (m_ImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_DevicePair->LogicalDevice, m_ImageView, m_Allocator);
            m_ImageView = VK_NULL_HANDLE;
        }
    }

    void VulkanTexture::CreateSampler()
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair != nullptr, "Device pair is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->PhysicalDevice != VK_NULL_HANDLE, "Physical device is null")
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair->LogicalDevice != VK_NULL_HANDLE, "Logical device is null")

        VkPhysicalDeviceProperties properties{ };
        vkGetPhysicalDeviceProperties(m_DevicePair->PhysicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{ };
        samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter               = VK_FILTER_LINEAR;
        samplerInfo.minFilter               = VK_FILTER_LINEAR;
        samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable        = VK_TRUE;
        samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable           = VK_FALSE;
        samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        OTR_VULKAN_VALIDATE(vkCreateSampler(m_DevicePair->LogicalDevice, &samplerInfo, m_Allocator, &m_Sampler))
    }

    void VulkanTexture::DestroySampler()
    {
        if (m_Sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(m_DevicePair->LogicalDevice, m_Sampler, m_Allocator);
            m_Sampler = VK_NULL_HANDLE;
        }
    }
}