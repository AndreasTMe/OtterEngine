#include "Otter.PCH.h"

#if OTR_PLATFORM_WINDOWS

#include "Graphics/Vulkan/VulkanBase.h"
#include "Graphics/Vulkan/VulkanPlatform.h"

namespace Otter::Graphics::Vulkan
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions, List<const char*>& layers)
    {
        extensions.Reserve(2);

        extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    }

    void GetRequiredDeviceExtensions(List<const char*>& extensions, List<const char*>& layers)
    {
        extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
}

#endif