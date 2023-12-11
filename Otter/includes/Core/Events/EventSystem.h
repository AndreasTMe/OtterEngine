#ifndef OTTERENGINE_EVENTSYSTEM_H
#define OTTERENGINE_EVENTSYSTEM_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Delegates.h"
#include "Core/Collections/Span.h"
#include "Core/Collections/Queue.h"
#include "Core/Events/Event.h"

namespace Otter
{
    /**
     * @brief The application's system that is responsible for handling events.
     */
    class EventSystem final
    {
    public:
        OTR_DISABLE_OBJECT_COPIES(EventSystem)
        OTR_DISABLE_OBJECT_MOVES(EventSystem)

        /**
         * @brief Initialises the event system.
         */
        static void Initialise();

        /**
         * @brief Shuts down the event system.
         */
        static void Shutdown();

        /**
         * @brief Schedules an event for the specified event type. If the event is blocking, all events scheduled
         * after it will be ignored until the event is processed.
         *
         * @tparam TEvent The event type.
         * @tparam TArgs The event arguments.
         *
         * @param args The event arguments to pass to the event constructor.
         */
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

        /**
         * @brief Processes all events in the event queue. The events are processed in the order they were scheduled.
         * The event queue is cleared after all events have been processed.
         */
        static void Process();

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(EventSystem)

        static Queue<Event>                        s_Events;
        static Span<Func<bool, const Event&>*, 12> s_EventListeners;

        static bool s_IsInitialised;
        static bool s_BlockEvents;

        /**
         * @brief Adds an event listener for the specified event type. The event listener will be called when an event
         * of the specified type is processed. The event listeners return a boolean value indicating whether the event
         * should be processed by other listeners.
         *
         * @tparam TEvent The event type.
         * @tparam TActionArg The event callback argument type.
         *
         * @param type The event type.
         * @param action The event listener.
         */
        template<typename TEvent = Event, typename TActionArg = const TEvent&>
        requires IsBaseOf<Event, TEvent>
        static void AddListener(EventType type, Func<bool, TActionArg>* action);
    };
}

#endif //OTTERENGINE_EVENTSYSTEM_H
