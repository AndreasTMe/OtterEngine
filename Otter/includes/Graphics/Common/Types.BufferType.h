#ifndef OTTERENGINE_TYPES_BUFFERTYPE_H
#define OTTERENGINE_TYPES_BUFFERTYPE_H

#include "Core/Types.h"

#define BUFFER_TYPE_LIST        \
    REPLACE_WITH(Vertex, 0x01)  \
    REPLACE_WITH(Index, 0x02)   \
    REPLACE_WITH(Uniform, 0x03)

namespace Otter::Graphics
{
    enum class BufferType : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        BUFFER_TYPE_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::Graphics::BufferType& bufferType)
{
    switch (bufferType)
    {
#define REPLACE_WITH(Item, Value) case Otter::Graphics::BufferType::Item: os << "BufferType::" << #Item; break;
        BUFFER_TYPE_LIST
#undef REPLACE_WITH
        default:
            os << "BufferType[Unknown]";
    }

    return os;
}
#undef BUFFER_TYPE_LIST

#endif //OTTERENGINE_TYPES_BUFFERTYPE_H
