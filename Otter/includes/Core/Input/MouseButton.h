#ifndef OTTERENGINE_MOUSEBUTTON_H
#define OTTERENGINE_MOUSEBUTTON_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"

namespace Otter
{
    /**
     * @brief The mouse button type. Its size is 1 byte.
     */
    enum class MouseButton : UInt8
    {
        /// @brief No mouse button.
        None = 0x00,

        /// @brief The left mouse button.
        Left = 0x01,

        /// @brief The middle mouse button.
        Middle = 0x02,

        /// @brief The right mouse button.
        Right = 0x03,

        /// @brief The maximum value of the enum.
        Max = 0xFF
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::MouseButton& mouseButton)
{
    switch (mouseButton)
    {
        case Otter::MouseButton::None:
            os << OTR_NAME_OF(Otter::MouseButton::None);
            break;
        case Otter::MouseButton::Left:
            os << OTR_NAME_OF(Otter::MouseButton::Left);
            break;
        case Otter::MouseButton::Middle:
            os << OTR_NAME_OF(Otter::MouseButton::Middle);
            break;
        case Otter::MouseButton::Right:
            os << OTR_NAME_OF(Otter::MouseButton::Right);
            break;
        case Otter::MouseButton::Max:
            os << OTR_NAME_OF(Otter::MouseButton::Max);
            break;
        default:
            os << "Unknown MouseButton";
    }

    return os;
}

#endif //OTTERENGINE_MOUSEBUTTON_H
