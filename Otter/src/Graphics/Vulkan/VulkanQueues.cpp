#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanQueues.h"
#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    void QueryQueueFamilies(const VkSurfaceKHR& surface,
                            const VkPhysicalDevice& physicalDevice,
                            UInt32& graphicsFamily,
                            UInt32& presentFamily)
    {
        OTR_INTERNAL_ASSERT_MSG(surface != VK_NULL_HANDLE,
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
                                                                     surface,
                                                                     &isPresentationSupported))

            if (isPresentationSupported)
                presentFamily = currentIndex;

            if (graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX)
                break;

            currentIndex++;
        }
    }
}