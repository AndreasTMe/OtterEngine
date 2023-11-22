#ifndef OTTERENGINE_VULKANPOINT_H
#define OTTERENGINE_VULKANPOINT_H

#include "Core/Types.h"
#include "Core/Collections/Span.h"
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#include "Math/Vector.h"
#include "Graphics/API/Vulkan/VulkanBase.Includes.h"
#include "Graphics/Common/Types.Vertex.h"

namespace Otter::Graphics::Vulkan
{
#define OTR_VULKAN_POINT2D_BINDINGS_COUNT 1
#define OTR_VULKAN_POINT2D_ATTRIBUTES_COUNT 3

    class VulkanPoint
    {
    public:
        OTR_INLINE static auto GetBindingDescriptions()
        {
            Span<VkVertexInputBindingDescription, OTR_VULKAN_POINT2D_BINDINGS_COUNT> bindingDescriptions{ };
            bindingDescriptions[0].binding   = 0;
            bindingDescriptions[0].stride    = sizeof(Vertex);
            bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescriptions;
        }

        OTR_INLINE static auto GetAttributeDescriptions()
        {
            Span<VkVertexInputAttributeDescription, OTR_VULKAN_POINT2D_ATTRIBUTES_COUNT> attributeDescriptions{ };

            // Position
            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Vertex, Position);
            // Color
            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Vertex, Color);
            // TexCoord
            attributeDescriptions[2].binding  = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset   = offsetof(Vertex, TexCoord);

            return attributeDescriptions.AsReadOnly();
        }
    };

#undef OTR_VULKAN_POINT2D_BINDINGS_COUNT
#undef OTR_VULKAN_POINT2D_ATTRIBUTES_COUNT
}

#endif //OTTERENGINE_VULKANPOINT_H
