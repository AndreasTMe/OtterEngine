#ifndef OTTERENGINE_VULKANTYPES_DESCRIPTORS_H
#define OTTERENGINE_VULKANTYPES_DESCRIPTORS_H

#include "Core/Collections/Span.h"
#include "Graphics/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    struct VulkanDescriptor
    {
        Span<VkDescriptorSet, OTR_VULKAN_BUFFERING_COUNT> Set       = { VK_NULL_HANDLE,
                                                                        VK_NULL_HANDLE,
                                                                        VK_NULL_HANDLE };
        VkDescriptorSetLayout                             SetLayout = VK_NULL_HANDLE;
        VkDescriptorPool                                  Pool      = VK_NULL_HANDLE;
    };
}

#endif //OTTERENGINE_VULKANTYPES_DESCRIPTORS_H
