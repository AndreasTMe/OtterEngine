#ifndef OTTERENGINE_EVENTACTION_H
#define OTTERENGINE_EVENTACTION_H

#include "Core/Defines.h"
#include "Core/Events/Event.h"

namespace Otter::Internal
{
    class EventAction final : public Action<const Event&>
    {
    public:
        OTR_INLINE explicit EventAction(EventAction* other)
            : Action<const Event&>(*other)
        {
        }
    };
}

#endif //OTTERENGINE_EVENTACTION_H
