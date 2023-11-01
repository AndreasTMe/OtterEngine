#ifndef OTTERENGINE_VULKANPLATFORM_H
#define OTTERENGINE_VULKANPLATFORM_H

#include "Core/Collections/List.h"

namespace Otter::Renderer
{
    static void GetRequiredInstanceExtensions(List<const char*>& extensions, List<const char*>& layers);

    static void GetRequiredDeviceExtensions(List<const char*>& extensions, List<const char*>& layers);
}

#endif //OTTERENGINE_VULKANPLATFORM_H
