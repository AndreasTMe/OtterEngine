#include "Otter.PCH.h"

#include "Core/Events/EventSystem.h"

namespace Otter
{
    using Event = Internal::Event;
    using EventType = Internal::EventType;

    Queue<Event> EventSystem::s_Events{ };
    bool         EventSystem::s_BlockEvents = false;

    Dictionary<EventType, Func<bool, const Event&>*> g_Listeners;

    template<typename TEvent = Event, typename TActionArg = const TEvent&>
    void AddListener(EventType type, Func<bool, TActionArg>* action);

    void EventSystem::Initialise()
    {
        static bool isInitialised = false;
        OTR_INTERNAL_ASSERT_MSG(!isInitialised, "Event system has already been initialised")

        g_Listeners = Dictionary<EventType, Func<bool, const Event&> * >(14);

        // Window Events
        AddListener(EventType::WindowClose, &GlobalActions::OnWindowClose);
        AddListener(EventType::WindowResize, &GlobalActions::OnWindowResize);
        AddListener(EventType::WindowMinimized, &GlobalActions::OnWindowMinimized);
        AddListener(EventType::WindowMaximized, &GlobalActions::OnWindowMaximized);
        AddListener(EventType::WindowRestored, &GlobalActions::OnWindowRestored);
        AddListener(EventType::WindowRefresh, &GlobalActions::OnWindowRefresh);

        // Keyboard Events
        AddListener(EventType::KeyPressed, &GlobalActions::OnKeyPressed);
        AddListener(EventType::KeyReleased, &GlobalActions::OnKeyReleased);
        AddListener(EventType::KeyHold, &GlobalActions::OnKeyHold);

        // Mouse Events
        AddListener(EventType::MouseButtonPressed, &GlobalActions::OnMouseButtonPressed);
        AddListener(EventType::MouseButtonReleased, &GlobalActions::OnMouseButtonReleased);
        AddListener(EventType::MouseScroll, &GlobalActions::OnMouseScroll);
        AddListener(EventType::MouseMoved, &GlobalActions::OnMouseMoved);
        AddListener(EventType::MouseDragged, &GlobalActions::OnMouseDragged);

        isInitialised = true;

        OTR_LOG_DEBUG("Event system initialised")
    }

    void EventSystem::Shutdown()
    {
        OTR_LOG_DEBUG("Shutting down event system...")

        g_Listeners.ClearDestructive();
        s_Events.ClearDestructive();

        GlobalActions::OnMouseDragged.ClearDestructive();
        GlobalActions::OnMouseMoved.ClearDestructive();
        GlobalActions::OnMouseScroll.ClearDestructive();
        GlobalActions::OnMouseButtonReleased.ClearDestructive();
        GlobalActions::OnMouseButtonPressed.ClearDestructive();

        GlobalActions::OnKeyHold.ClearDestructive();
        GlobalActions::OnKeyReleased.ClearDestructive();
        GlobalActions::OnKeyPressed.ClearDestructive();

        GlobalActions::OnWindowRefresh.ClearDestructive();
        GlobalActions::OnWindowRestored.ClearDestructive();
        GlobalActions::OnWindowMaximized.ClearDestructive();
        GlobalActions::OnWindowMinimized.ClearDestructive();
        GlobalActions::OnWindowResize.ClearDestructive();
        GlobalActions::OnWindowClose.ClearDestructive();
    }

    void EventSystem::Process()
    {
        Event event;
        while (s_Events.TryPeek(event))
        {
            auto type = event.GetEventType();

            Func<bool, const Event&>* callback;
            if (g_Listeners.TryGet(type, callback))
                callback->ReverseInvoke(event);

            s_Events.TryDequeue();
        }

        s_BlockEvents = false;
    }

    template<typename TEvent, typename TActionArg>
    void AddListener(EventType type, Func<bool, TActionArg>* action)
    {
        OTR_INTERNAL_ASSERT_MSG(type != EventType::None, "Event type cannot be None")
        OTR_INTERNAL_ASSERT_MSG(type != EventType::Max, "Event type cannot be Max")

        if (g_Listeners.Contains(type))
            return;

        g_Listeners.TryAdd(type, (Func<bool, const Event&>*) action);
    }
}