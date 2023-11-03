#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanRenderer.h"
#include "Graphics/Vulkan/VulkanPlatform.h"

namespace Otter::Graphics::Vulkan
{
#if !OTR_RELEASE
    VkDebugUtilsMessengerEXT g_DebugMessenger = VK_NULL_HANDLE;

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                 void* userData)
    {
        // TODO: Also use the message type to filter out messages
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                OTR_LOG_DEBUG(callbackData->pMessage)
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                OTR_LOG_INFO(callbackData->pMessage)
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                OTR_LOG_WARNING(callbackData->pMessage)
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                OTR_LOG_ERROR(callbackData->pMessage)
                break;
            default:
                OTR_LOG_FATAL("Unknown Error: {0}", callbackData->pMessage)
                break;
        }

        return VK_FALSE;
    }

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = { };
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
//            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType     =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
    }
#endif

    bool VulkanRenderer::TryInitialise(const void* windowHandle)
    {
#if !OTR_RELEASE
//        if (!CheckValidationLayerSupport())
//            throw std::runtime_error("Validation layers requested, but not available!");
#endif

        VkApplicationInfo appInfo{ };
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "Otter Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "Otter Engine";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{ };
        createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        List<const char*> extensions;
        List<const char*> layers;
        GetRequiredInstanceExtensions(extensions, layers);
        createInfo.enabledExtensionCount   = extensions.GetCount();
        createInfo.ppEnabledExtensionNames = extensions.GetData();

#if !OTR_RELEASE
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
        createInfo.enabledLayerCount   = layers.GetCount();
        createInfo.ppEnabledLayerNames = layers.GetData();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
        {
            OTR_LOG_FATAL("Failed to create Vulkan instance")
            return false;
        }

        return true;
    }

    void VulkanRenderer::Shutdown()
    {
#if !OTR_RELEASE
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_Instance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(m_Instance, g_DebugMessenger, nullptr);
#endif

        vkDestroyInstance(m_Instance, nullptr);
    }
}