#ifndef OTTERENGINE_VULKANTYPES_SHADER_H
#define OTTERENGINE_VULKANTYPES_SHADER_H

#include "Graphics/API/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanShader
    {
        VkShaderModule VertexModule   = VK_NULL_HANDLE;
        VkShaderModule FragmentModule = VK_NULL_HANDLE;
    };
}

#endif //OTTERENGINE_VULKANTYPES_SHADER_H
