#include "Otter.PCH.h"

#include "Core/Events/EventSystem.h"

namespace Otter
{
    using Event = Internal::Event;
    using EventType = Internal::EventType;

    Dictionary<EventType, UnsafeHandle> g_Listeners;

    void EventSystem::Initialise()
    {
        static bool isInitialised = false;
        OTR_INTERNAL_ASSERT_MSG(!isInitialised, "Event system has already been initialised")

        g_Listeners = Dictionary<EventType, UnsafeHandle>(14);

        // Window Events
        AddListener(EventType::WindowClose, GlobalActions::OnWindowClose);
        AddListener(EventType::WindowResize, GlobalActions::OnWindowResize);
        AddListener(EventType::WindowMinimized, GlobalActions::OnWindowMinimized);
        AddListener(EventType::WindowMaximized, GlobalActions::OnWindowMaximized);
        AddListener(EventType::WindowRestored, GlobalActions::OnWindowRestored);
        AddListener(EventType::WindowRefresh, GlobalActions::OnWindowRefresh);

        // Keyboard Events
        AddListener(EventType::KeyPressed, GlobalActions::OnKeyPressed);
        AddListener(EventType::KeyReleased, GlobalActions::OnKeyReleased);
        AddListener(EventType::KeyHold, GlobalActions::OnKeyHold);

        // Mouse Events
        AddListener(EventType::MouseButtonPressed, GlobalActions::OnMouseButtonPressed);
        AddListener(EventType::MouseButtonReleased, GlobalActions::OnMouseButtonReleased);
        AddListener(EventType::MouseScroll, GlobalActions::OnMouseScroll);
        AddListener(EventType::MouseMoved, GlobalActions::OnMouseMoved);
        AddListener(EventType::MouseDragged, GlobalActions::OnMouseDragged);

        isInitialised = true;
    }

    void EventSystem::Shutdown()
    {
        RemoveAllListeners();
        ClearEvents();

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
        while (m_Events.TryPeek(event))
        {
            auto type = event.GetEventType();

            UnsafeHandle handle{ };
            if (g_Listeners.TryGet(type, handle))
                ((Func<bool, const Event&>*) handle.m_Pointer)->ReverseInvoke(event);

            m_Events.TryDequeue();
        }
    }

    template<typename TEvent, typename TActionArg>
    void EventSystem::AddListener(EventType type, const Func<bool, TActionArg>& action)
    {
        OTR_INTERNAL_ASSERT_MSG(type != EventType::None, "Event type cannot be None")
        OTR_INTERNAL_ASSERT_MSG(type != EventType::Max, "Event type cannot be Max")

        if (g_Listeners.Contains(type))
            return;

        UnsafeHandle handle = Unsafe::New(sizeof(Func<bool, const Event&>));
        handle.m_Pointer = new(handle.m_Pointer) Func<bool, TActionArg>(action);
        g_Listeners.TryAdd(type, handle);
    }

    void EventSystem::RemoveAllListeners()
    {
        Action<const UnsafeHandle&> deleteAction{ };
        deleteAction += [](const UnsafeHandle& handle)
        {
            Unsafe::Delete(handle);
        };

        g_Listeners.ForEachValue(deleteAction);
        g_Listeners.ClearDestructive();
    }

    void EventSystem::ClearEvents()
    {
        m_Events.ClearDestructive();
    }
}