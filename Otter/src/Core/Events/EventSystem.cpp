#include "Otter.PCH.h"

#include "Core/Events/EventSystem.h"

namespace Otter
{
    void EventSystem::Initialise()
    {
        static bool isInitialised = false;
        OTR_INTERNAL_ASSERT_MSG(!isInitialised, "Event system has already been initialised")

        // Window Events
        AddListener(Internal::EventType::WindowClose, GlobalActions::OnWindowClose);
        AddListener(Internal::EventType::WindowResize, GlobalActions::OnWindowResize);
        AddListener(Internal::EventType::WindowMinimized, GlobalActions::OnWindowMinimized);
        AddListener(Internal::EventType::WindowMaximized, GlobalActions::OnWindowMaximized);
        AddListener(Internal::EventType::WindowRestored, GlobalActions::OnWindowRestored);
        AddListener(Internal::EventType::WindowRefresh, GlobalActions::OnWindowRefresh);

        // Keyboard Events
        AddListener(Internal::EventType::KeyPressed, GlobalActions::OnKeyPressed);
        AddListener(Internal::EventType::KeyReleased, GlobalActions::OnKeyReleased);
        AddListener(Internal::EventType::KeyHold, GlobalActions::OnKeyHold);

        // Mouse Events
        AddListener(Internal::EventType::MouseButtonPressed, GlobalActions::OnMouseButtonPressed);
        AddListener(Internal::EventType::MouseButtonReleased, GlobalActions::OnMouseButtonReleased);
        AddListener(Internal::EventType::MouseScroll, GlobalActions::OnMouseScroll);
        AddListener(Internal::EventType::MouseMoved, GlobalActions::OnMouseMoved);
        AddListener(Internal::EventType::MouseDragged, GlobalActions::OnMouseDragged);

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

            if (m_Listeners.find(type) != m_Listeners.end())
                ((Func<bool, const Event&>*) m_Listeners[type].m_Pointer)->ReverseInvoke(event);

            m_Events.DequeueUnsafe();
        }

        m_Events.Clear();
    }

    template<typename TEvent, typename TActionArg>
    void EventSystem::AddListener(EventType type, const Func<bool, TActionArg>& action)
    {
        OTR_INTERNAL_ASSERT_MSG(type != Internal::EventType::None, "Event type cannot be None")
        OTR_INTERNAL_ASSERT_MSG(type != Internal::EventType::Max, "Event type cannot be Max")

        if (m_Listeners.find(type) != m_Listeners.end())
            return;

        UnsafeHandle handle = Unsafe::New(sizeof(Func<bool, const Event&>));
        handle.m_Pointer = new(handle.m_Pointer) Func<bool, TActionArg>(action);
        m_Listeners[type] = handle;
    }

    void EventSystem::RemoveAllListeners()
    {
        for (auto& [_, handle]: m_Listeners)
            Unsafe::Delete(handle);

        m_Listeners.clear();
    }

    void EventSystem::ClearEvents()
    {
        m_Events.ClearDestructive();
    }
}