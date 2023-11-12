#ifndef OTTERENGINE_VULKANQUEUES_H
#define OTTERENGINE_VULKANQUEUES_H

#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    void QueryQueueFamilies(const VkSurfaceKHR& surface,
                            const VkPhysicalDevice& physicalDevice,
                            UInt32& graphicsFamily,
                            UInt32& presentFamily);
}

#endif //OTTERENGINE_VULKANQUEUES_H
