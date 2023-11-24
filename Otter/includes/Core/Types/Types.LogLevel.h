#ifndef OTTERENGINE_TYPES_LOGLEVEL_H
#define OTTERENGINE_TYPES_LOGLEVEL_H

#include "Core/Types/Types.Base.h"

#define LOG_LEVEL_LIST          \
    REPLACE_WITH(Trace,0x00)    \
    REPLACE_WITH(Debug,0x01)    \
    REPLACE_WITH(Info,0x02)     \
    REPLACE_WITH(Warning,0x03)  \
    REPLACE_WITH(Error,0x04)    \
    REPLACE_WITH(Fatal,0x05)

namespace Otter
{
    enum class LogLevel : UInt8
    {
#define REPLACE_WITH(Item, Value) Item = Value,
        LOG_LEVEL_LIST
#undef REPLACE_WITH
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::LogLevel& logLevel)
{
    switch (logLevel)
    {
#define REPLACE_WITH(Item, Value) case Otter::LogLevel::Item: os << "LogLevel::" << #Item; break;
        LOG_LEVEL_LIST
#undef REPLACE_WITH
        default:
            os << "LogLevel[Unknown]";
    }

    return os;
}
#undef LOG_LEVEL_LIST

#endif //OTTERENGINE_TYPES_LOGLEVEL_H
