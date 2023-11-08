#ifndef OTTERENGINE_VULKANDEVICE_H
#define OTTERENGINE_VULKANDEVICE_H

#include "Graphics/Vulkan/VulkanBase.Types.h"

namespace Otter::Graphics::Vulkan
{
    void CreateDevicePairs(VulkanContext* vulkanContext);

    void QueryQueueFamilies(const VkPhysicalDevice& physicalDevice, UInt32& graphicsFamily, UInt32& presentFamily);
    void QuerySwapchainSupport(const VkPhysicalDevice& physicalDevice, SwapchainSupportInfo& swapchainSupportInfo);
    bool DeviceSupportsRequiredExtensions(const VkPhysicalDevice& physicalDevice);
}

#endif //OTTERENGINE_VULKANDEVICE_H
