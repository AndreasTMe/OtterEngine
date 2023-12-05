#ifndef OTTERENGINE_TYPES_EVENTTYPE_H
#define OTTERENGINE_TYPES_EVENTTYPE_H

#include "Core/Types/Types.Base.h"

#define EVENT_TYPE_LIST                     \
    /* Window Events */                     \
    REPLACE_WITH(WindowClose,0x01)          \
    REPLACE_WITH(WindowResize,0x02)         \
    REPLACE_WITH(WindowMinimized,0x03)      \
    REPLACE_WITH(WindowMaximized,0x04)      \
    REPLACE_WITH(WindowRestored,0x05)       \
                                            \
    /* Keyboard Events */                   \
    REPLACE_WITH(KeyPressed,0x06)           \
    REPLACE_WITH(KeyReleased,0x07)          \
    REPLACE_WITH(KeyHold,0x08)              \
                                            \
    /* Mouse Events */                      \
    REPLACE_WITH(MouseButtonPressed,0x09)   \
    REPLACE_WITH(MouseButtonReleased,0x0A)  \
    REPLACE_WITH(MouseScroll,0x0B)          \
    REPLACE_WITH(MouseMoved,0x0C)

// TODO: Some event types to think about for later?
// Joystick Events
// Controller Events
// Touch Events
// File Events
// Log Events

namespace Otter
{
    enum class EventType : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        EVENT_TYPE_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::EventType& eventType)
{
    switch (eventType)
    {
#define REPLACE_WITH(Item, Value) case Otter::EventType::Item: os << "EventType::" << #Item; break;
        EVENT_TYPE_LIST
#undef REPLACE_WITH
        default:
            os << "EventType[Unknown]";
    }

    return os;
}
#undef EVENT_TYPE_LIST

#endif //OTTERENGINE_TYPES_EVENTTYPE_H
