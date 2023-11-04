#ifndef OTTERENGINE_VULKANSURFACE_H
#define OTTERENGINE_VULKANSURFACE_H

#include "Graphics/Vulkan/VulkanBase.Types.h"

namespace Otter::Graphics::Vulkan
{
    void CreateSurface(VulkanContext* vulkanContext, const void* platformContext);
}

#endif //OTTERENGINE_VULKANSURFACE_H
