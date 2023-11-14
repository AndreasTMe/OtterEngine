#ifndef OTTERENGINE_VULKANSHADERS_H
#define OTTERENGINE_VULKANSHADERS_H

#include "Graphics/Vulkan/VulkanBase.Includes.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Shader.h"

namespace Otter::Graphics::Vulkan
{
    bool TryCreateShaderModule(const VkDevice& logicalDevice,
                               const VkAllocationCallbacks* allocator,
                               const char* filePath,
                               VkShaderModule* outShaderModule);
    void DestroyShaderModule(const VkDevice& logicalDevice,
                             const VkAllocationCallbacks* allocator,
                             VkShaderModule* outShaderModule);
}

#endif //OTTERENGINE_VULKANSHADERS_H
