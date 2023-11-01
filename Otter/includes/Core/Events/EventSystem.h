#ifndef OTTERENGINE_EVENTSYSTEM_H
#define OTTERENGINE_EVENTSYSTEM_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Delegates.h"
#include "Core/Collections/Queue.h"

#include "Core/Events/Event.h"
#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyboardEvents.h"
#include "Core/Events/MouseEvents.h"

namespace Otter
{
    class EventSystem final
    {
    public:
        OTR_DISABLE_OBJECT_COPIES(EventSystem)
        OTR_DISABLE_OBJECT_MOVES(EventSystem)

        static void Initialise();
        static void Shutdown();

        template<typename TEvent, typename... TArgs>
        static void Schedule(TArgs&& ... args)
        {
            s_Events.Enqueue(TEvent(std::forward<TArgs>(args)...));
        }

        static void Process();

    private:
        OTR_DISABLE_CONSTRUCTION(EventSystem)

        static Queue<Internal::Event> s_Events;
    };
}

#endif //OTTERENGINE_EVENTSYSTEM_H
