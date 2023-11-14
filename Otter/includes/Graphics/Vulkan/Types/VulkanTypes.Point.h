#ifndef OTTERENGINE_VULKANTYPES_POINT_H
#define OTTERENGINE_VULKANTYPES_POINT_H

#include "Core/Types.h"
#include "Core/Collections/Span.h"
#include "Math/Vector.h"

namespace Otter::Graphics::Vulkan
{
    struct Point
    {
        Vector<3, Float32> Position;
        Vector<4, Float32> Color;

        OTR_INLINE static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{ };
            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Point);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        OTR_INLINE static ReadOnlySpan<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
        {
            Span<VkVertexInputAttributeDescription, 2> attributeDescriptions{ };
            // Position
            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Point, Position);
            // Color
            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Point, Color);

            return attributeDescriptions.AsReadOnly();
        }
    };
}

#endif //OTTERENGINE_VULKANTYPES_POINT_H
