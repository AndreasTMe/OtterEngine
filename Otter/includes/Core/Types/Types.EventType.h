#ifndef OTTERENGINE_TYPES_EVENTTYPE_H
#define OTTERENGINE_TYPES_EVENTTYPE_H

#include "Core/Types/Types.Base.h"

#define EVENT_TYPE_LIST                     \
    /* Window Events (Use 0x01 - 0x10) */   \
    REPLACE_WITH(WindowClose,0x01)          \
    REPLACE_WITH(WindowResize,0x02)         \
    REPLACE_WITH(WindowMinimized,0x03)      \
    REPLACE_WITH(WindowMaximized,0x04)      \
    REPLACE_WITH(WindowRestored,0x05)       \
    REPLACE_WITH(WindowRefresh,0x06)        \
                                            \
    /* Keyboard Events (Use 0x11 - 0x20) */ \
    REPLACE_WITH(KeyPressed,0x11)           \
    REPLACE_WITH(KeyReleased,0x12)          \
    REPLACE_WITH(KeyHold,0x13)              \
                                            \
    /* Mouse Events (Use 0x20 - 0x30) */    \
    REPLACE_WITH(MouseButtonPressed,0x21)   \
    REPLACE_WITH(MouseButtonReleased,0x22)  \
    REPLACE_WITH(MouseScroll,0x23)          \
    REPLACE_WITH(MouseMoved,0x24)           \
    REPLACE_WITH(MouseDragged,0x25)         \
    REPLACE_WITH(MouseDragStarted,0x26)     \
    REPLACE_WITH(MouseDragEnded,0x27)
// TODO: Maybe implement these events later? Update Event methods if necessary.
// MouseEnter         = 0x28,
// MouseExit          = 0x29,
// MouseDragEntered   = 0x2A,
// MouseDragExited    = 0x2B,

// Joystick Events (Use 0x30 - 0x40)
// Controller Events (Use 0x40 - 0x50)
// Touch Events (Use 0x50 - 0x60)
// File Events (Use 0x60 - 0x70)
// Log Events (Use 0x70 - 0x80)

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
