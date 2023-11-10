#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanRenderer.h"
#include "Graphics/Vulkan/VulkanBase.Types.h"
#include "Graphics/Vulkan/VulkanExtensions.h"
#include "Graphics/Vulkan/VulkanQueues.h"
#include "Graphics/Vulkan/VulkanSwapchains.h"

namespace Otter::Graphics::Vulkan
{
    namespace Internal
    {
        // HELP: VkInstance related
        static void CreateVulkanInstance();
        static void DestroyVulkanInstance();

        // HELP: VkSurfaceKHR related
        static void CreateSurface(const void* platformContext);
        static void DestroySurface();

        // HELP: VkPhysicalDevice & VkDevice related
        static void CreateDevicePairs();
        static void DestroyDevicePairs();
        static void SelectPhysicalDevice();
        static void CreateLogicalDevice();

        // HELP: VkSwapchainKHR related
        static void CreateSwapchains();
        static void RecreateSwapchains();
        static void DestroySwapchains();

        // HELP: VkRenderPass related
        static void CreateRenderPass();
        static void DestroyRenderPass();

        // HELP: VkCommandPool & VkCommandBuffer related
        static void CreateCommandBuffers();
        static void DestroyCommandBuffers();

#if !OTR_RUNTIME
        // HELP: VkDebugUtilsMessenger related
        static void CreateVulkanDebugMessenger();
        static void DestroyVulkanDebugMessenger();

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                               [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                               const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                               void* userData);

        void GetRequiredInstanceValidationLayers(List<const char*>& layers);
#endif
    }

    static VulkanContext* gs_Context = nullptr;

    List <VkImage>       g_SwapchainImages;
    List <VkImageView>   g_SwapchainImageViews;
    List <VkFramebuffer> g_SwapchainFrameBuffers;

    void Initialise(const void* const platformContext)
    {
        gs_Context = New<VulkanContext>();

        Internal::CreateVulkanInstance();

#if !OTR_RUNTIME
        Internal::CreateVulkanDebugMessenger();
#endif

        Internal::CreateSurface(platformContext);
        Internal::CreateDevicePairs();
        Internal::CreateSwapchains();
        Internal::CreateRenderPass();
        Internal::CreateCommandBuffers();
    }

    void Shutdown()
    {
        vkDeviceWaitIdle(gs_Context->DevicePair.LogicalDevice);

        Internal::DestroyCommandBuffers();
        Internal::DestroyRenderPass();
        Internal::DestroySwapchains();
        Internal::DestroyDevicePairs();
        Internal::DestroySurface();

#if !OTR_RUNTIME
        Internal::DestroyVulkanDebugMessenger();
#endif

        Internal::DestroyVulkanInstance();

        Delete(gs_Context);
    }

    void RenderFrame()
    {
    }

    namespace Internal
    {
        void CreateVulkanInstance()
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

            OTR_VULKAN_VALIDATE(vkCreateInstance(&createInfo, gs_Context->Allocator, &gs_Context->Instance))
        }

        void DestroyVulkanInstance()
        {
            vkDestroyInstance(gs_Context->Instance, gs_Context->Allocator);
        }

        void CreateSurface(const void* const platformContext)
        {
#if OTR_PLATFORM_WINDOWS
            const auto* const platformContextData       = ((PlatformContext*) platformContext)->Data;
            const auto* const windowsPlatformWindowData =
                          (Otter::Internal::WindowsPlatformWindowData*) platformContextData;

            VkWin32SurfaceCreateInfoKHR createInfo = { };
            createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            createInfo.hwnd      = windowsPlatformWindowData->WindowHandle;
            createInfo.hinstance = windowsPlatformWindowData->InstanceHandle;

            OTR_VULKAN_VALIDATE(vkCreateWin32SurfaceKHR(gs_Context->Instance,
                                                        &createInfo,
                                                        gs_Context->Allocator,
                                                        &gs_Context->Surface))
#else
            OTR_LOG_FATAL("'CreateSurface' not supported for this platform")
#endif
        }

