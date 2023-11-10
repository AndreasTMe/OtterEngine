#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanRenderer.h"
#include "Graphics/Vulkan/VulkanBase.Types.h"
#include "Graphics/Vulkan/VulkanBase.Internal.h"
#include "Math/Core.h"

namespace Otter::Graphics::Vulkan
{
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
        Internal::CreateSwapchain();
        Internal::CreateRenderPass();

        Internal::CreateCommandBuffers(gs_Context->m_DevicePair.m_GraphicsCommandPool,
                                       gs_Context->m_DevicePair.m_CommandBuffers);
    }

    void Shutdown()
    {
        vkDeviceWaitIdle(gs_Context->m_DevicePair.m_LogicalDevice);

        Internal::DestroyCommandBuffers(gs_Context->m_DevicePair.m_CommandBuffers);

        Internal::DestroyRenderPass();
        Internal::DestroySwapchain();
        Internal::DestroyDevicePairs();
        Internal::DestroySurface();

#if !OTR_RUNTIME
        Internal::DestroyVulkanDebugMessenger();
#endif

        Internal::DestroyVulkanInstance();

        Delete(gs_Context);
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
            Internal::GetRequiredInstanceExtensions(extensions);

            createInfo.enabledExtensionCount   = extensions.GetCount();
            createInfo.ppEnabledExtensionNames = extensions.GetData();

#if !OTR_RUNTIME
            OTR_LOG_TRACE("Creating Vulkan debug messenger...")

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
            Internal::PopulateDebugMessengerCreateInfo(debugCreateInfo);

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

            OTR_VULKAN_VALIDATE(vkCreateInstance(&createInfo, gs_Context->m_Allocator, &gs_Context->m_Instance))
        }

        void DestroyVulkanInstance()
        {
            vkDestroyInstance(gs_Context->m_Instance, gs_Context->m_Allocator);
        }

        void CreateSurface(const void* const platformContext)
        {
#if OTR_PLATFORM_WINDOWS
            const auto* const platformContextData       = ((PlatformContext*) platformContext)->m_Data;
            const auto* const windowsPlatformWindowData =
                          (Otter::Internal::WindowsPlatformWindowData*) platformContextData;

            VkWin32SurfaceCreateInfoKHR createInfo = { };
            createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            createInfo.hwnd      = windowsPlatformWindowData->m_WindowHandle;
            createInfo.hinstance = windowsPlatformWindowData->m_InstanceHandle;

            OTR_VULKAN_VALIDATE(vkCreateWin32SurfaceKHR(gs_Context->m_Instance,
                                                        &createInfo,
                                                        gs_Context->m_Allocator,
                                                        &gs_Context->m_Surface))
#else
            OTR_LOG_FATAL("'CreateSurface' not supported for this platform")
#endif
        }

        void DestroySurface()
        {
            vkDestroySurfaceKHR(gs_Context->m_Instance, gs_Context->m_Surface, gs_Context->m_Allocator);
        }

        void CreateDevicePairs()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_Instance != VK_NULL_HANDLE,
                                    "Instance must be initialized before creating device pairs")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_Surface != VK_NULL_HANDLE,
                                    "Surface must be initialized before creating device pairs")

            OTR_LOG_TRACE("Creating Vulkan device pairs...")

            SelectPhysicalDevice();
            CreateLogicalDevice();
        }

        void DestroyDevicePairs()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                    "Logical device must be initialized before destroying device pairs")

            vkDestroyDevice(gs_Context->m_DevicePair.m_LogicalDevice, gs_Context->m_Allocator);
        }

        void SelectPhysicalDevice()
        {
            UInt32 deviceCount = 0;
            OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Context->m_Instance,
                                                           &deviceCount,
                                                           nullptr))

            OTR_INTERNAL_ASSERT_MSG(deviceCount > 0, "Failed to find GPUs with Vulkan support")

            VkPhysicalDevice queriedDevices[deviceCount];
            OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Context->m_Instance, &deviceCount, queriedDevices))

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
                QueryQueueFamilies(queriedDevice, graphicsFamily, presentFamily);

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
                QuerySwapchainSupport(queriedDevice, swapchainSupportInfo);

                bool isSwapchainSupported = swapchainSupportInfo.m_SurfaceFormats.GetCount() > 0
                                            && swapchainSupportInfo.m_PresentModes.GetCount() > 0;

                if (!isSwapchainSupported)
                {
                    OTR_LOG_TRACE("{0} is not suitable due to missing swapchain support. "
                                  "Moving to the next one...", deviceProperties.deviceName)
                    continue;
                }

                gs_Context->m_DevicePair.m_PhysicalDevice                  = queriedDevice;
                gs_Context->m_DevicePair.m_GraphicsQueueFamily.m_Index     = graphicsFamily;
                gs_Context->m_DevicePair.m_PresentationQueueFamily.m_Index = presentFamily;

                OTR_LOG_TRACE("Found a suitable GPU: {0}", deviceProperties.deviceName)

                break;
            }

            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_PhysicalDevice != VK_NULL_HANDLE,
                                    "Failed to find a suitable GPU")
        }

        void CreateLogicalDevice()
        {
            List <VkDeviceQueueCreateInfo> queueCreateInfos;

            HashSet <UInt32> uniqueQueueFamilies(2);
            uniqueQueueFamilies.TryAdd(gs_Context->m_DevicePair.m_GraphicsQueueFamily.m_Index);
            uniqueQueueFamilies.TryAdd(gs_Context->m_DevicePair.m_PresentationQueueFamily.m_Index);

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

            OTR_VULKAN_VALIDATE(vkCreateDevice(gs_Context->m_DevicePair.m_PhysicalDevice,
                                               &createInfo,
                                               nullptr,
                                               &gs_Context->m_DevicePair.m_LogicalDevice))

            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                    "Failed to create logical device")

            vkGetDeviceQueue(gs_Context->m_DevicePair.m_LogicalDevice,
                             gs_Context->m_DevicePair.m_GraphicsQueueFamily.m_Index,
                             0,
                             &gs_Context->m_DevicePair.m_GraphicsQueueFamily.m_Handle);
            vkGetDeviceQueue(gs_Context->m_DevicePair.m_LogicalDevice,
                             gs_Context->m_DevicePair.m_PresentationQueueFamily.m_Index,
                             0,
                             &gs_Context->m_DevicePair.m_PresentationQueueFamily.m_Handle);

            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_GraphicsQueueFamily.m_Handle != VK_NULL_HANDLE,
                                    "Failed to get graphics queue")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_PresentationQueueFamily.m_Handle != VK_NULL_HANDLE,
                                    "Failed to get presentation queue")
        }

        void QueryQueueFamilies(const VkPhysicalDevice& physicalDevice, UInt32& graphicsFamily, UInt32& presentFamily)
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_Surface != VK_NULL_HANDLE,
                                    "Surface must be initialized before querying queue families")
            OTR_INTERNAL_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE,
                                    "Physical device must be initialized before querying queue families")

            UInt32 queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

            OTR_INTERNAL_ASSERT_MSG(queueFamilyCount > 0, "Failed to find queue families")

            VkQueueFamilyProperties queueFamilies[queueFamilyCount];
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

            UInt32 currentIndex = 0;
            for (const auto& queueFamily: queueFamilies)
            {
                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    graphicsFamily = currentIndex;

                VkBool32 isPresentationSupported = false;
                OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,
                                                                         currentIndex,
                                                                         gs_Context->m_Surface,
                                                                         &isPresentationSupported))

                if (isPresentationSupported)
                    presentFamily = currentIndex;

                if (graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX)
                    break;

                currentIndex++;
            }
        }

        void GetDeviceRequiredExtensions(List<const char*>& requiredExtensions)
        {
            requiredExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }

        bool DeviceSupportsRequiredExtensions(const VkPhysicalDevice& physicalDevice)
        {
            OTR_INTERNAL_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE,
                                    "Physical device must be initialized before checking for extensions")

            List<const char*> requiredExtensions;
            GetDeviceRequiredExtensions(requiredExtensions);

            UInt32 extensionCount;
            OTR_VULKAN_VALIDATE(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr))

            VkExtensionProperties availableExtensions[extensionCount];
            OTR_VULKAN_VALIDATE(vkEnumerateDeviceExtensionProperties(physicalDevice,
                                                                     nullptr,
                                                                     &extensionCount,
                                                                     availableExtensions))

            bool foundRequiredExtensions = false;
            for (const auto& extension: requiredExtensions)
            {
                for (const auto& availableExtension: availableExtensions)
                {
                    if (strcmp(extension, availableExtension.extensionName) == 0)
                    {
                        foundRequiredExtensions = true;
                        break;
                    }
                }
            }

            return foundRequiredExtensions;
        }

        void CreateSwapchain()
        {
            OTR_LOG_TRACE("Creating Vulkan swapchain...")

            CreateSwapchainInternal(gs_Context->m_DevicePair, gs_Context->m_Allocator, gs_Context->m_Swapchain);
            CreateSwapchainImages(gs_Context->m_DevicePair.m_LogicalDevice,
                                  gs_Context->m_Swapchain.m_Handle,
                                  gs_Context->m_Swapchain.m_ImageCount);
            CreateSwapchainImageViews(gs_Context->m_DevicePair.m_LogicalDevice,
                                      gs_Context->m_Swapchain.m_SurfaceFormat.format);
        }

        void RecreateSwapchain()
        {
            OTR_LOG_TRACE("Recreating Vulkan swapchain...")

            DestroySwapchain();

            CreateSwapchainInternal(gs_Context->m_DevicePair,
                                    gs_Context->m_Allocator,
                                    gs_Context->m_Swapchain);
            CreateSwapchainImages(gs_Context->m_DevicePair.m_LogicalDevice,
                                  gs_Context->m_Swapchain.m_Handle,
                                  gs_Context->m_Swapchain.m_ImageCount);
            CreateSwapchainImageViews(gs_Context->m_DevicePair.m_LogicalDevice,
                                      gs_Context->m_Swapchain.m_SurfaceFormat.format);
        }

        void DestroySwapchain()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                    "Logical device must be initialized before destroying its swapchain")

            for (auto& swapchainFrameBuffer: g_SwapchainFrameBuffers)
            {
                vkDestroyFramebuffer(gs_Context->m_DevicePair.m_LogicalDevice,
                                     swapchainFrameBuffer,
                                     gs_Context->m_Allocator);
            }
            g_SwapchainFrameBuffers.ClearDestructive();

            for (auto& swapchainImageView: g_SwapchainImageViews)
            {
                vkDestroyImageView(gs_Context->m_DevicePair.m_LogicalDevice,
                                   swapchainImageView,
                                   gs_Context->m_Allocator);
            }
            g_SwapchainImageViews.ClearDestructive();
            g_SwapchainImages.ClearDestructive();

            vkDestroySwapchainKHR(gs_Context->m_DevicePair.m_LogicalDevice,
                                  gs_Context->m_Swapchain.m_Handle,
                                  gs_Context->m_Allocator);
        }

        void QuerySwapchainSupport(const VkPhysicalDevice& physicalDevice, SwapchainSupportInfo& swapchainSupportInfo)
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_Surface != VK_NULL_HANDLE,
                                    "Surface must be initialized before querying swapchain support")
            OTR_INTERNAL_ASSERT_MSG(physicalDevice != VK_NULL_HANDLE,
                                    "Physical device must be initialized before querying swapchain support")

            OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,
                                                                          gs_Context->m_Surface,
                                                                          &swapchainSupportInfo.m_SurfaceCapabilities))

            UInt32 surfaceFormatCount;
            OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                                                     gs_Context->m_Surface,
                                                                     &surfaceFormatCount,
                                                                     nullptr))

            if (surfaceFormatCount != 0)
            {
                VkSurfaceFormatKHR surfaceFormats[surfaceFormatCount];
                OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physicalDevice,
                    gs_Context->m_Surface,
                    &surfaceFormatCount,
                    surfaceFormats))

                Collections::New<VkSurfaceFormatKHR>(surfaceFormats,
                                                     surfaceFormatCount,
                                                     swapchainSupportInfo.m_SurfaceFormats);
            }

            UInt32 presentModeCount;
            OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                                                          gs_Context->m_Surface,
                                                                          &presentModeCount,
                                                                          nullptr))

            if (presentModeCount != 0)
            {
                VkPresentModeKHR presentModes[presentModeCount];
                OTR_VULKAN_VALIDATE(vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physicalDevice,
                    gs_Context->m_Surface,
                    &presentModeCount,
                    presentModes))

                Collections::New<VkPresentModeKHR>(presentModes,
                                                   presentModeCount,
                                                   swapchainSupportInfo.m_PresentModes);
            }
        }

        void CreateSwapchainInternal(const VulkanDevicePair& devicePair,
                                     const VkAllocationCallbacks* const allocator,
                                     VulkanSwapchain& swapchain)
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_Surface != VK_NULL_HANDLE,
                                    "Surface must be initialized before creating its swapchain")
            OTR_INTERNAL_ASSERT_MSG(devicePair.m_PhysicalDevice != VK_NULL_HANDLE,
                                    "Physical device must be initialized before creating its swapchain")
            OTR_INTERNAL_ASSERT_MSG(devicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                    "Logical device must be initialized before creating its swapchain")

            SwapchainSupportInfo swapchainSupportInfo;
            QuerySwapchainSupport(devicePair.m_PhysicalDevice, swapchainSupportInfo);

            swapchain.m_SurfaceFormat = SelectSwapchainSurfaceFormat(swapchainSupportInfo.m_SurfaceFormats);
            swapchain.m_PresentMode   = SelectSwapchainPresentMode(swapchainSupportInfo.m_PresentModes);
            swapchain.m_Extent        = SelectSwapchainExtent(swapchainSupportInfo.m_SurfaceCapabilities);
            swapchain.m_ImageCount    = swapchainSupportInfo.m_SurfaceCapabilities.minImageCount + 1;

            if (swapchainSupportInfo.m_SurfaceCapabilities.maxImageCount > 0
                && swapchain.m_ImageCount > swapchainSupportInfo.m_SurfaceCapabilities.maxImageCount)
                swapchain.m_ImageCount = swapchainSupportInfo.m_SurfaceCapabilities.maxImageCount;

            swapchain.m_MaxFramesInFlight = swapchain.m_ImageCount - 1;

            VkSwapchainCreateInfoKHR createInfo{ };
            createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = gs_Context->m_Surface;

            createInfo.minImageCount    = swapchain.m_ImageCount;
            createInfo.imageFormat      = swapchain.m_SurfaceFormat.format;
            createInfo.imageColorSpace  = swapchain.m_SurfaceFormat.colorSpace;
            createInfo.imageExtent      = swapchain.m_Extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            if (!devicePair.GraphicsAndPresentationQueueFamiliesAreTheSame())
            {
                ReadOnlySpan<UInt32, 2> queueFamilyIndices{ devicePair.m_GraphicsQueueFamily.m_Index,
                                                            devicePair.m_PresentationQueueFamily.m_Index };

                createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices   = queueFamilyIndices.GetData();
            }
            else
            {
                createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0;
                createInfo.pQueueFamilyIndices   = nullptr;
            }

            createInfo.preTransform   = swapchainSupportInfo.m_SurfaceCapabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode    = swapchain.m_PresentMode;
            createInfo.clipped        = VK_TRUE;
            createInfo.oldSwapchain   = VK_NULL_HANDLE;

            OTR_VULKAN_VALIDATE(vkCreateSwapchainKHR(devicePair.m_LogicalDevice,
                                                     &createInfo,
                                                     allocator,
                                                     &swapchain.m_Handle))
        }

        void CreateSwapchainImages(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain, UInt32& imageCount)
        {
            OTR_VULKAN_VALIDATE(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr))

            VkImage swapchainImages[imageCount];
            OTR_VULKAN_VALIDATE(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages))

            Collections::New<VkImage>(swapchainImages, imageCount, g_SwapchainImages);
        }

        void CreateSwapchainImageViews(const VkDevice& logicalDevice, const VkFormat& imageFormat)
        {
            VkImageView swapchainImageViews[g_SwapchainImages.GetCount()];

            UInt64 index = 0;
            for (const auto& swapchainImage: g_SwapchainImages)
            {
                VkImageViewCreateInfo createInfo{ };
                createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                createInfo.image                           = swapchainImage;
                createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format                          = imageFormat;
                createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel   = 0;
                createInfo.subresourceRange.levelCount     = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount     = 1;

                OTR_VULKAN_VALIDATE(vkCreateImageView(logicalDevice,
                                                      &createInfo,
                                                      nullptr,
                                                      &swapchainImageViews[index++]))
            }

            Collections::New<VkImageView>(swapchainImageViews, g_SwapchainImages.GetCount(), g_SwapchainImageViews);
        }

        VkSurfaceFormatKHR SelectSwapchainSurfaceFormat(const List <VkSurfaceFormatKHR>& surfaceFormats)
        {
            for (const auto& surfaceFormat: surfaceFormats)
                if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    return surfaceFormat;

            return surfaceFormats[0];
        }

        VkPresentModeKHR SelectSwapchainPresentMode(const List <VkPresentModeKHR>& presentModes)
        {
            for (const auto& presentMode: presentModes)
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return presentMode;

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D SelectSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
        {
            OTR_INTERNAL_ASSERT_MSG(capabilities.currentExtent.width != Math::PositiveInfinity<UInt32>,
                                    "Current extent width must be initialized before selecting swap extent")

            VkExtent2D swapchainExtent = capabilities.currentExtent;
            swapchainExtent.height = Math::Clamp(swapchainExtent.height,
                                                 capabilities.minImageExtent.height,
                                                 capabilities.maxImageExtent.height);
            swapchainExtent.width  = Math::Clamp(swapchainExtent.width,
                                                 capabilities.minImageExtent.width,
                                                 capabilities.maxImageExtent.width);

            return swapchainExtent;
        }

        void CreateRenderPass()
        {
            VkAttachmentDescription colorAttachment{ };
            colorAttachment.format         = gs_Context->m_Swapchain.m_SurfaceFormat.format;
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

            OTR_VULKAN_VALIDATE(vkCreateRenderPass(gs_Context->m_DevicePair.m_LogicalDevice,
                                                   &renderPassInfo,
                                                   gs_Context->m_Allocator,
                                                   &gs_Context->m_RenderPass))
        }

        void DestroyRenderPass()
        {
            vkDestroyRenderPass(gs_Context->m_DevicePair.m_LogicalDevice,
                                gs_Context->m_RenderPass,
                                gs_Context->m_Allocator);
        }

        void CreateCommandBuffers(VkCommandPool& commandPool, List <VkCommandBuffer>& commandBuffers)
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo{ };
            commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = gs_Context->m_DevicePair.m_GraphicsQueueFamily.m_Index;
            commandPoolCreateInfo.pNext            = nullptr;

            OTR_VULKAN_VALIDATE(vkCreateCommandPool(gs_Context->m_DevicePair.m_LogicalDevice,
                                                    &commandPoolCreateInfo,
                                                    gs_Context->m_Allocator,
                                                    &commandPool))

            VkCommandBufferAllocateInfo commandBufferAllocateInfo{ };
            commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool        = commandPool;
            commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = gs_Context->m_Swapchain.m_MaxFramesInFlight;
            commandBufferAllocateInfo.pNext              = nullptr;

            VkCommandBuffer tempCommandBuffers[gs_Context->m_Swapchain.m_MaxFramesInFlight];
            OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(gs_Context->m_DevicePair.m_LogicalDevice,
                                                         &commandBufferAllocateInfo,
                                                         tempCommandBuffers))

            Collections::New<VkCommandBuffer>(tempCommandBuffers,
                                              gs_Context->m_Swapchain.m_MaxFramesInFlight,
                                              commandBuffers);
        }

        void DestroyCommandBuffers(List <VkCommandBuffer>& commandBuffers)
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_GraphicsCommandPool != VK_NULL_HANDLE,
                                    "Vulkan command pool is null!")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->m_DevicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                    "Vulkan logical device is null!")

            vkDestroyCommandPool(gs_Context->m_DevicePair.m_LogicalDevice,
                                 gs_Context->m_DevicePair.m_GraphicsCommandPool,
                                 gs_Context->m_Allocator);

            commandBuffers.ClearDestructive();
        }

        void AllocateCommandBuffer(const VkDevice& logicalDevice,
                                   const VkCommandPool& commandPool,
                                   VkCommandBuffer& commandBuffer,
                                   bool isPrimary /*= true*/)
        {
            OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
            OTR_INTERNAL_ASSERT_MSG(commandPool != VK_NULL_HANDLE, "Vulkan command pool is null!")

            VkCommandBufferAllocateInfo allocInfo{ };
            allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level              = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                                                     : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            allocInfo.commandPool        = commandPool;
            allocInfo.commandBufferCount = 1;
            allocInfo.pNext              = nullptr;

            OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer))
        }

        void FreeCommandBuffer(const VkDevice& logicalDevice,
                               const VkCommandPool& commandPool,
                               VkCommandBuffer& commandBuffer)
        {
            OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
            OTR_INTERNAL_ASSERT_MSG(commandPool != VK_NULL_HANDLE, "Vulkan command pool is null!")

            vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
        }

        void BeginCommandBuffer(const VkCommandBuffer& commandBuffer,
                                bool isSingleUse,
                                bool isRenderPassContinue,
                                bool isSimultaneousUse)
        {
            VkCommandBufferBeginInfo beginInfo{ };
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (isSingleUse)
                beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            if (isRenderPassContinue)
                beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
            if (isSimultaneousUse)
                beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

            beginInfo.pInheritanceInfo = nullptr;
            beginInfo.pNext            = nullptr;

            OTR_VULKAN_VALIDATE(vkBeginCommandBuffer(commandBuffer, &beginInfo))
        }

        void EndCommandBuffer(const VkCommandBuffer& commandBuffer)
        {
            OTR_VULKAN_VALIDATE(vkEndCommandBuffer(commandBuffer))
        }

        // TODO: Move this to elsewhere maybe?
        void CopyBuffer(const VulkanContext* const vulkanContext,
                        const VkBuffer& sourceBuffer,
                        const VkDeviceSize& size,
                        VkBuffer& destinationBuffer)
        {
            OTR_INTERNAL_ASSERT_MSG(sourceBuffer != VK_NULL_HANDLE, "Vulkan source buffer is null!")
            OTR_INTERNAL_ASSERT_MSG(size != 0, "Vulkan device size is 0!")

            VkCommandBufferAllocateInfo allocInfo{ };
            allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool        = vulkanContext->m_DevicePair
                .m_GraphicsCommandPool; // TODO: Use copy command pool
            allocInfo.commandBufferCount = 1;
            allocInfo.pNext              = nullptr;

            VkCommandBuffer commandBuffer;
            OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(vulkanContext->m_DevicePair.m_LogicalDevice,
                                                         &allocInfo,
                                                         &commandBuffer))

            VkCommandBufferBeginInfo beginInfo{ };
            beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            beginInfo.pInheritanceInfo = nullptr;
            beginInfo.pNext            = nullptr;

            OTR_VULKAN_VALIDATE(vkBeginCommandBuffer(commandBuffer, &beginInfo))

            VkBufferCopy copyRegion{ };
            copyRegion.srcOffset = 0; // Optional
            copyRegion.dstOffset = 0; // Optional
            copyRegion.size      = size;
            vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion);

            OTR_VULKAN_VALIDATE(vkEndCommandBuffer(commandBuffer))

            VkSubmitInfo submitInfo{ };
            submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers    = &commandBuffer;
            // submitInfo.pWaitDstStageMask    = nullptr;
            // submitInfo.waitSemaphoreCount   = 0;
            // submitInfo.pWaitSemaphores      = nullptr;
            // submitInfo.signalSemaphoreCount = 0;
            // submitInfo.pSignalSemaphores    = nullptr;
            // submitInfo.pNext                = nullptr;

            OTR_VULKAN_VALIDATE(vkQueueSubmit(vulkanContext->m_DevicePair.m_GraphicsQueueFamily.m_Handle,
                                              1,
                                              &submitInfo,
                                              VK_NULL_HANDLE))
            OTR_VULKAN_VALIDATE(vkQueueWaitIdle(vulkanContext->m_DevicePair.m_GraphicsQueueFamily.m_Handle))

            vkFreeCommandBuffers(vulkanContext->m_DevicePair.m_LogicalDevice,
                                 vulkanContext->m_DevicePair.m_GraphicsCommandPool, // TODO: Use copy command pool
                                 1,
                                 &commandBuffer);
        }

        void GetRequiredInstanceExtensions(List<const char*>& extensions)
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
        void CreateVulkanDebugMessenger()
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            PopulateDebugMessengerCreateInfo(createInfo);

            auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                gs_Context->m_Instance,
                "vkCreateDebugUtilsMessengerEXT");

            if (createDebugUtilsMessenger)
            {
                OTR_VULKAN_VALIDATE(createDebugUtilsMessenger(gs_Context->m_Instance,
                                                              &createInfo, gs_Context->m_Allocator,
                                                              &gs_Context->m_DebugMessenger))
            }
        }

        void DestroyVulkanDebugMessenger()
        {
            auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                gs_Context->m_Instance,
                "vkDestroyDebugUtilsMessengerEXT");

            if (destroyDebugUtilsMessenger != nullptr)
                destroyDebugUtilsMessenger(gs_Context->m_Instance,
                                           gs_Context->m_DebugMessenger,
                                           gs_Context->m_Allocator);
        }

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
}