#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanSwapchain.h"
#include "Math/Core.h"

namespace Otter::Graphics::Vulkan
{
    List <VkImage>       g_SwapchainImages;
    List <VkImageView>   g_SwapchainImageViews;
    List <VkFramebuffer> g_SwapchainFrameBuffers;

    void CreateSwapchainInternal(const VkSurfaceKHR& surface,
                                 const VulkanDevicePair& devicePair,
                                 const VkAllocationCallbacks* allocator,
                                 VulkanSwapchain& swapchain);
    void CreateSwapchainImages(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain, UInt32& imageCount);
    void CreateSwapchainImageViews(const VkDevice& logicalDevice, const VkFormat& imageFormat);
    VkSurfaceFormatKHR SelectSwapchainSurfaceFormat(const List <VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR SelectSwapchainPresentMode(const List <VkPresentModeKHR>& presentModes);
    VkExtent2D SelectSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CreateSwapchain(const VulkanContext* const vulkanContext, VulkanSwapchain& swapchain)
    {
        OTR_LOG_TRACE("Creating Vulkan swapchain...")

        CreateSwapchainInternal(vulkanContext->m_Surface,
                                vulkanContext->m_DevicePair,
                                vulkanContext->m_Allocator,
                                swapchain);
        CreateSwapchainImages(vulkanContext->m_DevicePair.m_LogicalDevice, swapchain.m_Handle, swapchain.m_ImageCount);
        CreateSwapchainImageViews(vulkanContext->m_DevicePair.m_LogicalDevice, swapchain.m_SurfaceFormat.format);
    }

    void RecreateSwapchain(const VulkanContext* const vulkanContext, VulkanSwapchain& swapchain)
    {
        OTR_LOG_TRACE("Recreating Vulkan swapchain...")

        DestroySwapchain(vulkanContext);

        CreateSwapchainInternal(vulkanContext->m_Surface,
                                vulkanContext->m_DevicePair,
                                vulkanContext->m_Allocator,
                                swapchain);
        CreateSwapchainImages(vulkanContext->m_DevicePair.m_LogicalDevice, swapchain.m_Handle, swapchain.m_ImageCount);
        CreateSwapchainImageViews(vulkanContext->m_DevicePair.m_LogicalDevice, swapchain.m_SurfaceFormat.format);
    }

    void DestroySwapchain(const VulkanContext* const vulkanContext)
    {
        OTR_INTERNAL_ASSERT_MSG(vulkanContext->m_DevicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before destroying its swapchain")

        for (auto& swapchainFrameBuffer: g_SwapchainFrameBuffers)
        {
            vkDestroyFramebuffer(vulkanContext->m_DevicePair.m_LogicalDevice,
                                 swapchainFrameBuffer,
                                 vulkanContext->m_Allocator);
        }
        g_SwapchainFrameBuffers.ClearDestructive();

        for (auto& swapchainImageView: g_SwapchainImageViews)
        {
            vkDestroyImageView(vulkanContext->m_DevicePair.m_LogicalDevice,
                               swapchainImageView,
                               vulkanContext->m_Allocator);
        }
        g_SwapchainImageViews.ClearDestructive();
        g_SwapchainImages.ClearDestructive();

        vkDestroySwapchainKHR(vulkanContext->m_DevicePair.m_LogicalDevice,
                              vulkanContext->m_Swapchain.m_Handle,
                              vulkanContext->m_Allocator);
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
                                                                      &swapchainSupportInfo.m_SurfaceCapabilities))

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
                                                 swapchainSupportInfo.m_SurfaceFormats);
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
                                               swapchainSupportInfo.m_PresentModes);
        }
    }

    void CreateSwapchainInternal(const VkSurfaceKHR& surface,
                                 const VulkanDevicePair& devicePair,
                                 const VkAllocationCallbacks* const allocator,
                                 VulkanSwapchain& swapchain)
    {
        OTR_INTERNAL_ASSERT_MSG(surface != VK_NULL_HANDLE,
                                "Surface must be initialized before creating its swapchain")
        OTR_INTERNAL_ASSERT_MSG(devicePair.m_PhysicalDevice != VK_NULL_HANDLE,
                                "Physical device must be initialized before creating its swapchain")
        OTR_INTERNAL_ASSERT_MSG(devicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before creating its swapchain")

        SwapchainSupportInfo swapchainSupportInfo;
        QuerySwapchainSupport(surface, devicePair.m_PhysicalDevice, swapchainSupportInfo);

        swapchain.m_SurfaceFormat = SelectSwapchainSurfaceFormat(swapchainSupportInfo.m_SurfaceFormats);
        swapchain.m_PresentMode   = SelectSwapchainPresentMode(swapchainSupportInfo.m_PresentModes);
        swapchain.m_Extent        = SelectSwapchainExtent(swapchainSupportInfo.m_SurfaceCapabilities);
        swapchain.m_ImageCount    = swapchainSupportInfo.m_SurfaceCapabilities.minImageCount + 1;

        if (swapchainSupportInfo.m_SurfaceCapabilities.maxImageCount > 0
            && swapchain.m_ImageCount > swapchainSupportInfo.m_SurfaceCapabilities.maxImageCount)
            swapchain.m_ImageCount = swapchainSupportInfo.m_SurfaceCapabilities.maxImageCount;

        swapchain.m_MaxFramesInFlight = swapchain.m_ImageCount - 1;

        VkSwapchainCreateInfoKHR createInfo{ };
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount    = swapchain.m_ImageCount;
        createInfo.imageFormat      = swapchain.m_SurfaceFormat.format;
        createInfo.imageColorSpace  = swapchain.m_SurfaceFormat.colorSpace;
        createInfo.imageExtent      = swapchain.m_Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (!devicePair.GraphicsAndPresentationQueueFamiliesAreTheSame())
        {
            ReadOnlySpan<UInt32, 2> queueFamilyIndices{ devicePair.m_GraphicsQueueFamily.m_Index,
                                                        devicePair.m_PresentationQueueFamily.m_Index };

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

        createInfo.preTransform   = swapchainSupportInfo.m_SurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = swapchain.m_PresentMode;
        createInfo.clipped        = VK_TRUE;
        createInfo.oldSwapchain   = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreateSwapchainKHR(devicePair.m_LogicalDevice,
                                                 &createInfo,
                                                 allocator,
                                                 &swapchain.m_Handle))
    }

    void CreateSwapchainImages(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain, UInt32& imageCount)
    {
        OTR_VULKAN_VALIDATE(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr))

        VkImage swapchainImages[imageCount];
        OTR_VULKAN_VALIDATE(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages))

        Collections::New<VkImage>(swapchainImages, imageCount, g_SwapchainImages);
    }

    void CreateSwapchainImageViews(const VkDevice& logicalDevice, const VkFormat& imageFormat)
    {
        VkImageView swapchainImageViews[g_SwapchainImages.GetCount()];

        UInt64 index = 0;
        for (const auto& swapchainImage: g_SwapchainImages)
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

            OTR_VULKAN_VALIDATE(vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapchainImageViews[index++]))
        }

        Collections::New<VkImageView>(swapchainImageViews, g_SwapchainImages.GetCount(), g_SwapchainImageViews);
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
        OTR_INTERNAL_ASSERT_MSG(capabilities.currentExtent.width != Math::PositiveInfinity<UInt32>,
                                "Current extent width must be initialized before selecting swap extent")

        VkExtent2D swapchainExtent = capabilities.currentExtent;
        swapchainExtent.height = Math::Clamp(swapchainExtent.height,
                                             capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);
        swapchainExtent.width  = Math::Clamp(swapchainExtent.width,
                                             capabilities.minImageExtent.width,
                                             capabilities.maxImageExtent.width);

        return swapchainExtent;
    }
}