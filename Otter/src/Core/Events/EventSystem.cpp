#include "Core/Events/EventSystem.h"

#include "Core/GlobalActions.h"

namespace Otter
{
    Queue<Event>                        EventSystem::s_Events{ };
    Span<Func<bool, const Event&>*, 12> EventSystem::s_EventListeners{ };

    bool EventSystem::s_IsInitialised = false;
    bool EventSystem::s_BlockEvents   = false;

    void EventSystem::Initialise()
    {
        OTR_INTERNAL_ASSERT_MSG(!s_IsInitialised, "Event system has already been initialised")

        // Window Events
        AddListener(EventType::WindowClose, &OTR_GLOBAL_ACTIONS.OnWindowClose);
        AddListener(EventType::WindowResize, &OTR_GLOBAL_ACTIONS.OnWindowResize);
        AddListener(EventType::WindowMinimized, &OTR_GLOBAL_ACTIONS.OnWindowMinimized);
        AddListener(EventType::WindowMaximized, &OTR_GLOBAL_ACTIONS.OnWindowMaximized);
        AddListener(EventType::WindowRestored, &OTR_GLOBAL_ACTIONS.OnWindowRestored);

        // Keyboard Events
        AddListener(EventType::KeyPressed, &OTR_GLOBAL_ACTIONS.OnKeyPressed);
        AddListener(EventType::KeyReleased, &OTR_GLOBAL_ACTIONS.OnKeyReleased);
        AddListener(EventType::KeyRepeat, &OTR_GLOBAL_ACTIONS.OnKeyRepeat);

        // Mouse Events
        AddListener(EventType::MouseButtonPressed, &OTR_GLOBAL_ACTIONS.OnMouseButtonPressed);
        AddListener(EventType::MouseButtonReleased, &OTR_GLOBAL_ACTIONS.OnMouseButtonReleased);
        AddListener(EventType::MouseScroll, &OTR_GLOBAL_ACTIONS.OnMouseScroll);
        AddListener(EventType::MouseMoved, &OTR_GLOBAL_ACTIONS.OnMouseMoved);

        s_IsInitialised = true;

        OTR_LOG_DEBUG("Event system initialised")
    }

    void EventSystem::Shutdown()
    {
        OTR_INTERNAL_ASSERT_MSG(s_IsInitialised, "Event system has not been initialised")

        OTR_LOG_DEBUG("Shutting down event system...")

        s_Events.ClearDestructive();

        OTR_GLOBAL_ACTIONS.OnMouseMoved.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnMouseScroll.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnMouseButtonReleased.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnMouseButtonPressed.ClearDestructive();

        OTR_GLOBAL_ACTIONS.OnKeyRepeat.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnKeyReleased.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnKeyPressed.ClearDestructive();

        OTR_GLOBAL_ACTIONS.OnWindowRestored.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnWindowMaximized.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnWindowMinimized.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnWindowResize.ClearDestructive();
        OTR_GLOBAL_ACTIONS.OnWindowClose.ClearDestructive();

        s_IsInitialised = false;
    }

    void EventSystem::Process()
    {
        Event event;
        while (s_Events.TryPeek(&event))
        {
            auto type = event.GetEventType();
            if (type == EventType::None || type == EventType::Max)
            {
                s_Events.TryDequeue();
                continue;
            }

            auto* callback = s_EventListeners[static_cast<UInt64>(type) - 1];
            if (callback)
                callback->ReverseInvoke(event);

            s_Events.TryDequeue();
        }

        s_BlockEvents = false;
    }

    template<typename TEvent, typename TActionArg>
    requires IsBaseOf<Event, TEvent>
    void EventSystem::AddListener(EventType type, Func<bool, TActionArg>* action)
    {
        OTR_INTERNAL_ASSERT_MSG(type != EventType::None, "Event type cannot be None")
        OTR_INTERNAL_ASSERT_MSG(type != EventType::Max, "Event type cannot be Max")

        if (action)
            s_EventListeners[static_cast<UInt64>(type) - 1] = reinterpret_cast<Func<bool, const Event&>*>(action);
    }
}