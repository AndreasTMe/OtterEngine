#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanExtensions.h"

namespace Otter::Graphics::Vulkan
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions)
    {
        extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);

#if OTR_PLATFORM_WINDOWS
        extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif OTR_PLATFORM_IOS
        extensions.Add(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif OTR_PLATFORM_MACOS
            extensions.Add(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif OTR_PLATFORM_LINUX
            extensions.Add(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif OTR_PLATFORM_ANDROID
            extensions.Add(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

#if !OTR_RUNTIME
        extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        UInt32 availableExtensionCount = 0;
        OTR_VULKAN_VALIDATE(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr))
        auto* availableExtensions = Buffer::New<VkExtensionProperties>(availableExtensionCount);
        OTR_VULKAN_VALIDATE(vkEnumerateInstanceExtensionProperties(nullptr,
                                                                   &availableExtensionCount,
                                                                   availableExtensions))

        for (UInt32 i = 0; i < extensions.GetCount(); ++i)
        {
            bool found = false;

            for (UInt32 j = 0; j < availableExtensionCount; ++j)
            {
                if (strcmp(extensions[i], availableExtensions[j].extensionName) == 0)
                {
                    found = true;
                    OTR_LOG_TRACE("Required extension found: {0}", extensions[i])
                    break;
                }
            }

            OTR_INTERNAL_ASSERT_MSG(found, "Required extension is missing: {0}", extensions[i])
        }

        Buffer::Delete(availableExtensions, availableExtensionCount);
    }

    void GetDeviceRequiredExtensions(List<const char*>& requiredExtensions)
    {
        requiredExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    bool DeviceSupportsRequiredExtensions(const VkPhysicalDevice& physicalDevice)
    {
        OTR_INTERNAL_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE,
                                "Physical device must be initialized before checking for extensions")

        List<const char*> requiredExtensions;
        GetDeviceRequiredExtensions(requiredExtensions);

        UInt32 extensionCount;
        OTR_VULKAN_VALIDATE(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr))

        VkExtensionProperties availableExtensions[extensionCount];
        OTR_VULKAN_VALIDATE(vkEnumerateDeviceExtensionProperties(physicalDevice,
                                                                 nullptr,
                                                                 &extensionCount,
                                                                 availableExtensions))

        bool foundRequiredExtensions = false;
        for (const auto& extension: requiredExtensions)
        {
            for (const auto& availableExtension: availableExtensions)
            {
                if (strcmp(extension, availableExtension.extensionName) == 0)
                {
                    foundRequiredExtensions = true;
                    break;
                }
            }
        }

        return foundRequiredExtensions;
    }
}