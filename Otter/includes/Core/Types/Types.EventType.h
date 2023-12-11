#ifndef OTTERENGINE_TYPES_EVENTTYPE_H
#define OTTERENGINE_TYPES_EVENTTYPE_H

#include "Core/Defines/Defines.Utilities.h"
#include "Core/Types/Types.Base.h"

namespace Otter
{
    /**
     * @brief The type of event that has occurred. Its size is 1 byte.
     */
    enum class EventType : UInt8
    {
        /// @brief No event.
        None = 0x00,

        /// @brief Window was closed.
        WindowClose = 0x01,

        /// @brief Window was resized.
        WindowResize = 0x02,

        /// @brief Window was minimized.
        WindowMinimized = 0x03,

        /// @brief Window was maximized.
        WindowMaximized = 0x04,

        /// @brief Window was restored.
        WindowRestored = 0x05,

        /// @brief Key was pressed.
        KeyPressed = 0x06,

        /// @brief Key was released.
        KeyReleased = 0x07,

        /// @brief Key was repeated.
        KeyRepeat = 0x08,

        /// @brief Mouse button was pressed.
        MouseButtonPressed = 0x09,

        /// @brief Mouse button was released.
        MouseButtonReleased = 0x0A,

        /// @brief Mouse was scrolled.
        MouseScroll = 0x0B,

        /// @brief Mouse was moved.
        MouseMoved = 0x0C,

        /// @brief The maximum value of the enum.
        Max = 0xFF
    };

    // TODO: Some event types to think about for later?
    // Joystick Events
    // Controller Events
    // Touch Events
    // File Events
    // Log Events
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::EventType& eventType)
{
    switch (eventType)
    {
        case Otter::EventType::None:
            os << OTR_NAME_OF(Otter::EventType::None);
            break;
        case Otter::EventType::WindowClose:
            os << OTR_NAME_OF(Otter::EventType::WindowClose);
            break;
        case Otter::EventType::WindowResize:
            os << OTR_NAME_OF(Otter::EventType::WindowResize);
            break;
        case Otter::EventType::WindowMinimized:
            os << OTR_NAME_OF(Otter::EventType::WindowMinimized);
            break;
        case Otter::EventType::WindowMaximized:
            os << OTR_NAME_OF(Otter::EventType::WindowMaximized);
            break;
        case Otter::EventType::WindowRestored:
            os << OTR_NAME_OF(Otter::EventType::WindowRestored);
            break;
        case Otter::EventType::KeyPressed:
            os << OTR_NAME_OF(Otter::EventType::KeyPressed);
            break;
        case Otter::EventType::KeyReleased:
            os << OTR_NAME_OF(Otter::EventType::KeyReleased);
            break;
        case Otter::EventType::KeyRepeat:
            os << OTR_NAME_OF(Otter::EventType::KeyRepeat);
            break;
        case Otter::EventType::MouseButtonPressed:
            os << OTR_NAME_OF(Otter::EventType::MouseButtonPressed);
            break;
        case Otter::EventType::MouseButtonReleased:
            os << OTR_NAME_OF(Otter::EventType::MouseButtonReleased);
            break;
        case Otter::EventType::MouseScroll:
            os << OTR_NAME_OF(Otter::EventType::MouseScroll);
            break;
        case Otter::EventType::MouseMoved:
            os << OTR_NAME_OF(Otter::EventType::MouseMoved);
            break;
        case Otter::EventType::Max:
            os << OTR_NAME_OF(Otter::EventType::Max);
            break;
        default:
            os << "Unknown EventType";
    }

    return os;
}

#endif //OTTERENGINE_TYPES_EVENTTYPE_H
