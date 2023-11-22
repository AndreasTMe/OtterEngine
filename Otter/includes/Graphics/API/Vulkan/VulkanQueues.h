#ifndef OTTERENGINE_VULKANQUEUES_H
#define OTTERENGINE_VULKANQUEUES_H

#include "Graphics/API/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    void QueryQueueFamilies(const VkSurfaceKHR& surface,
                            const VkPhysicalDevice& physicalDevice,
                            UInt32* outGraphicsFamily,
                            UInt32* outPresentFamily);
}

#endif //OTTERENGINE_VULKANQUEUES_H
