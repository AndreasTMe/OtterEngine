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

        OTR_INLINE static EventSystem& GetInstance()
        {
            static EventSystem instance;
            return instance;
        }

        void Initialise();
        void Shutdown();

        template<typename TEvent, typename... TArgs>
        requires IsBaseOf<Event, TEvent>
        void Schedule(TArgs&& ... args)
        {
            if (m_BlockEvents)
                return;

            auto e = (Event) TEvent(std::forward<TArgs>(args)...);
            if (e.IsBlocking())
                m_BlockEvents = true;

            m_Events.TryEnqueue(e);
        }

        void Process();

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(EventSystem)

#define OTR_EVENT_TYPES_COUNT 12

        Queue<Event>                                            m_Events;
        Array<Func<bool, const Event&>*, OTR_EVENT_TYPES_COUNT> m_EventListeners;

        bool m_BlockEvents = false;

#undef OTR_EVENT_TYPES_COUNT

        template<typename TEvent = Event, typename TActionArg = const TEvent&>
        requires IsBaseOf<Event, TEvent>
        void AddListener(EventType type, Func<bool, TActionArg>* action);
    };
}

#define OTR_EVENT_SYSTEM Otter::EventSystem::GetInstance()

#endif //OTTERENGINE_EVENTSYSTEM_H
