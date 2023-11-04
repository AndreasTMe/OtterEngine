#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanRenderer.h"
#include "Graphics/Vulkan/VulkanPlatform.h"

namespace Otter::Graphics::Vulkan
{
    void VulkanRenderer::Initialise(const void* platformContext)
    {
        m_Context = New<VulkanContext>();

        CreateVulkanInstance();

#if !OTR_RUNTIME
        CreateVulkanDebugMessenger();
#endif

        CreateSurface(m_Context, platformContext);
    }

    void VulkanRenderer::Shutdown()
    {
#if !OTR_RUNTIME
        DestroyVulkanDebugMessenger();
#endif

        vkDestroySurfaceKHR(m_Context->m_Instance, m_Context->m_Surface, m_Context->m_Allocator);
        vkDestroyInstance(m_Context->m_Instance, m_Context->m_Allocator);

        Delete(m_Context);
    }

    void VulkanRenderer::CreateVulkanInstance()
    {
//        if (!CheckValidationLayerSupport())
//            throw std::runtime_error("Validation layers requested, but not available!");

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

#if !OTR_RUNTIME
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
        PopulateDebugMessengerCreateInfo(debugCreateInfo);

        createInfo.enabledLayerCount   = layers.GetCount();
        createInfo.ppEnabledLayerNames = layers.GetData();
        createInfo.pNext               = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#else
        createInfo.enabledLayerCount       = 0;
        createInfo.ppEnabledLayerNames     = nullptr;
        createInfo.pNext                   = nullptr;
#endif

        OTR_VULKAN_VALIDATE(vkCreateInstance(&createInfo, m_Context->m_Allocator, &m_Context->m_Instance))
    }

#if !OTR_RUNTIME
    void VulkanRenderer::CreateVulkanDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            m_Context->m_Instance,
            "vkCreateDebugUtilsMessengerEXT");

        if (createDebugUtilsMessenger)
        {
            OTR_VULKAN_VALIDATE(createDebugUtilsMessenger(m_Context->m_Instance,
                                                          &createInfo, m_Context->m_Allocator,
                                                          &m_Context->m_DebugMessenger))
        }
    }

    void VulkanRenderer::DestroyVulkanDebugMessenger()
    {
        auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            m_Context->m_Instance,
            "vkDestroyDebugUtilsMessengerEXT");

        if (destroyDebugUtilsMessenger != nullptr)
            destroyDebugUtilsMessenger(m_Context->m_Instance, m_Context->m_DebugMessenger, m_Context->m_Allocator);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

    void VulkanRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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
}