        void DestroySurface()
        {
            vkDestroySurfaceKHR(gs_Context->Instance, gs_Context->Surface, gs_Context->Allocator);
        }

        void CreateDevicePairs()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->Instance != VK_NULL_HANDLE,
                                    "Instance must be initialized before creating device pairs")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->Surface != VK_NULL_HANDLE,
                                    "Surface must be initialized before creating device pairs")

            OTR_LOG_TRACE("Creating Vulkan device pairs...")

            SelectPhysicalDevice();
            CreateLogicalDevice();
        }

        void DestroyDevicePairs()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Logical device must be initialized before destroying device pairs")

            vkDestroyDevice(gs_Context->DevicePair.LogicalDevice, gs_Context->Allocator);
        }

        void SelectPhysicalDevice()
        {
            UInt32 deviceCount = 0;
            OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Context->Instance,
                                                           &deviceCount,
                                                           nullptr))

            OTR_INTERNAL_ASSERT_MSG(deviceCount > 0, "Failed to find GPUs with Vulkan support")

            VkPhysicalDevice queriedDevices[deviceCount];
            OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Context->Instance, &deviceCount, queriedDevices))

            for (const auto& queriedDevice: queriedDevices)
            {
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(queriedDevice, &deviceProperties);

                OTR_LOG_TRACE("Checking GPU: {0}", deviceProperties.deviceName)

                // HELP: Require a discrete GPU
                if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    OTR_LOG_TRACE("{0} is not a discrete GPU. Moving to the next one...", deviceProperties.deviceName)
                    continue;
                }

                // HELP: Require a GPU with graphics and presentation queues
                UInt32 graphicsFamily = UINT32_MAX;
                UInt32 presentFamily  = UINT32_MAX;
                QueryQueueFamilies(gs_Context->Surface, queriedDevice, graphicsFamily, presentFamily);

                if (graphicsFamily == UINT32_MAX || presentFamily == UINT32_MAX)
                {
                    OTR_LOG_TRACE("{0} is not suitable due to missing Graphics and/or Presentation queue. "
                                  "Moving to the next one...", deviceProperties.deviceName)

                    continue;
                }

                // HELP: Require a GPU with all required extensions
                if (!DeviceSupportsRequiredExtensions(queriedDevice))
                {
                    OTR_LOG_TRACE("{0} is not suitable due to missing required extensions. "
                                  "Moving to the next one...", deviceProperties.deviceName)
                    continue;
                }

                // HELP: Require a GPU with swapchain support
                SwapchainSupportInfo swapchainSupportInfo;
                QuerySwapchainSupport(gs_Context->Surface, queriedDevice, swapchainSupportInfo);

                bool isSwapchainSupported = swapchainSupportInfo.SurfaceFormats.GetCount() > 0
                                            && swapchainSupportInfo.PresentModes.GetCount() > 0;

                if (!isSwapchainSupported)
                {
                    OTR_LOG_TRACE("{0} is not suitable due to missing swapchain support. "
                                  "Moving to the next one...", deviceProperties.deviceName)
                    continue;
                }

                gs_Context->DevicePair.PhysicalDevice                = queriedDevice;
                gs_Context->DevicePair.GraphicsQueueFamily.Index     = graphicsFamily;
                gs_Context->DevicePair.PresentationQueueFamily.Index = presentFamily;

                OTR_LOG_TRACE("Found a suitable GPU: {0}", deviceProperties.deviceName)

                break;
            }

            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.PhysicalDevice != VK_NULL_HANDLE,
                                    "Failed to find a suitable GPU")
        }

        void CreateLogicalDevice()
        {
            List <VkDeviceQueueCreateInfo> queueCreateInfos;

            HashSet <UInt32> uniqueQueueFamilies(2);
            uniqueQueueFamilies.TryAdd(gs_Context->DevicePair.GraphicsQueueFamily.Index);
            uniqueQueueFamilies.TryAdd(gs_Context->DevicePair.PresentationQueueFamily.Index);

            float queuePriority = 1.0f;

            Action<const UInt32&> addQueueCreateInfo;
            addQueueCreateInfo += [&](UInt32 queueFamily)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{ };
                queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount       = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.Add(queueCreateInfo);
            };
            uniqueQueueFamilies.ForEach(addQueueCreateInfo);
            addQueueCreateInfo.ClearDestructive();

            // TODO: Should be configuration driven, all features are disabled by default for now
            VkPhysicalDeviceFeatures deviceFeatures{ };

            List<const char*> requiredExtensions;
            GetDeviceRequiredExtensions(requiredExtensions);

            VkDeviceCreateInfo createInfo{ };
            createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.queueCreateInfoCount    = queueCreateInfos.GetCount();
            createInfo.pQueueCreateInfos       = queueCreateInfos.GetData();
            createInfo.pEnabledFeatures        = &deviceFeatures;
            createInfo.enabledExtensionCount   = requiredExtensions.GetCount();
            createInfo.ppEnabledExtensionNames = requiredExtensions.GetData();
            createInfo.enabledLayerCount       = 0;
            createInfo.ppEnabledLayerNames     = nullptr;

            OTR_VULKAN_VALIDATE(vkCreateDevice(gs_Context->DevicePair.PhysicalDevice,
                                               &createInfo,
                                               nullptr,
                                               &gs_Context->DevicePair.LogicalDevice))

            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Failed to create logical device")

            vkGetDeviceQueue(gs_Context->DevicePair.LogicalDevice,
                             gs_Context->DevicePair.GraphicsQueueFamily.Index,
                             0,
                             &gs_Context->DevicePair.GraphicsQueueFamily.Handle);
            vkGetDeviceQueue(gs_Context->DevicePair.LogicalDevice,
                             gs_Context->DevicePair.PresentationQueueFamily.Index,
                             0,
                             &gs_Context->DevicePair.PresentationQueueFamily.Handle);

            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.GraphicsQueueFamily.Handle != VK_NULL_HANDLE,
                                    "Failed to get graphics queue")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.PresentationQueueFamily.Handle != VK_NULL_HANDLE,
                                    "Failed to get presentation queue")
        }

        void CreateSwapchains()
        {
            OTR_LOG_TRACE("Creating Vulkan swapchain...")

            CreateSingleSwapchain(gs_Context->Surface,
                                  gs_Context->DevicePair,
                                  gs_Context->Allocator,
                                  gs_Context->Swapchain);

            CreateSwapchainImages(gs_Context->DevicePair.LogicalDevice,
                                  gs_Context->Swapchain.Handle,
                                  g_SwapchainImages);

            CreateSwapchainImageViews(gs_Context->DevicePair.LogicalDevice,
                                      g_SwapchainImages,
                                      gs_Context->Swapchain.SurfaceFormat.format,
                                      g_SwapchainImageViews);
        }

        void RecreateSwapchains()
        {
            OTR_LOG_TRACE("Recreating Vulkan swapchain...")

            DestroySwapchains();

            CreateSingleSwapchain(gs_Context->Surface,
                                  gs_Context->DevicePair,
                                  gs_Context->Allocator,
                                  gs_Context->Swapchain);

            CreateSwapchainImages(gs_Context->DevicePair.LogicalDevice,
                                  gs_Context->Swapchain.Handle,
                                  g_SwapchainImages);

            CreateSwapchainImageViews(gs_Context->DevicePair.LogicalDevice,
                                      g_SwapchainImages,
                                      gs_Context->Swapchain.SurfaceFormat.format,
                                      g_SwapchainImageViews);
        }

        void DestroySwapchains()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Logical device must be initialized before destroying its swapchain")

            for (auto& swapchainFrameBuffer: g_SwapchainFrameBuffers)
            {
                vkDestroyFramebuffer(gs_Context->DevicePair.LogicalDevice,
                                     swapchainFrameBuffer,
                                     gs_Context->Allocator);
            }
            g_SwapchainFrameBuffers.ClearDestructive();

            for (auto& swapchainImageView: g_SwapchainImageViews)
            {
                vkDestroyImageView(gs_Context->DevicePair.LogicalDevice,
                                   swapchainImageView,
                                   gs_Context->Allocator);
            }
            g_SwapchainImageViews.ClearDestructive();
            g_SwapchainImages.ClearDestructive();

            vkDestroySwapchainKHR(gs_Context->DevicePair.LogicalDevice,
                                  gs_Context->Swapchain.Handle,
                                  gs_Context->Allocator);
        }

        void CreateRenderPass()
        {
            VkAttachmentDescription colorAttachment{ };
            colorAttachment.format         = gs_Context->Swapchain.SurfaceFormat.format;
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

            OTR_VULKAN_VALIDATE(vkCreateRenderPass(gs_Context->DevicePair.LogicalDevice,
                                                   &renderPassInfo,
                                                   gs_Context->Allocator,
                                                   &gs_Context->RenderPass))
        }

        void DestroyRenderPass()
        {
            vkDestroyRenderPass(gs_Context->DevicePair.LogicalDevice,
                                gs_Context->RenderPass,
                                gs_Context->Allocator);
        }

        void CreateCommandBuffers()
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo{ };
            commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = gs_Context->DevicePair.GraphicsQueueFamily.Index;
            commandPoolCreateInfo.pNext            = nullptr;

            OTR_VULKAN_VALIDATE(vkCreateCommandPool(gs_Context->DevicePair.LogicalDevice,
                                                    &commandPoolCreateInfo,
                                                    gs_Context->Allocator,
                                                    &gs_Context->DevicePair.GraphicsCommandPool))

            VkCommandBufferAllocateInfo commandBufferAllocateInfo{ };
            commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool        = gs_Context->DevicePair.GraphicsCommandPool;
            commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = gs_Context->Swapchain.MaxFramesInFlight;
            commandBufferAllocateInfo.pNext              = nullptr;

            VkCommandBuffer tempCommandBuffers[gs_Context->Swapchain.MaxFramesInFlight];
            OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(gs_Context->DevicePair.LogicalDevice,
                                                         &commandBufferAllocateInfo,
                                                         tempCommandBuffers))

            Collections::New<VkCommandBuffer>(tempCommandBuffers,
                                              gs_Context->Swapchain.MaxFramesInFlight,
                                              gs_Context->DevicePair.CommandBuffers);
        }

        void DestroyCommandBuffers()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.GraphicsCommandPool != VK_NULL_HANDLE,
                                    "Vulkan command pool is null!")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Vulkan logical device is null!")

            vkDestroyCommandPool(gs_Context->DevicePair.LogicalDevice,
                                 gs_Context->DevicePair.GraphicsCommandPool,
                                 gs_Context->Allocator);

            gs_Context->DevicePair.CommandBuffers.ClearDestructive();
        }

