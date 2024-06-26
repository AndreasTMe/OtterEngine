#ifndef OTTERENGINE_VULKANBASE_INCLUDES_H
#define OTTERENGINE_VULKANBASE_INCLUDES_H

#include "Core/Assert.h"

#include <vulkan/vulkan.h>

#if OTR_PLATFORM_WINDOWS
    #include <vulkan/vulkan_win32.h>
    #include "Platform/Windows/Platform.Win32.h"
#elif OTR_PLATFORM_IOS
    #include <vulkan/vulkan_ios.h>
#elif OTR_PLATFORM_MACOS
    #include <vulkan/vulkan_macos.h>
#elif OTR_PLATFORM_LINUX
    #include <vulkan/vulkan_xlib.h>
    #include "Platform/Windows/Platform.Linux.h"
#elif OTR_PLATFORM_ANDROID
    #include <vulkan/vulkan_android.h>
#else
    #error "Unsupported platform!"
#endif

#define OTR_VULKAN_VALIDATE(vkCall)                                                     \
    {                                                                                   \
        VkResult result = vkCall;                                                       \
        OTR_INTERNAL_ASSERT_MSG(result == VK_SUCCESS,                                   \
                                "'{0}' failed with error code: {1}", #vkCall, result)   \
    }

#define OTR_VULKAN_BUFFERING_COUNT 3

#endif //OTTERENGINE_VULKANBASE_INCLUDES_H
