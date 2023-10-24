#ifndef OTTERENGINE_EVENTSYSTEM_H
#define OTTERENGINE_EVENTSYSTEM_H

#include <queue>
#include <unordered_map>

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Delegates.h"
#include "Core/Collections.h"

#include "Core/Events/Event.h"
#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyboardEvents.h"
#include "Core/Events/MouseEvents.h"

namespace Otter
{
    class EventSystem final
    {
        using Event = Internal::Event;
        using EventType = Internal::EventType;

    public:
        OTR_DISABLE_OBJECT_COPIES(EventSystem)
        OTR_DISABLE_OBJECT_MOVES(EventSystem)

        [[nodiscard]] OTR_INLINE static EventSystem* GetInstance()
        {
            static EventSystem instance;
            return &instance;
        }

        void Initialise();
        void Shutdown();

        template<typename TEvent, typename... TArgs>
        OTR_INLINE void Schedule(TArgs&& ... args)
        {
            m_Events.Push(TEvent(std::forward<TArgs>(args)...));
        }

        void Process();

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(EventSystem)

        // TODO: Replace with a priority queue.
        Queue<Event> m_Events{ };

        // TODO: Replace with a custom map.
        std::unordered_map<EventType, UnsafeHandle> m_Listeners;

        template<typename TEvent = Event, typename TActionArg = const TEvent&>
        void AddListener(EventType type, const Func<bool, TActionArg>& action);
        void RemoveAllListeners();
        void ClearEvents();
    };
}

#endif //OTTERENGINE_EVENTSYSTEM_H
