#ifndef OTTERENGINE_TYPES_GRAPHICSAPI_H
#define OTTERENGINE_TYPES_GRAPHICSAPI_H

#include "Core/Types.h"

namespace Otter
{
#define GRAPHICS_API_LIST       \
    REPLACE_WITH(Vulkan,0x01)   \
    REPLACE_WITH(DirectX12,0x02)\
    REPLACE_WITH(Metal,0x03)

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

    enum class GraphicsAPI : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        GRAPHICS_API_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };

#pragma clang diagnostic pop

    template<typename OStream>
    OStream& operator<<(OStream& os, const Otter::GraphicsAPI& graphicsApi)
    {
        switch (graphicsApi)
        {
#define REPLACE_WITH(Item, Value) case Otter::GraphicsAPI::Item: os << "GraphicsAPI::" << #Item; break;
            GRAPHICS_API_LIST
#undef REPLACE_WITH
            default:
                os << "GraphicsAPI[Unknown]";
        }

        return os;
    }
#undef KEYCODE_LIST
}

#endif //OTTERENGINE_TYPES_GRAPHICSAPI_H