#if !OTR_RUNTIME
        void CreateVulkanDebugMessenger()
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            PopulateDebugMessengerCreateInfo(createInfo);

            auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                gs_Context->Instance,
                "vkCreateDebugUtilsMessengerEXT");

            if (createDebugUtilsMessenger)
            {
                OTR_VULKAN_VALIDATE(createDebugUtilsMessenger(gs_Context->Instance,
                                                              &createInfo, gs_Context->Allocator,
                                                              &gs_Context->DebugMessenger))
            }
        }

        void DestroyVulkanDebugMessenger()
        {
            auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                gs_Context->Instance,
                "vkDestroyDebugUtilsMessengerEXT");

            if (destroyDebugUtilsMessenger != nullptr)
                destroyDebugUtilsMessenger(gs_Context->Instance,
                                           gs_Context->DebugMessenger,
                                           gs_Context->Allocator);
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                     [[maybe_unused]] void* userData)
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

        void GetRequiredInstanceValidationLayers(List<const char*>& layers)
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
                        OTR_LOG_TRACE("Required validation layer found: {0}", layers[i])
                        break;
                    }
                }

                OTR_INTERNAL_ASSERT_MSG(found, "Required layer is missing: {0}", layers[i])
            }

            Buffer::Delete(availableLayers, availableLayerCount);
        }
#endif
    }
}