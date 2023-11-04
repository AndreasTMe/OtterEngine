#include "Otter.PCH.h"

#if OTR_PLATFORM_WINDOWS

#include "Graphics/Vulkan/VulkanBase.h"
#include "Graphics/Vulkan/VulkanPlatform.h"

namespace Otter::Graphics::Vulkan
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions, List<const char*>& layers)
    {
        extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

        layers.Add("VK_LAYER_KHRONOS_validation");
    }

    void GetRequiredDeviceExtensions(List<const char*>& extensions, List<const char*>& layers)
    {
        extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    void CreateSurface(VulkanContext* vulkanContext, const void* const platformContext)
    {
        const auto* const platformContextData       = ((PlatformContext*) platformContext)->m_Data;
        const auto* const windowsPlatformWindowData = (Internal::WindowsPlatformWindowData*) platformContextData;

        VkWin32SurfaceCreateInfoKHR createInfo = { };
        createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd      = windowsPlatformWindowData->m_WindowHandle;
        createInfo.hinstance = windowsPlatformWindowData->m_InstanceHandle;

        OTR_VULKAN_VALIDATE(vkCreateWin32SurfaceKHR(vulkanContext->m_Instance,
                                                    &createInfo,
                                                    vulkanContext->m_Allocator,
                                                    &vulkanContext->m_Surface))
    }
}

#endif