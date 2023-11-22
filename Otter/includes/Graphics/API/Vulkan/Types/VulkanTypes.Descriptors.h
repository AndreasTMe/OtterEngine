#ifndef OTTERENGINE_VULKANTYPES_DESCRIPTORS_H
#define OTTERENGINE_VULKANTYPES_DESCRIPTORS_H

#include "Core/Collections/Span.h"
#include "Graphics/API/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanDescriptor
    {
        List <VkDescriptorSet> Sets      = { };
        VkDescriptorSetLayout  SetLayout = VK_NULL_HANDLE;
        VkDescriptorPool       Pool      = VK_NULL_HANDLE;
    };
}

#endif //OTTERENGINE_VULKANTYPES_DESCRIPTORS_H
