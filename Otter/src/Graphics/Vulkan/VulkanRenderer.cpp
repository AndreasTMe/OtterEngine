#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanRenderer.h"
#include "Graphics/Vulkan/VulkanSurface.h"
#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanSwapchain.h"
#include "Graphics/Vulkan/VulkanCommands.h"

namespace Otter::Graphics::Vulkan
{
    void VulkanRenderer::Initialise(const void* const platformContext)
    {
        m_Context = New<VulkanContext>();

        CreateVulkanInstance();

#if !OTR_RUNTIME
        CreateVulkanDebugMessenger();
#endif

        CreateSurface(m_Context, platformContext, m_Context->m_Surface);
        CreateDevicePairs(m_Context, m_Context->m_DevicePair);
        CreateSwapchain(m_Context, m_Context->m_Swapchain);
        CreateRenderPass();

        CreateCommandBuffers(m_Context,
                             m_Context->m_DevicePair.m_GraphicsCommandPool,
                             m_Context->m_DevicePair.m_CommandBuffers);
    }

    void VulkanRenderer::Shutdown()
    {
        vkDeviceWaitIdle(m_Context->m_DevicePair.m_LogicalDevice);

        DestroyCommandBuffers(m_Context, m_Context->m_DevicePair.m_CommandBuffers);

        DestroyRenderPass();
        DestroySwapchain(m_Context);
        DestroyDevicePairs(m_Context);
        DestroySurface(m_Context);

#if !OTR_RUNTIME
        DestroyVulkanDebugMessenger();
#endif

        DestroyVulkanInstance();

        Delete(m_Context);
    }

    void VulkanRenderer::RenderFrame()
    {
    }

    void VulkanRenderer::CreateVulkanInstance()
    {
        OTR_LOG_TRACE("Creating Vulkan instance...")

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

        OTR_LOG_TRACE("Getting required instance extensions and layers...")

        List<const char*> extensions;
        GetRequiredInstanceExtensions(extensions);

        createInfo.enabledExtensionCount   = extensions.GetCount();
        createInfo.ppEnabledExtensionNames = extensions.GetData();

#if !OTR_RUNTIME
        OTR_LOG_TRACE("Creating Vulkan debug messenger...")

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
        PopulateDebugMessengerCreateInfo(debugCreateInfo);

        List<const char*> layers;
        GetRequiredInstanceValidationLayers(layers);

        createInfo.enabledLayerCount   = layers.GetCount();
        createInfo.ppEnabledLayerNames = layers.GetData();
        createInfo.pNext               = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#else
        createInfo.enabledLayerCount   = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext               = nullptr;
#endif

        OTR_VULKAN_VALIDATE(vkCreateInstance(&createInfo, m_Context->m_Allocator, &m_Context->m_Instance))
    }

    void VulkanRenderer::DestroyVulkanInstance()
    {
        vkDestroyInstance(m_Context->m_Instance, m_Context->m_Allocator);
    }

    void VulkanRenderer::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{ };
        colorAttachment.format         = m_Context->m_Swapchain.m_SurfaceFormat.format;
        colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{ };
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subPass{ };
        subPass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.colorAttachmentCount = 1;
        subPass.pColorAttachments    = &colorAttachmentRef;

        VkSubpassDependency dependency{ };
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{ };
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments    = &colorAttachment;
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subPass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        OTR_VULKAN_VALIDATE(vkCreateRenderPass(m_Context->m_DevicePair.m_LogicalDevice,
                                               &renderPassInfo,
                                               m_Context->m_Allocator,
                                               &m_Context->m_RenderPass))
    }

    void VulkanRenderer::DestroyRenderPass()
    {
        vkDestroyRenderPass(m_Context->m_DevicePair.m_LogicalDevice, m_Context->m_RenderPass, m_Context->m_Allocator);
    }

    void VulkanRenderer::GetRequiredInstanceExtensions(List<const char*>& extensions)
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
                    OTR_LOG_TRACE("Required extension found: {0}", extensions[i]);
                    break;
                }
            }

            OTR_INTERNAL_ASSERT_MSG(found, "Required extension is missing: {0}", extensions[i])
        }

        Buffer::Delete(availableExtensions, availableExtensionCount);
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

    void VulkanRenderer::GetRequiredInstanceValidationLayers(List<const char*>& layers)
    {
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
    }
#endif
}