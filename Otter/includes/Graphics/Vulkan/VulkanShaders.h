#ifndef OTTERENGINE_VULKANSHADERS_H
#define OTTERENGINE_VULKANSHADERS_H

#include <fstream>

#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    bool LoadShaderModule(const char* filePath, VkShaderModuleCreateInfo* outCreateInfo);
}

#endif //OTTERENGINE_VULKANSHADERS_H
