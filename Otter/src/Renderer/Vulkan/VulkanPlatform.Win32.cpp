#include "Otter.PCH.h"

#if OTR_PLATFORM_WINDOWS

#include "Renderer/Vulkan/VulkanBase.h"
#include "Renderer/Vulkan/VulkanPlatform.h"

namespace Otter::Renderer
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions, List<const char*>& layers)
    {
        extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    }

    void GetRequiredDeviceExtensions(List<const char*>& extensions, List<const char*>& layers)
    {
        extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
}

#endif