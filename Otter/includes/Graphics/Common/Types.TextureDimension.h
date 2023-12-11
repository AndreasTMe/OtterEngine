#ifndef OTTERENGINE_TYPES_TEXTUREDIMENSION_H
#define OTTERENGINE_TYPES_TEXTUREDIMENSION_H

#include "Core/BaseTypes.h"

#define TEXTURE_DIMENSION_LIST  \
    REPLACE_WITH(TwoD, 0x01)    \
    REPLACE_WITH(ThreeD, 0x02)  \
    REPLACE_WITH(Cube, 0x03)

namespace Otter::Graphics
{
    enum class TextureDimension : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        TEXTURE_DIMENSION_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::Graphics::TextureDimension& attributeSize)
{
    switch (attributeSize)
    {
#define REPLACE_WITH(Item, Value) case Otter::Graphics::TextureDimension::Item: os << "TextureDimension::" << #Item; break;
        TEXTURE_DIMENSION_LIST
#undef REPLACE_WITH
        default:
            os << "TextureDimension[Unknown]";
    }

    return os;
}
#undef TEXTURE_DIMENSION_LIST

#endif //OTTERENGINE_TYPES_TEXTUREDIMENSION_H
