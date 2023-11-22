#ifndef OTTERENGINE_TYPES_SHADERATTRIBUTESIZE_H
#define OTTERENGINE_TYPES_SHADERATTRIBUTESIZE_H

#include "Core/Types.h"

#define SHADER_ATTRIBUTE_SIZE_LIST  \
    REPLACE_WITH(Bit8, 1)           \
    REPLACE_WITH(Bit16, 2)          \
    REPLACE_WITH(Bit32, 4)          \
    REPLACE_WITH(Bit64, 8)

namespace Otter::Graphics
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

    enum class ShaderAttributeSize : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        SHADER_ATTRIBUTE_SIZE_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };
}

#pragma clang diagnostic pop

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::Graphics::ShaderAttributeSize& attributeSize)
{
    switch (attributeSize)
    {
#define REPLACE_WITH(Item, Value) case Otter::Graphics::ShaderAttributeSize::Item: os << "ShaderAttributeSize::" << #Item; break;
        SHADER_ATTRIBUTE_SIZE_LIST
#undef REPLACE_WITH
        default:
            os << "ShaderAttributeSize[Unknown]";
    }

    return os;
}
#undef SHADER_ATTRIBUTE_SIZE_LIST

#endif //OTTERENGINE_TYPES_SHADERATTRIBUTESIZE_H
