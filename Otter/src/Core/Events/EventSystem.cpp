#include "Otter.PCH.h"

#include "Core/Events/EventSystem.h"

namespace Otter
{
    void EventSystem::Initialise()
    {
        OTR_INTERNAL_ASSERT_MSG(!m_IsInitialised, "Event system has already been initialised")

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

        m_IsInitialised = true;

        OTR_LOG_DEBUG("Event system initialised")
    }

    void EventSystem::Shutdown()
    {
        OTR_INTERNAL_ASSERT_MSG(m_IsInitialised, "Event system has not been initialised")

        OTR_LOG_DEBUG("Shutting down event system...")

        m_Events.ClearDestructive();

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

        m_IsInitialised = false;
    }

    void EventSystem::Process()
    {
        Event event;
        while (m_Events.TryPeek(&event))
        {
            auto type = event.GetEventType();
            if (type == EventType::None || type == EventType::Max)
            {
                m_Events.TryDequeue();
                continue;
            }

            auto* callback = m_EventListeners[static_cast<UInt64>(type) - 1];
            if (callback)
                callback->ReverseInvoke(event);

            m_Events.TryDequeue();
        }

        m_BlockEvents = false;
    }

    template<typename TEvent, typename TActionArg>
    requires IsBaseOf<Event, TEvent>
    void EventSystem::AddListener(EventType type, Func<bool, TActionArg>* action)
    {
        OTR_INTERNAL_ASSERT_MSG(type != EventType::None, "Event type cannot be None")
        OTR_INTERNAL_ASSERT_MSG(type != EventType::Max, "Event type cannot be Max")

        if (action)
            m_EventListeners[static_cast<UInt64>(type) - 1] = reinterpret_cast<Func<bool, const Event&>*>(action);
    }
}