#ifndef OTTERENGINE_VULKANBASE_PLATFORM_H
#define OTTERENGINE_VULKANBASE_PLATFORM_H

#include "Core/Collections/List.h"

namespace Otter::Graphics::Vulkan
{
    void GetRequiredInstanceExtensions(List<const char*>& extensions, List<const char*>& layers);

    void GetRequiredDeviceExtensions(List<const char*>& extensions, List<const char*>& layers);
}

#endif //OTTERENGINE_VULKANBASE_PLATFORM_H
