#ifndef OTTERENGINE_EVENTSYSTEM_H
#define OTTERENGINE_EVENTSYSTEM_H

#include <unordered_map>

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

        [[nodiscard]] OTR_INLINE static EventSystem* GetInstance()
        {
            static EventSystem instance;
            return &instance;
        }

        void Initialise();
        void Shutdown();

        template<typename TEvent, typename... TArgs>
        void Schedule(TArgs&& ... args)
        {
            m_Events.Enqueue(TEvent(std::forward<TArgs>(args)...));
        }

        void Process();

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(EventSystem)

        Queue<Internal::Event> m_Events{ };

        template<typename TEvent = Internal::Event, typename TActionArg = const TEvent&>
        void AddListener(Internal::EventType type, const Func<bool, TActionArg>& action);
        void RemoveAllListeners();
        void ClearEvents();
    };
}

#endif //OTTERENGINE_EVENTSYSTEM_H
