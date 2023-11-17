#ifndef OTTERENGINE_VULKANEXTENSIONS_H
#define OTTERENGINE_VULKANEXTENSIONS_H

#include "Core/Collections/List.h"
#include "Graphics/API/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions);

    void GetDeviceRequiredExtensions(List<const char*>& requiredExtensions);
    bool DeviceSupportsRequiredExtensions(const VkPhysicalDevice& physicalDevice);
}

#endif //OTTERENGINE_VULKANEXTENSIONS_H
