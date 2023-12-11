#ifndef OTTERENGINE_EVENTCATEGORY_H
#define OTTERENGINE_EVENTCATEGORY_H

#include "Core/Defines.h"

namespace Otter
{
    /**
     * @brief Represents the category of an event. Each category is represented by a bit in an integer value,
     * allowing for easy combining of categories using bitwise operators.
     */
    enum EventCategory
    {
        /// @brief No category.
        None = 0,

        /// @brief The event is related to the window.
        Window = OTR_BITMASK(0),

        /// @brief The event is related to input.
        Input = OTR_BITMASK(1),

        /// @brief The event is related to keyboard input.
        Keyboard = OTR_BITMASK(2),

        /// @brief The event is related to mouse input.
        Mouse = OTR_BITMASK(3),

        /// @brief The event is blocking any incoming events.
        Blocking = OTR_BITMASK(30),

        /// @brief All categories.
        All = OTR_BITMASK(31)
    };

    // TODO: Some event categories to think about for later?
    // File     = OTR_BIT(4),
    // System   = OTR_BIT(5),
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::EventCategory& eventCategory)
{
    if (eventCategory == Otter::EventCategory::None)
    {
        os << OTR_NAME_OF(Otter::EventCategory::None);
        return os;
    }
    else if (eventCategory == Otter::EventCategory::All)
    {
        os << OTR_NAME_OF(Otter::EventCategory::All);
        return os;
    }

    bool hasCategory = false;

    if (eventCategory & Otter::EventCategory::Window)
    {
        os << OTR_NAME_OF(Otter::EventCategory::Window);
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Input)
    {
        os << (!hasCategory ? "" : " | ") << OTR_NAME_OF(Otter::EventCategory::Input);
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Keyboard)
    {
        os << (!hasCategory ? "" : " | ") << OTR_NAME_OF(Otter::EventCategory::Keyboard);
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Mouse)
    {
        os << (!hasCategory ? "" : " | ") << OTR_NAME_OF(Otter::EventCategory::Mouse);
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Blocking)
    {
        os << (!hasCategory ? "" : " | ") << OTR_NAME_OF(Otter::EventCategory::Blocking);
        hasCategory = true;
    }

    if (!hasCategory)
        os << "Unknown EventCategory";

    return os;
}
#undef EVENT_CATEGORY_LIST

#endif //OTTERENGINE_EVENTCATEGORY_H
