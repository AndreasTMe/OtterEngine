#ifndef OTTERENGINE_VULKANTYPES_SHADER_H
#define OTTERENGINE_VULKANTYPES_SHADER_H

#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanShaderStage
    {
        VkShaderModule           Handle     = VK_NULL_HANDLE;
        VkShaderModuleCreateInfo CreateInfo = { };
    };

    struct VulkanShader
    {
        VulkanShaderStage VertexStage;
        VulkanShaderStage FragmentStage;
    };
}

#endif //OTTERENGINE_VULKANTYPES_SHADER_H
