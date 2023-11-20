#ifndef OTTERENGINE_TYPES_EVENTCATEGORY_H
#define OTTERENGINE_TYPES_EVENTCATEGORY_H

#include "Core/Defines.h"

namespace Otter::Internal
{
    enum EventCategory
    {
        None = OTR_BITMASK(0),

        Window   = OTR_BITMASK(1),
        Input    = OTR_BITMASK(2),
        Keyboard = OTR_BITMASK(3),
        Mouse    = OTR_BITMASK(4),
        // TODO: Some categories to think about for later?
        // File     = OTR_BIT(5),
        // System   = OTR_BIT(6),
        Blocking = OTR_BITMASK(30),

        All = OTR_BITMASK(31)
    };
}

#endif //OTTERENGINE_TYPES_EVENTCATEGORY_H
