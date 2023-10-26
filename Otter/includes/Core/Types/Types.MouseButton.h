#ifndef OTTERENGINE_TYPES_MOUSEBUTTON_H
#define OTTERENGINE_TYPES_MOUSEBUTTON_H

#include "Core/Types/Types.Base.h"

namespace Otter
{
#define MOUSE_BUTTON_LIST       \
    REPLACE_WITH(Left,0x01)     \
    REPLACE_WITH(Middle,0x02)   \
    REPLACE_WITH(Right,0x03)

    enum class MouseButton : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        MOUSE_BUTTON_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };

    template<typename OStream>
    OStream& operator<<(OStream& os, const Otter::MouseButton& mouseButton)
    {
        switch (mouseButton)
        {
#define REPLACE_WITH(Item, Value) case Otter::MouseButton::Item: os << "MouseButton::" << #Item; break;
            MOUSE_BUTTON_LIST
#undef REPLACE_WITH
            default:
                os << "MouseButton[Unknown]";
        }

        return os;
    }
#undef KEYCODE_LIST
}

#endif //OTTERENGINE_TYPES_MOUSEBUTTON_H
