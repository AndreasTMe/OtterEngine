#ifndef OTTERENGINE_TYPES_SHADERATTRIBUTETYPE_H
#define OTTERENGINE_TYPES_SHADERATTRIBUTETYPE_H

#include "Core/Types.h"

#define SHADER_ATTRIBUTE_TYPE_LIST  \
    REPLACE_WITH(Float, 0x01)       \
    REPLACE_WITH(Float2, 0x02)      \
    REPLACE_WITH(Float3, 0x03)      \
    REPLACE_WITH(Float4, 0x04)      \
    REPLACE_WITH(Matrix3x3, 0x05)   \
    REPLACE_WITH(Matrix4x4, 0x06)   \
    REPLACE_WITH(Int, 0x07)         \
    REPLACE_WITH(Int2, 0x08)        \
    REPLACE_WITH(Int3, 0x09)        \
    REPLACE_WITH(Int4, 0x10)        \
    REPLACE_WITH(Bool, 0x11)

namespace Otter::Graphics
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

    enum class ShaderAttributeType : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        SHADER_ATTRIBUTE_TYPE_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };
}

#pragma clang diagnostic pop

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::Graphics::ShaderAttributeType& attributeType)
{
    switch (attributeType)
    {
#define REPLACE_WITH(Item, Value) case Otter::Graphics::ShaderAttributeType::Item: os << "ShaderAttributeType::" << #Item; break;
        SHADER_ATTRIBUTE_TYPE_LIST
#undef REPLACE_WITH
        default:
            os << "ShaderAttributeType[Unknown]";
    }

    return os;
}
#undef SHADER_ATTRIBUTE_TYPE_LIST

#endif //OTTERENGINE_TYPES_SHADERATTRIBUTETYPE_H
