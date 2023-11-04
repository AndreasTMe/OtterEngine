#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanSurface.h"

namespace Otter::Graphics::Vulkan
{
#if OTR_PLATFORM_WINDOWS

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

#else

    void CreateSurface(VulkanContext* vulkanContext, const void* const platformContext)
    {
        OTR_LOG_FATAL("'CreateSurface' not supported for this platform")
    }

#endif
}