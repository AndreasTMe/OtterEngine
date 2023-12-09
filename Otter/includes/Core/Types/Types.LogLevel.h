#ifndef OTTERENGINE_TYPES_LOGLEVEL_H
#define OTTERENGINE_TYPES_LOGLEVEL_H

#include "Core/Defines/Defines.Utilities.h"
#include "Core/Types/Types.Base.h"

namespace Otter
{
    /**
     * @brief The type of the log level. Its size is 1 byte.
     */
    enum class LogLevel : UInt8
    {
        /// @brief The trace log level.
        Trace = 0x00,

        /// @brief The debug log level.
        Debug = 0x01,

        /// @brief The info log level.
        Info = 0x02,

        /// @brief The warning log level.
        Warning = 0x03,

        /// @brief The error log level.
        Error = 0x04,

        /// @brief The fatal log level.
        Fatal = 0x05,
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::LogLevel& logLevel)
{
    switch (logLevel)
    {
        case Otter::LogLevel::Trace:
            os << OTR_NAME_OF(Otter::LogLevel::Trace);
            break;
        case Otter::LogLevel::Debug:
            os << OTR_NAME_OF(Otter::LogLevel::Debug);
            break;
        case Otter::LogLevel::Info:
            os << OTR_NAME_OF(Otter::LogLevel::Info);
            break;
        case Otter::LogLevel::Warning:
            os << OTR_NAME_OF(Otter::LogLevel::Warning);
            break;
        case Otter::LogLevel::Error:
            os << OTR_NAME_OF(Otter::LogLevel::Error);
            break;
        case Otter::LogLevel::Fatal:
            os << OTR_NAME_OF(Otter::LogLevel::Fatal);
            break;
        default:
            os << "Unknown LogLevel";
    }

    return os;
}

#endif //OTTERENGINE_TYPES_LOGLEVEL_H
