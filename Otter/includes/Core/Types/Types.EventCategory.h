#ifndef OTTERENGINE_TYPES_EVENTCATEGORY_H
#define OTTERENGINE_TYPES_EVENTCATEGORY_H

#include "Core/Defines.h"

namespace Otter
{
    enum EventCategory
    {
        None = 0,

        Window   = OTR_BITMASK(0),
        Input    = OTR_BITMASK(1),
        Keyboard = OTR_BITMASK(2),
        Mouse    = OTR_BITMASK(3),
        // TODO: Some event categories to think about for later?
        // File     = OTR_BIT(4),
        // System   = OTR_BIT(5),
        Blocking = OTR_BITMASK(30),

        All = OTR_BITMASK(31)
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::EventCategory& eventCategory)
{
    if (eventCategory == Otter::EventCategory::None)
    {
        os << "EventCategory::None";
        return os;
    }
    else if (eventCategory == Otter::EventCategory::All)
    {
        os << "EventCategory::All";
        return os;
    }

    bool hasCategory = false;

    if (eventCategory & Otter::EventCategory::Window)
    {
        os << "EventCategory::Window";
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Input)
    {
        os << (!hasCategory ? "" : " | ") << "EventCategory::Input";
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Keyboard)
    {
        os << (!hasCategory ? "" : " | ") << "EventCategory::Keyboard";
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Mouse)
    {
        os << (!hasCategory ? "" : " | ") << "EventCategory::Mouse";
        hasCategory = true;
    }
    if (eventCategory & Otter::EventCategory::Blocking)
    {
        os << (!hasCategory ? "" : " | ") << "EventCategory::Blocking";
        hasCategory = true;
    }

    if (!hasCategory)
        os << "EventCategory[Unknown]";

    return os;
}
#undef EVENT_CATEGORY_LIST

#endif //OTTERENGINE_TYPES_EVENTCATEGORY_H
