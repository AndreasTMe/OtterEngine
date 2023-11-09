#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanSwapchain.h"

namespace Otter::Graphics::Vulkan
{
    static VkInstance   gs_Instance = VK_NULL_HANDLE;
    static VkSurfaceKHR gs_Surface  = VK_NULL_HANDLE;

    VulkanDevicePair g_DevicePair = { };

    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void GetDeviceRequiredExtensions(List<const char*>& requiredExtensions);

    void CreateDevicePairs(const VulkanContext* const vulkanContext, VulkanDevicePair& outDevicePair)
    {
        OTR_INTERNAL_ASSERT_MSG(vulkanContext->m_Instance != VK_NULL_HANDLE,
                                "Instance must be initialized before creating device pairs")
        OTR_INTERNAL_ASSERT_MSG(vulkanContext->m_Surface != VK_NULL_HANDLE,
                                "Surface must be initialized before creating device pairs")

        OTR_LOG_TRACE("Creating Vulkan device pairs...")

        gs_Instance = vulkanContext->m_Instance;
        gs_Surface  = vulkanContext->m_Surface;

        SelectPhysicalDevice();
        CreateLogicalDevice();

        outDevicePair = g_DevicePair;
    }

    void DestroyDevicePairs(const VulkanContext* const vulkanContext)
    {
        OTR_INTERNAL_ASSERT_MSG(vulkanContext->m_DevicePair.m_LogicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before destroying device pairs")

        vkDestroyDevice(vulkanContext->m_DevicePair.m_LogicalDevice, vulkanContext->m_Allocator);
    }

    void SelectPhysicalDevice()
    {
        UInt32 deviceCount = 0;
        OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Instance, &deviceCount, nullptr))

        OTR_INTERNAL_ASSERT_MSG(deviceCount > 0, "Failed to find GPUs with Vulkan support")

        VkPhysicalDevice queriedDevices[deviceCount];
        OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Instance, &deviceCount, queriedDevices))

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
            QuerySwapchainSupport(gs_Surface, queriedDevice, swapchainSupportInfo);

            bool isSwapchainSupported = swapchainSupportInfo.m_SurfaceFormats.GetCount() > 0
                                        && swapchainSupportInfo.m_PresentModes.GetCount() > 0;

            if (!isSwapchainSupported)
            {
                OTR_LOG_TRACE("{0} is not suitable due to missing swapchain support. "
                              "Moving to the next one...", deviceProperties.deviceName)
                continue;
            }

            g_DevicePair.m_PhysicalDevice                  = queriedDevice;
            g_DevicePair.m_GraphicsQueueFamily.m_Index     = graphicsFamily;
            g_DevicePair.m_PresentationQueueFamily.m_Index = presentFamily;

            OTR_LOG_TRACE("Found a suitable GPU: {0}", deviceProperties.deviceName)

            break;
        }

        OTR_INTERNAL_ASSERT_MSG(g_DevicePair.m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU")
    }

    void CreateLogicalDevice()
    {
        List <VkDeviceQueueCreateInfo> queueCreateInfos;

        HashSet <UInt32> uniqueQueueFamilies(2);
        uniqueQueueFamilies.TryAdd(g_DevicePair.m_GraphicsQueueFamily.m_Index);
        uniqueQueueFamilies.TryAdd(g_DevicePair.m_PresentationQueueFamily.m_Index);

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

        OTR_VULKAN_VALIDATE(vkCreateDevice(g_DevicePair.m_PhysicalDevice,
                                           &createInfo,
                                           nullptr,
                                           &g_DevicePair.m_LogicalDevice))

        OTR_INTERNAL_ASSERT_MSG(g_DevicePair.m_LogicalDevice != VK_NULL_HANDLE, "Failed to create logical device")

        vkGetDeviceQueue(g_DevicePair.m_LogicalDevice,
                         g_DevicePair.m_GraphicsQueueFamily.m_Index,
                         0,
                         &g_DevicePair.m_GraphicsQueueFamily.m_Handle);
        vkGetDeviceQueue(g_DevicePair.m_LogicalDevice,
                         g_DevicePair.m_PresentationQueueFamily.m_Index,
                         0,
                         &g_DevicePair.m_PresentationQueueFamily.m_Handle);

        OTR_INTERNAL_ASSERT_MSG(g_DevicePair.m_GraphicsQueueFamily.m_Handle != VK_NULL_HANDLE,
                                "Failed to get graphics queue")
        OTR_INTERNAL_ASSERT_MSG(g_DevicePair.m_PresentationQueueFamily.m_Handle != VK_NULL_HANDLE,
                                "Failed to get presentation queue")
    }

    void QueryQueueFamilies(const VkPhysicalDevice& physicalDevice, UInt32& graphicsFamily, UInt32& presentFamily)
    {
        OTR_INTERNAL_ASSERT_MSG(gs_Surface != VK_NULL_HANDLE,
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
                                                                     gs_Surface,
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
}