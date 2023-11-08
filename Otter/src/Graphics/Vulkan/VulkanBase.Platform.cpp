#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanBase.Includes.h"
#include "Graphics/Vulkan/VulkanBase.Platform.h"

namespace Otter::Graphics::Vulkan
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions)
    {
        extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);

#if OTR_PLATFORM_WINDOWS
        extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif OTR_PLATFORM_APPLE
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
                    OTR_LOG_TRACE("Required extension found: {0}", extensions[i]);
                    break;
                }
            }

            OTR_INTERNAL_ASSERT_MSG(found, "Required extension is missing: {0}", extensions[i])
        }

        Buffer::Delete(availableExtensions, availableExtensionCount);
    }

    void GetRequiredInstanceValidationLayers(List<const char*>& layers)
    {
#if !OTR_RUNTIME
        layers.Add("VK_LAYER_KHRONOS_validation");

        UInt32 availableLayerCount = 0;
        OTR_VULKAN_VALIDATE(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr))
        auto* availableLayers = Buffer::New<VkLayerProperties>(availableLayerCount);
        OTR_VULKAN_VALIDATE(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers))

        for (UInt32 i = 0; i < layers.GetCount(); ++i)
        {
            bool found = false;

            for (UInt32 j = 0; j < availableLayerCount; ++j)
            {
                if (strcmp(layers[i], availableLayers[j].layerName) == 0)
                {
                    found = true;
                    OTR_LOG_TRACE("Required validation layer found: {0}", layers[i]);
                    break;
                }
            }

            OTR_INTERNAL_ASSERT_MSG(found, "Required layer is missing: {0}", layers[i])
        }

        Buffer::Delete(availableLayers, availableLayerCount);
#endif
    }
}

