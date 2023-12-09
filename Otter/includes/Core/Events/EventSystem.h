#ifndef OTTERENGINE_EVENTSYSTEM_H
#define OTTERENGINE_EVENTSYSTEM_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Delegates.h"
#include "Core/Collections/Span.h"
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
        requires IsBaseOf<Event, TEvent>
        static void Schedule(TArgs&& ... args)
        {
            if (s_BlockEvents)
                return;

            auto e = (Event) TEvent(std::forward<TArgs>(args)...);
            if (e.IsBlocking())
                s_BlockEvents = true;

            s_Events.TryEnqueue(e);
        }

        static void Process();

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(EventSystem)

        static Queue<Event>                        s_Events;
        static Span<Func<bool, const Event&>*, 12> s_EventListeners;

        static bool s_IsInitialised;
        static bool s_BlockEvents;

        template<typename TEvent = Event, typename TActionArg = const TEvent&>
        requires IsBaseOf<Event, TEvent>
        static void AddListener(EventType type, Func<bool, TActionArg>* action);
    };
}

#endif //OTTERENGINE_EVENTSYSTEM_H
