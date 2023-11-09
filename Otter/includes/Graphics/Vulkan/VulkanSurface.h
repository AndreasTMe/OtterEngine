#ifndef OTTERENGINE_VULKANSURFACE_H
#define OTTERENGINE_VULKANSURFACE_H

#include "Graphics/Vulkan/VulkanBase.Types.h"

namespace Otter::Graphics::Vulkan
{
    void CreateSurface(const VulkanContext* vulkanContext, const void* platformContext, VkSurfaceKHR& outSurface);
    void DestroySurface(const VulkanContext* vulkanContext);
}

#endif //OTTERENGINE_VULKANSURFACE_H
