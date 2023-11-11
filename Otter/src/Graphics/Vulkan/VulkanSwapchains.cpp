#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanSwapchains.h"
#include "Graphics/Vulkan/VulkanBase.Includes.h"
#include "Math/Core.h"

namespace Otter::Graphics::Vulkan
{
    void CreateSingleSwapchain(const VkSurfaceKHR& surface,
                               const VulkanDevicePair& devicePair,
                               const VkAllocationCallbacks* const allocator,
                               VulkanSwapchain& swapchain)
    {
        OTR_INTERNAL_ASSERT_MSG(surface != VK_NULL_HANDLE,
                                "Surface must be initialized before creating its swapchain")
        OTR_INTERNAL_ASSERT_MSG(devicePair.PhysicalDevice != VK_NULL_HANDLE,
                                "Physical device must be initialized before creating its swapchain")
        OTR_INTERNAL_ASSERT_MSG(devicePair.LogicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before creating its swapchain")

        SwapchainSupportInfo swapchainSupportInfo;
        QuerySwapchainSupport(surface, devicePair.PhysicalDevice, swapchainSupportInfo);

        swapchain.Extent        = SelectSwapchainExtent(swapchainSupportInfo.SurfaceCapabilities);
        swapchain.SurfaceFormat = SelectSwapchainSurfaceFormat(swapchainSupportInfo.SurfaceFormats);
        swapchain.PresentMode   = SelectSwapchainPresentMode(swapchainSupportInfo.PresentModes);

        UInt32 imageCount = swapchainSupportInfo.SurfaceCapabilities.minImageCount + 1;
        if (swapchainSupportInfo.SurfaceCapabilities.maxImageCount > 0
            && imageCount > swapchainSupportInfo.SurfaceCapabilities.maxImageCount)
            imageCount = swapchainSupportInfo.SurfaceCapabilities.maxImageCount;

        swapchain.MaxFramesInFlight = imageCount - 1;

        VkSwapchainCreateInfoKHR createInfo{ };
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = swapchain.SurfaceFormat.format;
        createInfo.imageColorSpace  = swapchain.SurfaceFormat.colorSpace;
        createInfo.imageExtent      = swapchain.Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (!devicePair.GraphicsAndPresentationQueueFamiliesAreTheSame())
        {
            ReadOnlySpan<UInt32, 2> queueFamilyIndices{ devicePair.GraphicsQueueFamily.Index,
                                                        devicePair.PresentationQueueFamily.Index };

            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices.GetData();
        }
        else
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices   = nullptr;
        }

        createInfo.preTransform   = swapchainSupportInfo.SurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = swapchain.PresentMode;
        createInfo.clipped        = VK_TRUE;
        createInfo.oldSwapchain   = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreateSwapchainKHR(devicePair.LogicalDevice,
                                                 &createInfo,
                                                 allocator,
                                                 &swapchain.Handle))
    }

    void QuerySwapchainSupport(const VkSurfaceKHR& surface,
                               const VkPhysicalDevice& physicalDevice,
                               SwapchainSupportInfo& swapchainSupportInfo)
    {
        OTR_INTERNAL_ASSERT_MSG(surface != VK_NULL_HANDLE,
                                "Surface must be initialized before querying swapchain support")
        OTR_INTERNAL_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE,
                                "Physical device must be initialized before querying swapchain support")

        OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,
                                                                      surface,
                                                                      &swapchainSupportInfo.SurfaceCapabilities))

        UInt32 surfaceFormatCount;
        OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                                                 surface,
                                                                 &surfaceFormatCount,
                                                                 nullptr))

        if (surfaceFormatCount != 0)
        {
            VkSurfaceFormatKHR surfaceFormats[surfaceFormatCount];
            OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfaceFormatsKHR(
                physicalDevice,
                surface,
                &surfaceFormatCount,
                surfaceFormats))

            Collections::New<VkSurfaceFormatKHR>(surfaceFormats,
                                                 surfaceFormatCount,
                                                 swapchainSupportInfo.SurfaceFormats);
        }

        UInt32 presentModeCount;
        OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                                                      surface,
                                                                      &presentModeCount,
                                                                      nullptr))

        if (presentModeCount != 0)
        {
            VkPresentModeKHR presentModes[presentModeCount];
            OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfacePresentModesKHR(
                physicalDevice,
                surface,
                &presentModeCount,
                presentModes))

            Collections::New<VkPresentModeKHR>(presentModes,
                                               presentModeCount,
                                               swapchainSupportInfo.PresentModes);
        }
    }

    VkSurfaceFormatKHR SelectSwapchainSurfaceFormat(const List <VkSurfaceFormatKHR>& surfaceFormats)
    {
        for (const auto& surfaceFormat: surfaceFormats)
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return surfaceFormat;

        return surfaceFormats[0];
    }

    VkPresentModeKHR SelectSwapchainPresentMode(const List <VkPresentModeKHR>& presentModes)
    {
        for (const auto& presentMode: presentModes)
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return presentMode;

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SelectSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        VkExtent2D swapchainExtent = capabilities.currentExtent;
        swapchainExtent.height = Math::Clamp(swapchainExtent.height,
                                             capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);
        swapchainExtent.width  = Math::Clamp(swapchainExtent.width,
                                             capabilities.minImageExtent.width,
                                             capabilities.maxImageExtent.width);

        return swapchainExtent;
    }

    void CreateSwapchainImages(const VkDevice& logicalDevice,
                               const VkSwapchainKHR& swapchain,
                               List <VkImage>& swapchainImages)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before creating swapchain images")
        OTR_INTERNAL_ASSERT_MSG(swapchain != VK_NULL_HANDLE,
                                "Swapchain must be initialized before creating its images")

        UInt32 imageCount = 0;
        OTR_VULKAN_VALIDATE(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr))

        VkImage tempSwapchainImages[imageCount];
        OTR_VULKAN_VALIDATE(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, tempSwapchainImages))

        Collections::New<VkImage>(tempSwapchainImages, imageCount, swapchainImages);
    }

    void CreateSwapchainImageViews(const VkDevice& logicalDevice,
                                   const VkAllocationCallbacks* const allocator,
                                   const List <VkImage>& swapchainImages,
                                   const VkFormat& imageFormat,
                                   List <VkImageView>& swapchainImageViews)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before creating swapchain image views")

        VkImageView tempSwapchainImageViews[swapchainImages.GetCount()];

        UInt64 index = 0;
        for (const auto& swapchainImage: swapchainImages)
        {
            VkImageViewCreateInfo createInfo{ };
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = swapchainImage;
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = imageFormat;
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            OTR_VULKAN_VALIDATE(vkCreateImageView(logicalDevice,
                                                  &createInfo,
                                                  allocator,
                                                  &tempSwapchainImageViews[index++]))
        }

        Collections::New<VkImageView>(tempSwapchainImageViews, swapchainImages.GetCount(), swapchainImageViews);
    }

    void CreateSwapchainFrameBuffers(const VkDevice& logicalDevice,
                                     const VkAllocationCallbacks* const allocator,
                                     const VkExtent2D& swapChainExtent,
                                     const List <VkImageView>& swapchainImageViews,
                                     const VkRenderPass& renderPass,
                                     List <VkFramebuffer>& swapchainFrameBuffers)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before creating swapchain image views")
        OTR_INTERNAL_ASSERT_MSG(renderPass != VK_NULL_HANDLE,
                                "Render pass must be initialized before creating swapchain frame buffers")

        VkFramebuffer tempSwapchainFrameBuffers[swapchainImageViews.GetCount()];

        for (size_t i = 0; i < swapchainImageViews.GetCount(); i++)
        {
            VkFramebufferCreateInfo framebufferInfo{ };
            framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass      = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments    = &swapchainImageViews[i];
            framebufferInfo.width           = swapChainExtent.width;
            framebufferInfo.height          = swapChainExtent.height;
            framebufferInfo.layers          = 1;
            framebufferInfo.pNext           = nullptr;

            OTR_VULKAN_VALIDATE(vkCreateFramebuffer(logicalDevice,
                                                    &framebufferInfo,
                                                    allocator,
                                                    &tempSwapchainFrameBuffers[i]))
        }

        Collections::New<VkFramebuffer>(tempSwapchainFrameBuffers,
                                        swapchainImageViews.GetCount(),
                                        swapchainFrameBuffers);
    }
}