#ifndef OTTERENGINE_VULKANPLATFORM_H
#define OTTERENGINE_VULKANPLATFORM_H

#include "Core/Collections/List.h"
#include "Graphics/Vulkan/VulkanContext.h"

namespace Otter::Graphics::Vulkan
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions, List<const char*>& layers);

    void GetRequiredDeviceExtensions(List<const char*>& extensions, List<const char*>& layers);

    void CreateSurface(VulkanContext* vulkanContext, const void* platformContext);
}

#endif //OTTERENGINE_VULKANPLATFORM_H
